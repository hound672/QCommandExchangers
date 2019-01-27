DEPTH = ../../..
include($${DEPTH}/test/tests.pri)
INCLUDEPATH += $${DEPTH}

TARGET = TestCommandProcessor

SOURCES = TestCommandProcessor.cpp \ 
    ../../../CommandBuffer.cpp \
    ../../../CommandProcessor.cpp \
    ../../../QAbstractExchanger.cpp \
    ../../../QAnswerBuffer.cpp

HEADERS += \
    TestCommandProcessor.h \
    ../../../CommandBuffer.h \
    ../../../CommandProcessor.h \
    ../../../QAbstractExchanger.h \
    ../../../QAnswerBuffer.h
