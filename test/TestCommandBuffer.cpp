#include <QTest>
#include "TestCommandBuffer.h"

CTestCommandBuffer::CTestCommandBuffer(QObject *parent) : QObject(parent)
{
  buffer.append("$OK\r\n");
}

void CTestCommandBuffer::checkLine()
{
  QCOMPARE(buffer.checkLine(), CCommandBuffer::LINE_COMPELETED);
}

void CTestCommandBuffer::getLine()
{
  QCOMPARE(buffer.getLine(), QByteArray("$OK"));
}

void CTestCommandBuffer::parse()
{
  static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};
  QCOMPARE(buffer.parse(descrOk), CCommandBuffer::PARSE_OK);
}

