TARGET = BulletBox
TEMPLATE = app
DEFINES -= COUNT_FPS \
    COUNT_TIME

CONFIG -= debug
CONFIG += release

mac{
    CONFIG -= app_bundle
    #Use clang for c++11
    QMAKESPEC = macx-clang-libc++
    MY_QT5_ROOT  = /Users/sneakyc4/projects/qt5/qtbase
    MY_BULLET_ROOT = /usr/local
}else{
    MY_QT5_ROOT  = /opt/qt5/
    MY_BULLET_ROOT = /usr
}

INCLUDEPATH += $$MY_QT5_ROOT/include
INCLUDEPATH += $$MY_BULLET_ROOT/include/bullet
INCLUDEPATH += $$MY_BULLET_ROOT/include/bullet/LinearMath

message($$QMAKESPEC)

QT      += core gui quick 3d
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS    += -lBulletDynamics -lBulletCollision -lLinearMath

SOURCES += main.cpp \
    bulletboxview.cpp \
    bullet3dbox.cpp \
    motionstates.cpp \
    #bulletboxviewport.cpp

HEADERS  += \
    bulletboxview.h \
    bullet3dbox.h \
    motionstates.h \
    #bulletboxviewport.h

QMAKE_CXXFLAGS += --std=c++0x

RESOURCES += \
    mainresource.qrc

OTHER_FILES += \
    tron.fsh \
    standard.vsh \
    standard.fsh \
    fractals_julia.fsh
