DEPTH = ../../..
include($${DEPTH}/test/tests.pri)
INCLUDEPATH += $${DEPTH}

TARGET = TestCommandBuffer

SOURCES = TestCommandBuffer.cpp \ 
    ../../../CommandBuffer.cpp \
    ../../../CommandProcessor.cpp \
    ../../../QAbstractExchanger.cpp \
    ../../../QAnswerBuffer.cpp

HEADERS += \
    TestCommandBuffer.h \
    ../../../CommandBuffer.h \
    ../../../CommandProcessor.h \
    ../../../QAbstractExchanger.h \
    ../../../QAnswerBuffer.h
