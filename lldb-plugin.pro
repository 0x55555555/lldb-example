#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T21:47:00
#
#-------------------------------------------------

QT += widgets

TARGET = lldb-plugin
TEMPLATE = app

DEFINES += LLDBPLUGIN_LIBRARY

QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
QMAKE_MAC_SDK = macosx10.9

SOURCES += \
    mainwindow.cpp

HEADERS += LldbPlugin.h \
    mainwindow.h

LLVM_BUILD_PATH = ../../llvm-build/Debug+Asserts/
LLVM_PATH = ../../llvm/
LLDB_PATH = $${LLVM_PATH}/tools/lldb/

LIBS += -L$${LLVM_BUILD_PATH}/lib/ -llldb

INCLUDEPATH += $${LLDB_PATH}/include

FORMS += \
    mainwindow.ui
