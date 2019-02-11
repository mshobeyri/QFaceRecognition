#ifndef QFACERECOGNITION_H
#define QFACERECOGNITION_H

#include "facerecognitiontypes.hpp"
#include <QObject>

class QFaceRecognitionPrivate;
class QFaceRecognition : public QObject
{
    Q_OBJECT

public:
    enum Mode { Detection, Recognize };

    Q_ENUM(Mode)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(double distanceThreshold READ distanceThreshold WRITE
                                                               setDistanceThreshold)
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

    Q_INVOKABLE Mode mode() const;
    Q_INVOKABLE void setMode(const Mode& mode);

    Q_INVOKABLE double distanceThreshold() const;
    Q_INVOKABLE void setDistanceThreshold(double distanceThreshold);

signals:
    void modeChanged();
    void faceDetected(QRect position);
    void faceRecognized(QString name, QRect position);
    void detectionProcessEnded(QList<QRect> positions);
    void recognizeProcessEnded(QStringList names, QStringList positions);

private:
    Q_DECLARE_PRIVATE(QFaceRecognition)
    QScopedPointer<QFaceRecognitionPrivate> d_ptr;
};


#endif // QFACERECOGNITION_H
