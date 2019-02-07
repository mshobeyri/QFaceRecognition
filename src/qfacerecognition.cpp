#include "qfacerecognition.hpp"
#include "imageconvertor.hpp"

#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/string.h>

#include <QDebug>
#include <QDir>
#include <QImage>
#include <QPixmap>

using namespace std;
using namespace dlib;

template <
    template <int, template <typename> class, int, typename> class block,
    int N,
    template <typename> class BN,
    typename SUBNET>
using residual = add_prev1<block<N, BN, 1, tag1<SUBNET>>>;

template <
    template <int, template <typename> class, int, typename> class block,
    int N,
    template <typename> class BN,
    typename SUBNET>
using residual_down =
    add_prev2<avg_pool<2, 2, 2, 2, skip1<tag2<block<N, BN, 2, tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block =
    BN<con<N, 3, 3, 1, 1, relu<BN<con<N, 3, 3, stride, stride, SUBNET>>>>>;

template <int N, typename SUBNET>
using ares = relu<residual<block, N, affine, SUBNET>>;
template <int N, typename SUBNET>
using ares_down = relu<residual_down<block, N, affine, SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256, SUBNET>;
template <typename SUBNET>
using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
template <typename SUBNET>
using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
template <typename SUBNET>
using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<
    128,
    avg_pool_everything<alevel0<alevel1<alevel2<alevel3<alevel4<max_pool<
        3,
        3,
        2,
        2,
        relu<affine<con<32, 7, 7, 2, 2, input_rgb_image_sized<150>>>>>>>>>>>>>;


class QFaceRecognitionPrivate
{
public:
    QFaceRecognitionPrivate() {}

    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor       sp;
    anet_type             net;

    std::vector<matrix<rgb_pixel>> known_faces;
    std::vector<string>            known_faces_names;
    std::vector<matrix<float, 0, 1>> known_face_descriptors;

    void introduce(const QString& name, const matrix<rgb_pixel>& img);

    void extractDescriptors() {
        known_face_descriptors = net(known_faces);
    }

    void readFolder(
        const QString&                  path,
        std::vector<matrix<rgb_pixel>>& imgs,
        std::vector<string>&            names);
};

void
QFaceRecognitionPrivate::introduce(
    const QString& name, const matrix<rgb_pixel>& img) {
    for (auto face : detector(img)) {
        auto shape = sp(img, face);

        matrix<rgb_pixel> face_chip;
        extract_image_chip(
            img, get_face_chip_details(shape, 150, 0.25), face_chip);
        known_faces.push_back(move(face_chip));
        known_faces_names.push_back(name.toStdString());
    }
}

void
readImage(matrix<rgb_pixel>& img, const std::string& path) {
    QPixmap p{QString::fromStdString(path)};
}

void
QFaceRecognitionPrivate::readFolder(
    const QString&                  path,
    std::vector<matrix<rgb_pixel>>& imgs,
    std::vector<string>&            names) {
    QDir        d(path);
    QStringList images = d.entryList(
        QStringList() << "*.jpg"
                      << "*.png",
        QDir::Files);
    for (auto& filename : images) {
        matrix<rgb_pixel> img;
        QImage            image(path + "/" + filename);
        convert(image, img);
        save_jpeg(img, "ss.jpg");
        std::string imgName = filename.section(".", 0, 0).toStdString();
        for (auto face : detector(img)) {
            auto              shape = sp(img, face);
            matrix<rgb_pixel> face_chip;
            extract_image_chip(
                img, get_face_chip_details(shape, 150, 0.25), face_chip);
            imgs.push_back(move(face_chip));
            names.push_back(imgName);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////

QFaceRecognition::QFaceRecognition(
    const QString& netPath, const QString& spPath)
    : d_ptr(new QFaceRecognitionPrivate) {
    deserialize(netPath.toStdString()) >> d_ptr->net;
    deserialize(spPath.toStdString()) >> d_ptr->sp;
}

QFaceRecognition::~QFaceRecognition() {}

#ifdef QFACERECOGNITION_QML
void
QFaceRecognition::registerQmlTypes(
    const QString& netPath, const QString& spPath) {
    //    registerQmlTypes<QFaceRecognition{netPath,spPath}>(
    //        "FaceRecognition", 1, 0, "FaceRecognition");
}
#endif

void
QFaceRecognition::introduce(const QString& name, const QImage& face) {}

void
QFaceRecognition::introduce(const QString& name, const QPixmap& face) {}

void
QFaceRecognition::introduceFolder(const QString& path) {
    d_ptr->readFolder(path, d_ptr->known_faces, d_ptr->known_faces_names);
    d_ptr->extractDescriptors();
}

void
QFaceRecognition::introduceFile(const QString& path) {}

QString
QFaceRecognition::recognize(const QImage& face) {
    return "";
}

QString
QFaceRecognition::recognize(const QPixmap& face) {
    return "";
}

QList<QPair<QString, QString>>
QFaceRecognition::recognizeFolder(const QString& path, double diff) {
    std::vector<matrix<rgb_pixel>> unknown_faces;
    std::vector<string>            unknown_faces_names;
    d_ptr->readFolder(path, unknown_faces, unknown_faces_names);
    std::vector<matrix<float, 0, 1>> unknown_face_descriptors =
        d_ptr->net(unknown_faces);

    QList<QPair<QString, QString>> ans;

    for (size_t i = 0; i < unknown_face_descriptors.size(); ++i)
        for (size_t j = 0; j < d_ptr->known_face_descriptors.size(); ++j) {
            auto l = length(
                unknown_face_descriptors[i] - d_ptr->known_face_descriptors[j]);
            if (l < static_cast<float>(diff))
                ans.push_back(
                    {QString::fromStdString(unknown_faces_names[i]),
                     QString::fromStdString(d_ptr->known_faces_names[j])});
        }
    return ans;
}

QString
QFaceRecognition::recognizeFile(const QString& path) {
    return "";
}
