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
    SimpleVideoFrame& videoFrame, const QRect& _captureRect) {
    static unsigned int i = 0;
    i++;
    //    qDebug() << "Future: Going to process frame: " << i;

    const int         width  = videoFrame.size.width();
    const int         height = videoFrame.size.height();
    const CaptureRect captureRect(_captureRect, width, height);
    const uchar*      data =
        reinterpret_cast<const uchar*>(videoFrame.data.constData());

    uchar* pixel;
    int    wh;
    int    w_2;
    int    wh_54;

    const uint32_t* yuvPtr = reinterpret_cast<const uint32_t*>(data);

    /// Create QImage from QVideoFrame.
    QImage* image_ptr = nullptr;

    switch (videoFrame.pixelFormat) {
    case QVideoFrame::Format_BGR555:
        /// This is a forced "conversion", colors end up swapped.
        image_ptr = new QImage(data, width, height, QImage::Format_RGB555);
        break;
    case QVideoFrame::Format_BGR565:
        /// This is a forced "conversion", colors end up swapped.
        image_ptr = new QImage(data, width, height, QImage::Format_RGB16);
        break;
    case QVideoFrame::Format_YUV420P:
        // fix for issues #4 and #9
        image_ptr = new QImage(
            captureRect.targetWidth,
            captureRect.targetHeight,
            QImage::Format_Grayscale8);
        pixel = image_ptr->bits();
        wh    = width * height;
        w_2   = width / 2;
        wh_54 = wh * 5 / 4;

        for (int y = captureRect.startY; y < captureRect.endY; y++) {
            const int Y_offset = y * width;
            const int y_2      = y / 2;
            const int U_offset = y_2 * w_2 + wh;
            const int V_offset = y_2 * w_2 + wh_54;
            for (int x = captureRect.startX; x < captureRect.endX; x++) {
                const int   x_2 = x / 2;
                const uchar Y   = data[Y_offset + x];
                const uchar U   = data[U_offset + x_2];
                const uchar V   = data[V_offset + x_2];
                *pixel          = yuvToGray(Y, U, V);
                ++pixel;
            }
        }
        break;
    case QVideoFrame::Format_NV12:
        /// nv12 format, encountered on macOS
        image_ptr = new QImage(
            captureRect.targetWidth,
            captureRect.targetHeight,
            QImage::Format_Grayscale8);
        pixel = image_ptr->bits();
        wh    = width * height;
        w_2   = width / 2;
        wh_54 = wh * 5 / 4;

        for (int y = captureRect.startY; y < captureRect.endY; y++) {
            const int Y_offset = y * width;
            const int y_2      = y / 2;
            const int U_offset = y_2 * w_2 + wh;
            const int V_offset = y_2 * w_2 + wh_54;
            for (int x = captureRect.startX; x < captureRect.endX; x++) {
                const int   x_2 = x / 2;
                const uchar Y   = data[Y_offset + x];
                const uchar U   = data[U_offset + x_2];
                const uchar V   = data[V_offset + x_2];
                *pixel          = yuvToGray(Y, U, V);
                ++pixel;
            }
        }
        break;
    case QVideoFrame::Format_YUYV:
        image_ptr = new QImage(
            captureRect.targetWidth,
            captureRect.targetHeight,
            QImage::Format_Grayscale8);
        pixel = image_ptr->bits();

        for (int y = captureRect.startY; y < captureRect.endY; y++) {
            const uint32_t* row = &yuvPtr[y * (width / 2) - (width / 4)];
            for (int x = captureRect.startX; x < captureRect.endX; x++) {
                const uint8_t* pxl = reinterpret_cast<const uint8_t*>(&row[x]);
                const uint8_t  y0  = pxl[0];
                const uint8_t  u   = pxl[1];
                const uint8_t  v   = pxl[3];
                *pixel             = yuvToGray(y0, u, v);
                ++pixel;
            }
        }
        break;
    /// TODO: Handle (create QImages from) YUV formats.
    default:
        QImage::Format imageFormat =
            QVideoFrame::imageFormatFromPixelFormat(videoFrame.pixelFormat);
        image_ptr = new QImage(data, width, height, imageFormat);
        break;
    }

    if (!image_ptr || image_ptr->isNull()) {
        qDebug() << "QFaceRecognitionFilterRunnable error: Cant create image "
                    "file to process.";
        qDebug() << "Maybe it was a format conversion problem? ";
        qDebug() << "VideoFrame format: " << videoFrame.pixelFormat;
        qDebug()
            << "Image corresponding format: "
            << QVideoFrame::imageFormatFromPixelFormat(videoFrame.pixelFormat);
        filter->recognizing = false;
        return;
    }

    if (captureRect.isValid && image_ptr->size() != _captureRect.size())
        image_ptr = new QImage(image_ptr->copy(_captureRect));

    decode(*image_ptr);

    delete image_ptr;
}

QString
QFaceRecognitionFilterRunnable::decode(const QImage& image) {
    return "";
}
