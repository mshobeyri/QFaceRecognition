#include "qfacerecognitionfilter.h"

#include "imageconvertor.hpp"

#include <QDebug>
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QVideoFrame>

QFaceRecognitionFilter::QFaceRecognitionFilter(QObject* parent)
    : QAbstractVideoFilter(parent), recognizing(false) {
    fr.introduceFolder("../../assets/known");
}

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

QFaceRecognitionFilterRunnable::QFaceRecognitionFilterRunnable(
    QFaceRecognitionFilter* filter)
    : QObject(nullptr), filter(filter) {}

QFaceRecognitionFilterRunnable::~QFaceRecognitionFilterRunnable() {
    filter = nullptr;
}

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
    img = img.mirrored();

    auto s = recognize(img);
    if (s.length() > 0)
        emit filter->faceRecognized(s[0].name, s[0].position);
    filter->recognizing = false;
}

QFaceList
QFaceRecognitionFilterRunnable::recognize(const QImage& image) {
    return filter->fr.recognize(image);
}
