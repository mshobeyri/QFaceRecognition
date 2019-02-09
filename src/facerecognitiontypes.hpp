#ifndef FACERECOGNITIONTYPES_HPP
#define FACERECOGNITIONTYPES_HPP

#include "dlib/matrix.h"
#include "dlib/pixel.h"

#include <QImage>
#include <QRect>
#include <QString>

struct QFace {
    QString                       name = "";
    QImage                        faceImg;
    dlib::matrix<dlib::rgb_pixel> faceMatrix;
    dlib::matrix<float, 0, 1> descriptor;
    QRect position;
    bool  isKnown = false;
};

using QFaceList = std::vector<QFace>;

#endif // FACERECOGNITIONTYPES_HPP
