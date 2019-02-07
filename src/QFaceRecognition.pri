INCLUDEPATH += $$PWD/../3rdparty/dlib
DEPENDPATH += $$PWD/../3rdparty/dlib

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/


HEADERS += $$PWD/qfacerecognition.hpp \
    $$PWD/qfacerecognitionfilter.h \
    $$PWD/imageconvertor.hpp
SOURCES += $$PWD/qfacerecognition.cpp \
    $$PWD/qfacerecognitionfilter.cpp \
    $$PWD/imageconvertor.cpp
