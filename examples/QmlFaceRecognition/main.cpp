#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <qfacerecognition.hpp>

int
main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QFaceRecognition::registerQmlTypes(
        "model/dlib_face_recognition_resnet_model_v1.dat",
        "shape_predictor_5_face_landmarks.dat");
    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
