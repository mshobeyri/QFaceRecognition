#ifndef FACERECOGNITIONTYPES_HPP
#define FACERECOGNITIONTYPES_HPP

#include "dlib/matrix.h"
#include "dlib/pixel.h"

#include <QImage>
#include <QRect>
#include <QString>

struct QFace {
    QString                       name      = "";
    QString                       source    = "";
    double                        bestMatch = 0;
    QImage                        faceImg;
    dlib::matrix<dlib::rgb_pixel> faceMatrix;
    dlib::matrix<float, 0, 1> descriptor;
    QRect position;
    bool  isKnown = false;
    operator QString() const {
        auto n = isKnown ? name : "unknown";
        if (source.isEmpty()) {
            return QString{n + ", bm:" + QString::number(bestMatch)};
        } else {
            return QString{source + " is " + n + ", bm:" +
                           QString::number(bestMatch)};
        }
    }
};

using QFaceList = QList<QFace>;

#endif // FACERECOGNITIONTYPES_HPP
