QT += core testlib
QT -= gui

CONFIG += c++11

TARGET = UnitTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD\..\

SOURCES += main.cpp \
    ../CommandBuffer.cpp \
    ../QAnswerBuffer.cpp \
    TestCommandBuffer.cpp \
    TestAnswerBuffer.cpp \
    ../CommandProcessor.cpp \
    TestCommandProcessor.cpp \
    ../QAbstractExchanger.cpp \
    Exchanger.cpp \
    TestAbstractExchanger.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ../CommandBuffer.h \
    ../QAnswerBuffer.h \
    TestCommandBuffer.h \
    TestAnswerBuffer.h \
    ../CommandProcessor.h \
    TestCommandProcessor.h \
    ../QAbstractExchanger.h \
    Exchanger.h \
    ../QCommandExchangersGlobal.h \
    TestAbstractExchanger.h

DISTFILES += \
    ../readme.MD
