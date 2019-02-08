INCLUDEPATH += $$PWD/../3rdparty/dlib
DEPENDPATH += $$PWD/../3rdparty/dlib

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/


contains( QT, multimedia){
DEFINES += QFACERECOGNITION_MEDIA

HEADERS +=  \
$$PWD/qfacerecognitionfilter.h

SOURCES +=  \
$$PWD/qfacerecognitionfilter.cpp
}

contains( QT, quick ){
DEFINES += QFACERECOGNITION_QML
}

HEADERS += $$PWD/qfacerecognition.hpp \
    $$PWD/imageconvertor.hpp
SOURCES += $$PWD/qfacerecognition.cpp \
    $$PWD/imageconvertor.cpp

