#include "imageconvertor.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QRgb>

using namespace dlib;

void
convert(const QImage& src, matrix<rgb_pixel>& dst) {
    dst.set_size(src.height(), src.width());
    QImage src888 = src;//;.convertToFormat(QImage::Format_RGB888);
    auto   dstIt  = dst.begin();
    int    i      = 0;

    QRgb* st = (QRgb*)src888.bits();
    while (dstIt != dst.end() && i < src.width() * src.height()) {
        const auto& str = st[i];
        dstIt->blue     = qBlue(str);
        dstIt->green    = qGreen(str);
        dstIt->red      = qRed(str);
        ++dstIt;
        ++i;
    }
}

void
convert(const QPixmap& src, matrix<rgb_pixel>& dst) {
    convert(src.toImage(), dst);
}
