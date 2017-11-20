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
    vlg_toolkit_mainwindow.cpp\
    vlg_toolkit_peer.cpp\
    vlg_toolkit_core_util.cpp\
    vlg_toolkit_connection.cpp\
    vlg_toolkit_newconndlg.cpp\
    vlg_toolkit_vlg_model.cpp\
    vlg_toolkit_model_tab.cpp\
    vlg_toolkit_sbs_window.cpp\
    vlg_toolkit_sbs_vlg_class_model.cpp\
    vlg_toolkit_tx_window.cpp\
    vlg_toolkit_tx_vlg_class_model.cpp

HEADERS  += vlg_toolkit_mainwindow.h\
    vlg_toolkit_peer.h\
    vlg_toolkit_core_util.h\
    vlg_toolkit_glob.h\
    vlg_toolkit_connection.h\
    vlg_toolkit_newconndlg.h\
    vlg_toolkit_vlg_model.h\
    vlg_toolkit_model_tab.h\
    vlg_toolkit_sbs_window.h\
    vlg_toolkit_sbs_vlg_class_model.h\
    vlg_toolkit_tx_window.h\
    vlg_toolkit_tx_vlg_class_model.h

FORMS    += vlg_toolkit_mainwindow.ui\
    vlg_toolkit_connection.ui\
    vlg_toolkit_newconndlg.ui\
    vlg_toolkit_model_tab.ui\
    vlg_toolkit_sbs_window.ui\
    vlg_toolkit_tx_window.ui

RESOURCES += \
    vlg_toolkit_prj_res.qrc

win32 {
    INCLUDEPATH += $$PWD/../contrib/pthread
}

INCLUDEPATH += $$PWD/../src\
               $$PWD/../public\
               $$PWD/../public/cpp\
               $$PWD/../src/cr\
               $$PWD/../src/vlg

win32 {
    LIBS += -L$$PWD/../contrib_lib/nt10_x64\
            -lwsock32\
            -lshell32
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../bin/MSVC/x64/Debug/StaticLibrary/v120
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../bin/MSVC/x64/Release/StaticLibrary/v120
    }
    LIBS += -lpthreadVC2

    LIBS += -lcr\
        -lvlg
}

macx {
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../bin/OSX_64/DBG/lib
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../bin/OSX_64/REL/lib
    }

    LIBS += -lcr\
        -lvlg
}




