#-------------------------------------------------
#
# Project created by QtCreator 2014-08-01T16:22:41
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -D _CRT_SECURE_NO_WARNINGS

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = blz_toolkit
TEMPLATE = app

DEFINES += 'APP_NAME=\"blz_toolkit\"'
VERSION = 0.0.0
DEFINES += APP_VERSION=$$VERSION

RC_FILE = tkt.rc

SOURCES += main.cpp\
    blz_toolkit_mainwindow.cpp \
    blz_toolkit_peer.cpp \
    blz_toolkit_core_util.cpp \
    blz_toolkit_connection.cpp \
    blz_toolkit_newconndlg.cpp \
    blz_toolkit_blz_model.cpp \
    blz_toolkit_model_tab.cpp \
    blz_toolkit_sbs_window.cpp \
    blz_toolkit_sbs_blz_class_model.cpp \
    blz_toolkit_tx_window.cpp \
    blz_toolkit_tx_blz_class_model.cpp

HEADERS  += blz_toolkit_mainwindow.h \
    blz_toolkit_peer.h \
    blz_toolkit_core_util.h \
    blz_toolkit_glob.h \
    blz_toolkit_connection.h \
    blz_toolkit_newconndlg.h \
    blz_toolkit_blz_model.h \
    blz_toolkit_model_tab.h \
    blz_toolkit_sbs_window.h \
    blz_toolkit_sbs_blz_class_model.h \
    blz_toolkit_tx_window.h \
    blz_toolkit_tx_blz_class_model.h

FORMS    += blz_toolkit_mainwindow.ui \
    blz_toolkit_connection.ui \
    blz_toolkit_newconndlg.ui \
    blz_toolkit_model_tab.ui \
    blz_toolkit_sbs_window.ui \
    blz_toolkit_tx_window.ui

RESOURCES += \
    blz_toolkit_prj_res.qrc

win32 {
    INCLUDEPATH += $$PWD/../../contrib/pthread
}

INCLUDEPATH += $$PWD/../../src \
               $$PWD/../../public/cpp \
               $$PWD/../../src/crlib

win32 {
    LIBS += -L$$PWD/../../contrib_lib/nt10_x64 \
            -lwsock32 \
            -lshell32
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../bin/MSVC/x64/Debug/StaticLibrary/v120
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../bin/MSVC/x64/Release/StaticLibrary/v120
    }
    LIBS += -lpthreadVC2

    LIBS += -lcrlib \
        -lblz_model \
        -lblz_peer_lib \
        -lblz_persistence
}

macx {
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../bin/OSX_64/DBG/lib
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../bin/OSX_64/REL/lib
    }

    LIBS += -lcr \
        -lblzmodel \
        -lblzpeer \
        -lblzpers
}




