#include "qfacerecognition.hpp"
#include <QCoreApplication>
#include <QDebug>

int
main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    if (argc < 3) {
        qDebug() << "Run this example by invoking it like this: ";
        qDebug() << "   ./ConsoleFaceRecognition <known image folder> <unknown "
                    "images folder>";

        return 1;
    }
    QFaceRecognition f{"../../model/dlib_face_recognition_resnet_model_v1.dat",
                       "../../model/shape_predictor_5_face_landmarks.dat"};

    f.introduceFolder(argv[1]);


    qDebug() << f.recognizeFolder(argv[2], 0.6);


    return a.exec();
}
