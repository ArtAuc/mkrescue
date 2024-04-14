QT       += core gui
QT += sql
QT += printsupport
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    data/database.cpp \
    data/database_getregistry.cpp \
    widgets/dogcard.cpp \
    pages/dogregistry.cpp \
    widgets/editdogwidget.cpp \
    pages/editpage/editpage.cpp \
    pages/editpage/editpage_es.cpp \
    pages/editpage/editpage_saveedit.cpp \
    pages/editpage/editpage_utils.cpp \
    widgets/editpeoplewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_exportpdf.cpp \
    mainwindow_loadpages.cpp \
    pages/registry.cpp \
    data/saveddata.cpp \
    simplecrypt.cpp

HEADERS += \
    data/database.h \
    widgets/customdatetimeedit.h \
    widgets/destinationpage.h \
    widgets/dogcard.h \
    pages/dogregistry.h \
    widgets/editdogwidget.h \
    pages/editpage/editpage.h \
    widgets/editpeoplewidget.h \
    pages/homepage.h \
    widgets/hovertoolbutton.h \
    pages/loginpage.h \
    mainwindow.h \
    widgets/menutree.h \
    pages/registry.h \
    data/saveddata.h \
    pages/settingspage.h \
    simplecrypt.h \
    widgets/statwidget.h \
    widgets/tristatecheckbox.h \
    utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    restore.py \
