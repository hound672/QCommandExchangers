DEPTH = ../../..
include($${DEPTH}/test/tests.pri)
INCLUDEPATH += $${DEPTH}

TARGET = TestAbstractExchanger

SOURCES = TestAbstractExchanger.cpp \ 
    Exchanger.cpp \
    ../../../CommandBuffer.cpp \
    ../../../CommandProcessor.cpp \
    ../../../QAbstractExchanger.cpp \
    ../../../QAnswerBuffer.cpp

HEADERS += \
    TestAbstractExchanger.h \
    Exchanger.h \
    ../../../CommandBuffer.h \
    ../../../CommandProcessor.h \
    ../../../QAbstractExchanger.h \
    ../../../QAnswerBuffer.h
