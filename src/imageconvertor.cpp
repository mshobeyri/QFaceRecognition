#include "imageconvertor.hpp"

#include <QDebug>
#include <QRgb>

using namespace dlib;

void
convert(const QImage& src, matrix<rgb_pixel>& dst) {
    dst.set_size(src.height(), src.width());
    auto it = dst.begin();
    int col = 0;
    for (int i = 0; i < src.height(); ++i)
        for (int j = 0; it != dst.end() && j < src.width(); ++j, it++) {
            auto c = src.pixel(j, i);

            it->blue  = qBlue(c);
            it->green = qGreen(c);
            it->red   = qRed(c);
        }
}
