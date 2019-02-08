#include "dlib/matrix.h"
#include "dlib/pixel.h"
#include "imageconvertor.hpp"
#include "qfacerecognition.hpp"


#include <QCoreApplication>
#include <QDebug>
#include <QImage>

int
main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    QImage                        src{"C:/Qt/erfan.png"};
    dlib::matrix<dlib::rgb_pixel> dst;

    if (argc < 3) {
        qDebug() << "Run this example by invoking it like this: ";
        qDebug() << "./ConsoleFaceRecognition <known image folder> <unknown "
                    "images folder>";

        return 1;
    }

    QFaceRecognition::setModel(
        "../../model/dlib_face_recognition_resnet_model_v1.dat",
        "../../model/shape_predictor_5_face_landmarks.dat");
    QFaceRecognition f;

    qDebug() << "read known";
    f.introduceFolder(argv[1]);

    qDebug() << "read unknown";
    qDebug() << f.recognizeFolder(argv[2], 0.6);


    return a.exec();
}
