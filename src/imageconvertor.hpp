#ifndef IMAGECONVERTOR_HPP
#define IMAGECONVERTOR_HPP

#include <QImage>
#include <QPixmap>
#include <dlib/matrix.h>
#include <dlib/pixel.h>


using namespace dlib;

void
convert(const QImage& src, matrix<rgb_pixel>& dst);

void
convert(const QPixmap& src, matrix<rgb_pixel>& dst);

#endif // IMAGECONVERTOR_HPP
