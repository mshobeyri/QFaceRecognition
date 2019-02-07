#include "imageconvertor.hpp"

#include <QRgb>
#include <QDebug>

using namespace dlib;
void
convert(const QImage& src, matrix<rgb_pixel>& dst) {
    dst.set_size(src.height(), src.width());

    for (int i=0; i < src.width(); ++i)
        for (int j = 0; j < src.height(); ++j) {
            auto c = src.pixel(i, j);
            auto& sc = dst(j,i);
            sc.blue = qBlue(c);
            sc.green = qGreen(c);
            sc.red = qRed(c);
        }
}
