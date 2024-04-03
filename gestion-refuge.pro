QT       += core gui
QT += sql
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    database.cpp \
    database_getregistry.cpp \
    destinationpage.cpp \
    dogcard.cpp \
    dogregistry.cpp \
    editdogwidget.cpp \
    editpage.cpp \
    editpage_es.cpp \
    editpage_saveedit.cpp \
    editpeoplewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_exportpdf.cpp \
    mainwindow_loadpages.cpp \
    registry.cpp

HEADERS += \
    database.h \
    destinationpage.h \
    dogcard.h \
    dogregistry.h \
    editdogwidget.h \
    editpage.h \
    editpeoplewidget.h \
    mainwindow.h \
    menutree.h \
    registry.h \
    tristatecheckbox.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
