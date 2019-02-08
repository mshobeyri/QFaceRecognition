#ifndef QFACERECOGNITION_H
#define QFACERECOGNITION_H

#include <QObject>

class QFaceRecognitionPrivate;
class QFaceRecognition : public QObject
{
    Q_OBJECT
public:
    QFaceRecognition(QObject * parent = nullptr);
    ~QFaceRecognition();

    static void setModel(const QString& netPath, const QString& spPath);
#ifdef QFACERECOGNITION_QML
    static void registerQmlTypes();
#endif
    Q_INVOKABLE void introduce(const QString& name, const QImage& image);
    Q_INVOKABLE void introduce(const QString& name, const QPixmap& face);
    Q_INVOKABLE void introduceFolder(const QString& path);
    Q_INVOKABLE void introduceFile(const QString& path);

    Q_INVOKABLE QString recognize(const QImage& face);
    Q_INVOKABLE QString recognize(const QPixmap& face);
    Q_INVOKABLE         QList<QPair<QString, QString>>
                        recognizeFolder(const QString& path, double diff);
    Q_INVOKABLE QString recognizeFile(const QString& path);

private:
    Q_DECLARE_PRIVATE(QFaceRecognition)
    QScopedPointer<QFaceRecognitionPrivate> d_ptr;
};


#endif // QFACERECOGNITION_H
