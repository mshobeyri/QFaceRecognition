#ifndef FACERECOGNITIONTYPES_HPP
#define FACERECOGNITIONTYPES_HPP

#include "dlib/matrix.h"
#include "dlib/pixel.h"

#include <QImage>
#include <QRect>
#include <QString>

struct QFace {
    QString                       name     = "";
    QString                       source   = "";
    double                        distance = 100;
    QImage                        faceImg;
    dlib::matrix<dlib::rgb_pixel> faceChip;
    dlib::matrix<float, 0, 1> descriptor;
    QRect position;
    bool  isKnown = false;
    operator QString() const {
        QString txt;
        if (!source.isEmpty())
            txt += source + ", ";
        auto n = isKnown ? name : "unknown";
        txt += n + ", distance:" + QString::number(distance);

        return txt;
    }
};

using QFaceList = QList<QFace>;

#endif // FACERECOGNITIONTYPES_HPP
