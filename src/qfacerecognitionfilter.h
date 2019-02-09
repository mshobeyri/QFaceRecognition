#ifndef QFACERECOGNITIONFILTER_H
#define QFACERECOGNITIONFILTER_H

#include "facerecognitiontypes.hpp"
#include "qfacerecognition.hpp"

#include <QDebug>
#include <QFuture>
#include <QObject>
#include <QtMultimedia/QAbstractVideoFilter>


struct SimpleVideoFrame {
    QByteArray               data;
    QSize                    size;
    QVideoFrame::PixelFormat pixelFormat;

    SimpleVideoFrame() : size{0, 0}, pixelFormat{QVideoFrame::Format_Invalid} {}

    void copyData(QVideoFrame& frame) {
        frame.map(QAbstractVideoBuffer::ReadOnly);

        if (data.size() != frame.mappedBytes()) {
            qDebug() << "needed to resize";
            qDebug() << "size: " << data.size()
                     << ", new size: " << frame.mappedBytes();
            data.resize(frame.mappedBytes());
        }
        memcpy(data.data(), frame.bits(), frame.mappedBytes());
        size        = frame.size();
        pixelFormat = frame.pixelFormat();

        frame.unmap();
    }
};

class QFaceRecognitionFilter : public QAbstractVideoFilter
{

    friend class QFaceRecognitionFilterRunnable;

    Q_OBJECT

public:
    explicit QFaceRecognitionFilter(QObject* parent = 0);
    virtual ~QFaceRecognitionFilter();

    bool isRecognizing() {
        return recognizing;
    }
    QFaceRecognition* getFaceRecognizer() {
        return &fr;
    }

    QVideoFilterRunnable* createFilterRunnable();

private:
    QFaceRecognition fr{this};
    bool             recognizing;
    QRectF           captureRect;

    SimpleVideoFrame frame;
    QFuture<void>    processThread;
};

class QFaceRecognitionFilterRunnable : public QObject,
                                       public QVideoFilterRunnable
{
    Q_OBJECT

public:
    explicit QFaceRecognitionFilterRunnable(QFaceRecognitionFilter* filter);
    virtual ~QFaceRecognitionFilterRunnable();

    QVideoFrame
    run(QVideoFrame*               input,
        const QVideoSurfaceFormat& surfaceFormat,
        RunFlags                   flags);
    void processVideoFrameProbed(
        SimpleVideoFrame& videoFrame, const QRect& captureRect);

private:
    QFaceList decode(const QImage& image);

private:
    QFaceRecognitionFilter* filter;
};

#endif // QFACERECOGNITIONFILTER_H
