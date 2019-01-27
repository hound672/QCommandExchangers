DEPTH = ../../..
include($${DEPTH}/test/tests.pri)
INCLUDEPATH += $${DEPTH}

TARGET = TestAnswerBuffer

SOURCES = TestAnswerBuffer.cpp \ 
    ../../../CommandBuffer.cpp \
    ../../../CommandProcessor.cpp \
    ../../../QAbstractExchanger.cpp \
    ../../../QAnswerBuffer.cpp

HEADERS += \
    TestAnswerBuffer.h \
    ../../../CommandBuffer.h \
    ../../../CommandProcessor.h \
    ../../../QAbstractExchanger.h \
    ../../../QAnswerBuffer.h
