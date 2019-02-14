


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
    $$PWD/imageconvertor.hpp \
    $$PWD/facerecognitiontypes.hpp
SOURCES += $$PWD/qfacerecognition.cpp \
    $$PWD/imageconvertor.cpp


INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

INCLUDEPATH += $$PWD/../3rdparty/dlib
DEPENDPATH += $$PWD/../3rdparty/dlib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -ldlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -ldlibd
else:unix: LIBS += -L$$PWD/../lib/ -ldlib
