#-------------------------------------------------
#
# Project created by QtCreator 2014-08-01T16:22:41
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -D _CRT_SECURE_NO_WARNINGS

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vlg_toolkit
TEMPLATE = app

DEFINES += 'APP_NAME=\"VlgToolKit\"'
VERSION = 0.0.0
DEFINES += APP_VERSION=$$VERSION

RC_FILE = tkt.rc

SOURCES += main.cpp\
    tkt_mainwindow.cpp\
    tkt_core_util.cpp\
    tkt_connection.cpp\
    tkt_newconndlg.cpp\
    tkt_vlg_model.cpp\
    tkt_model_tab.cpp\
    tkt_sbs_window.cpp\
    tkt_sbs_vlg_nclass_model.cpp\
    tkt_tx_window.cpp\
    tkt_tx_vlg_nclass_model.cpp

HEADERS += tkt_mainwindow.h\
    tkt_core_util.h\
    tkt_glob.h\
    tkt_connection.h\
    tkt_newconndlg.h\
    tkt_vlg_model.h\
    tkt_model_tab.h\
    tkt_sbs_window.h\
    tkt_sbs_vlg_nclass_model.h\
    tkt_tx_window.h\
    tkt_tx_vlg_nclass_model.h

FORMS += tkt_mainwindow.ui\
    tkt_connection.ui\
    tkt_newconndlg.ui\
    tkt_model_tab.ui\
    tkt_sbs_window.ui\
    tkt_tx_window.ui

RESOURCES += \
    tkt_prj_res.qrc

INCLUDEPATH += $$PWD/../src\
               $$PWD/../public\
               $$PWD/../public/cpp\
               $$PWD/../src/vlg\
               $$PWD/../contrib

win32 {
    LIBS += -L$$PWD\
            -lwsock32\
            -lshell32

    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../bin/MSVC/x64/Debug/StaticLibrary
        LIBS += -L$$PWD/../bin/MSVC/x64/Debug/DynamicLibrary
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../bin/MSVC/x64/Release/StaticLibrary
        LIBS += -L$$PWD/../bin/MSVC/x64/Release/DynamicLibrary
    }

    LIBS += -lvlg\
            -lapicpp
}

macx {
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../bin/OSX_64/DBG/lib
        LIBS += -L$$PWD/../bin/OSX_64/DBG/libso
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../bin/OSX_64/REL/lib
        LIBS += -L$$PWD/../bin/OSX_64/REL/libso
    }

    LIBS += -lvlg\
            -lapicpp
}




