#include "qfacerecognition.hpp"
#include "QtQml/qqml.h"
#include "imageconvertor.hpp"

#ifdef QFACERECOGNITION_MEDIA
#include "qfacerecognitionfilter.h"
#endif

#include <dlib/clustering.h>
#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/string.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
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


///////////////////////////////////////////////////////////////////////////////

class QFaceRecognitionPrivate
{
public:
    QFaceRecognitionPrivate() {}

    frontal_face_detector  detector = get_frontal_face_detector();
    static shape_predictor sp;
    static anet_type       net;


    double m_distanceThreshold = 0.6;

    QFaceList knownFaces;

    void extractFeatures(QFace& face);
    bool detectFace(const matrix<rgb_pixel>& img, QFace& face);
    QFaceList detectFaces(const matrix<rgb_pixel>& img);
    void recognizeFace(QFace& face);
    QFaceList recognizeFaces(const matrix<rgb_pixel>& img);
    bool introduce(const QString& name, const matrix<rgb_pixel>& img);

    QStringList imagesInPath(const QString& path);
};

shape_predictor QFaceRecognitionPrivate::sp;
anet_type       QFaceRecognitionPrivate::net;


void
QFaceRecognitionPrivate::extractFeatures(QFace& face) {

    static int s = 0;
    s++;
    dlib::save_jpeg(
        face.faceChip,
        QString("c:/Qt/" + QString::number(s) + ".jpg").toStdString());
    face.descriptor = net(face.faceChip);
}

bool
QFaceRecognitionPrivate::detectFace(const matrix<rgb_pixel>& img, QFace& face) {
    auto detected = detector(img);
    if (detected.size() > 1) {
        qDebug() << "more than one face detected";
        return false;
    }
    if (detected.size() < 1) {
        qDebug() << "no face detected";
        return false;
    }
    auto faceImg = detector(img)[0];
    auto shape   = sp(img, faceImg);

    extract_image_chip(
        img, get_face_chip_details(shape, 150, 0.25), face.faceChip);

    return true;
}

QFaceList
QFaceRecognitionPrivate::detectFaces(const matrix<rgb_pixel>& img) {
    QFaceList detectedFaces;
    for (auto& faceImg : detector(img)) {
        QFace face;
        auto  shape = sp(img, faceImg);
        extract_image_chip(
            img, get_face_chip_details(shape, 150, 0.25), face.faceChip);
        const auto& r = shape.get_rect();
        face.position = QRect{r.left(),
                              r.top(),
                              static_cast<int>(r.width()),
                              static_cast<int>(r.height())};
        face.descriptor = net(face.faceChip);
        detectedFaces.push_back(std::move(face));
    }
    return detectedFaces;
}

void
QFaceRecognitionPrivate::recognizeFace(QFace& face) {
    extractFeatures(face);
    for (int j = 0; j < knownFaces.size(); ++j) {
        auto l = length(face.descriptor - knownFaces[j].descriptor);
        if (l < face.distance)
            face.distance = l;
        if (length(face.descriptor - knownFaces[j].descriptor) <
            m_distanceThreshold) {
            face.name    = knownFaces[j].name;
            face.isKnown = true;
            return;
        }
    }
}

QFaceList
QFaceRecognitionPrivate::recognizeFaces(const matrix<rgb_pixel>& img) {
    QFaceList detectedFaces = detectFaces(img);
    for (auto& face : detectedFaces) {
        recognizeFace(face);
    }
    return detectedFaces;
}

bool
QFaceRecognitionPrivate::introduce(
    const QString& name, const matrix<rgb_pixel>& img) {
    QFace face;
    if (detectFace(img, face)) {
        extractFeatures(face);
        face.name    = name;
        face.isKnown = true;
        knownFaces.push_back(std::move(face));
        return true;
    }
    return false;
}


QStringList
QFaceRecognitionPrivate::imagesInPath(const QString& path) {
    QDir d(path);
    return d.entryList(
        QStringList() << "*.jpg"
                      << "*.png"
                      << "*.jpeg",
        QDir::Files);
}


///////////////////////////////////////////////////////////////////////////////

QFaceRecognition::QFaceRecognition(QObject* parent)
    : QObject(parent), d_ptr(new QFaceRecognitionPrivate) {}

QFaceRecognition::~QFaceRecognition() {}

void
QFaceRecognition::setModel(const QString& netPath, const QString& spPath) {
    deserialize(netPath.toStdString()) >> QFaceRecognitionPrivate::net;
    deserialize(spPath.toStdString()) >> QFaceRecognitionPrivate::sp;
}

#ifdef QFACERECOGNITION_QML
void
QFaceRecognition::registerQmlTypes() {
    qmlRegisterType<QFaceRecognition>(
        "FaceRecognition", 1, 0, "FaceRecognition");
#ifdef QFACERECOGNITION_MEDIA
    qmlRegisterType<QFaceRecognitionFilter>(
        "FaceRecognition", 1, 0, "FaceRecognitionFilter");
#endif
}
#endif

void
QFaceRecognition::introduce(const QString& name, const QImage& image) {
    matrix<rgb_pixel> img;
    convert(image, img);
    d_ptr->introduce(name, img);
}

void
QFaceRecognition::introduce(const QString& name, const QPixmap& pixmap) {
    introduce(name, pixmap.toImage());
}

void
QFaceRecognition::introduceFolder(const QString& path) {
    for (auto& filename : d_ptr->imagesInPath(path)) {
        introduceFile(path + "/" + filename);
    }
}

void
QFaceRecognition::introduceFile(const QString& path) {
    introduce(QFileInfo{path}.baseName(), QImage{path});
}

QFaceList
QFaceRecognition::recognize(const QImage& image) {
    matrix<rgb_pixel> img;
    convert(image, img);
    return d_ptr->recognizeFaces(img);
}

QFaceList
QFaceRecognition::recognize(const QPixmap& pixmap) {
    return recognize(pixmap.toImage());
}

QFaceList
QFaceRecognition::recognizeFolder(const QString& path) {
    QFaceList faces;
    for (auto& filename : d_ptr->imagesInPath(path)) {
        faces.append(recognizeFile(path + "/" + filename));
    }
    return faces;
}

QFaceList
QFaceRecognition::recognizeFile(const QString& path) {
    auto faceList = recognize(QImage{path});
    for (auto& face : faceList) {
        face.source = path;
    }
    return faceList;
}

double
QFaceRecognition::distanceThreshold() const {
    return d_ptr->m_distanceThreshold;
}

void
QFaceRecognition::setDistanceThreshold(double distanceThreshold) {
    d_ptr->m_distanceThreshold = distanceThreshold;
}
