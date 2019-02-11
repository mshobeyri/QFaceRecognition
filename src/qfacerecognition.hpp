#ifndef QFACERECOGNITION_H
#define QFACERECOGNITION_H

#include "facerecognitiontypes.hpp"
#include <QObject>

class QFaceRecognitionPrivate;
class QFaceRecognition : public QObject
{
    Q_OBJECT
    Q_ENUMS(QFaceRecognitionMode)
    Q_PROPERTY(uint mode READ mode WRITE setMode NOTIFY modeChanged)
public:
    QFaceRecognition(QObject* parent = nullptr);
    ~QFaceRecognition();

    static void setModel(const QString& netPath, const QString& spPath);
#ifdef QFACERECOGNITION_QML
    static void registerQmlTypes();
#endif

    Q_INVOKABLE void introduce(const QString& name, const QImage& image);
    Q_INVOKABLE void introduce(const QString& name, const QPixmap& pixmap);
    Q_INVOKABLE void introduceFolder(const QString& path);
    Q_INVOKABLE void introduceFile(const QString& path);

    Q_INVOKABLE QFaceList recognize(const QImage& image);
    Q_INVOKABLE QFaceList recognize(const QPixmap& pixmap);
    Q_INVOKABLE QFaceList recognizeFolder(const QString& path);
    Q_INVOKABLE QFaceList recognizeFile(const QString& path);

    Q_INVOKABLE uint mode() const;
    Q_INVOKABLE  void setMode(const uint& mode);

    double           distanceThreshold() const;
    void setDistanceThreshold(double distanceThreshold);
signals:
    void modeChanged();
private:
    Q_DECLARE_PRIVATE(QFaceRecognition)
    QScopedPointer<QFaceRecognitionPrivate> d_ptr;
};


#endif // QFACERECOGNITION_H
