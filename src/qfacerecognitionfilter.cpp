#include "qfacerecognitionfilter.h"

#include "imageconvertor.hpp"

#include <QDebug>
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QVideoFrame>

namespace {
uchar
gray(uchar r, uchar g, uchar b) {
    return (306 * (r & 0xFF) + 601 * (g & 0xFF) + 117 * (b & 0xFF) + 0x200) >>
           10;
}
uchar
yuvToGray(uchar Y, uchar U, uchar V) {
    const int C = int(Y) - 16;
    const int D = int(U) - 128;
    const int E = int(V) - 128;
    return gray(
        qBound<uchar>(0, uchar((298 * C + 409 * E + 128) >> 8), 255),
        qBound<uchar>(0, uchar((298 * C - 100 * D - 208 * E + 128) >> 8), 255),
        qBound<uchar>(0, uchar((298 * C + 516 * D + 128) >> 8), 255));
}
}

QFaceRecognitionFilter::QFaceRecognitionFilter(QObject* parent)
    : QAbstractVideoFilter(parent), recognizing(false) {}

QFaceRecognitionFilter::~QFaceRecognitionFilter() {
    if (!processThread.isFinished()) {
        processThread.cancel();
        processThread.waitForFinished();
    }
}

QVideoFilterRunnable*
QFaceRecognitionFilter::createFilterRunnable() {
    return new QFaceRecognitionFilterRunnable(this);
}

static bool
isRectValid(const QRect& rect) {
    return rect.x() >= 0 && rect.y() >= 0 && rect.isValid();
}

struct CaptureRect {
    CaptureRect(const QRect& captureRect, int sourceWidth, int sourceHeight)
        : isValid(isRectValid(captureRect)),
          sourceWidth(sourceWidth),
          sourceHeight(sourceHeight),
          startX(isValid ? captureRect.x() : 0),
          targetWidth(isValid ? captureRect.width() : sourceWidth),
          endX(startX + targetWidth),
          startY(isValid ? captureRect.y() : 0),
          targetHeight(isValid ? captureRect.height() : sourceHeight),
          endY(startY + targetHeight) {}

    bool isValid;
    char pad[3]; // avoid warning about padding

    int sourceWidth;
    int sourceHeight;

    int startX;
    int targetWidth;
    int endX;

    int startY;
    int targetHeight;
    int endY;
};

QFaceRecognitionFilterRunnable::QFaceRecognitionFilterRunnable(
    QFaceRecognitionFilter* filter) {}

QFaceRecognitionFilterRunnable::~QFaceRecognitionFilterRunnable() {}

QVideoFrame
QFaceRecognitionFilterRunnable::run(
    QVideoFrame*                   input,
    const QVideoSurfaceFormat&     surfaceFormat,
    QVideoFilterRunnable::RunFlags flags) {
    Q_UNUSED(surfaceFormat);
    Q_UNUSED(flags);

    if (!input || !input->isValid()) {
        // qDebug() << "[QFaceRecognitionRunnable] Invalid Input ";
        return *input;
    }
    if (filter->isRecognizing()) {
        // qDebug() << "------ decoder busy.";
        return *input;
    }
    if (!filter->processThread.isFinished()) {
        // qDebug() << "--[]-- decoder busy.";
        return *input;
    }

    filter->recognizing = true;

    filter->frame.copyData(*input);
    filter->processThread = QtConcurrent::run(
        this,
        &QFaceRecognitionFilterRunnable::processVideoFrameProbed,
        filter->frame,
        filter->captureRect.toRect());

    return *input;
}

void
QFaceRecognitionFilterRunnable::processVideoFrameProbed(
    SimpleVideoFrame& videoFrame, const QRect& captureRect) {

    const int    width  = videoFrame.size.width();
    const int    height = videoFrame.size.height();
    const uchar* data =
        reinterpret_cast<const uchar*>(videoFrame.data.constData());

    QImage img{data,
               width,
               height,
               QVideoFrame::imageFormatFromPixelFormat(videoFrame.pixelFormat)};
    img = img.copy(captureRect);

    decode(img);
}

QFaceList
QFaceRecognitionFilterRunnable::decode(const QImage& image) {
    return filter->fr.recognize(image);
}
