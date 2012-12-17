INCLUDEPATH += /opt/qt5/include
INCLUDEPATH += /usr/include/bullet
QT      += core gui quick 3d

LIBS    += -lBulletDynamics -lBulletCollision -lLinearMath

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BulletBox
TEMPLATE = app

SOURCES += main.cpp \
    bulletboxview.cpp \
    bullet3dbox.cpp \
    motionstates.cpp

HEADERS  += \
    bulletboxview.h \
    bullet3dbox.h \
    motionstates.h

QMAKE_CXXFLAGS += -std=c++0x

RESOURCES += \
    mainresource.qrc

DEFINES -= COUNT_FPS \
    COUNT_TIME

OTHER_FILES += \
    tron.fsh \
    standard.vsh \
    standard.fsh
