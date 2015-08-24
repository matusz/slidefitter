#-------------------------------------------------
#
# Project created by QtCreator 2015-02-11T17:05:02
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = slidefitter
TEMPLATE = app

LIBS += -lexif

SOURCES += main.cpp\
        mainwindow.cpp \
    slideinfo.cpp \
    slidedisplaywidget.cpp \
    settingsdialog.cpp \
    slideshowsequencer.cpp \
    firstcolumnstretchtableview.cpp \
    firstcolumnstretchtreeview.cpp \
    imageslideinfo.cpp \
    slideinfolist.cpp \
    slideinfomodel.cpp \
    slideinfocomparator.cpp \
    slideloader.cpp \
    slide.cpp \
    slidedisplaywidgetlist.cpp

HEADERS  += mainwindow.h \
    slideinfo.h \
    slidedisplaywidget.h \
    settingsdialog.h \
    slideshowsequencer.h \
    constants.h \
    firstcolumnstretchtableview.h \
    firstcolumnstretchtreeview.h \
    imageslideinfo.h \
    slideinfolist.h \
    slideinfomodel.h \
    slideinfocomparator.h \
    slideloader.h \
    slide.h \
    slidedisplaywidgetlist.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += slideshow.qrc
