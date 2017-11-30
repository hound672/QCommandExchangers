#include <QTest>
#include "TestCommandBuffer.h"

static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};
static const CCommandBuffer::STextParsingDesc descrErr = {"$ERR", ','};
static const CCommandBuffer::STextParsingDesc testCmd = {"$TEST", ','};
static const CCommandBuffer::STextParsingDesc difSep = {"$DIFSEP", '/'};


CTestCommandBuffer::CTestCommandBuffer(QObject *parent) : QObject(parent)
{
  buffer.append("$OK\r\n$ERR:256\r\n$TEST:9812,AABB,HELLO,1,2,3\r\n$DIFSEP:1/2/3\r\n");
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
  QCOMPARE(buffer.parse(descrOk), CCommandBuffer::PARSE_OK);
}

void CTestCommandBuffer::testError()
{
  bool gotError = false;
  bool gotCmdAnswer = false;
  bool gotDifSep = false;

  while (this->buffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {

    if (this->buffer.parse(descrErr) == CCommandBuffer::PARSE_OK) {
      gotError = true;
      QCOMPARE(this->buffer.startsWith("$ERR"), true);
      int errorCode = this->buffer.getParamInt(0);
      QCOMPARE(errorCode, 256);
    }

    if (this->buffer.parse(testCmd) == CCommandBuffer::PARSE_OK) {
      gotCmdAnswer = true;

      int val1 = this->buffer.getParamInt(0);
      QCOMPARE(val1, 9812);

      int val2 = this->buffer.getParamIntFromHex(1);
      QCOMPARE(val2, 0xAABB);

      QString val3 = this->buffer.getParamString(2);
      QCOMPARE(val3, QString("HELLO"));

      QStringList val4 = this->buffer.getParamStringList(3);
      QStringList sampleList = {"1", "2", "3"};
      QCOMPARE(val4, sampleList);
    }

    if (this->buffer.parse(difSep) == CCommandBuffer::PARSE_OK) {
      gotDifSep = true;

      QStringList sampleList = {"1", "2", "3"};
      QStringList val1 = this->buffer.getParamStringList(0);
      QCOMPARE(val1, sampleList);
    }

    this->buffer.releaseLine();
  }

  QCOMPARE(gotError, true);
  QCOMPARE(gotCmdAnswer, true);
  QCOMPARE(gotDifSep, true);
}

