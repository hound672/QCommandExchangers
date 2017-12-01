#include <QDebug>
#include <QTest>
#include "TestCommandBuffer.h"

static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};
static const CCommandBuffer::STextParsingDesc descrErr = {"$ERR", ','};
static const CCommandBuffer::STextParsingDesc testCmd = {"$TEST", ','};
static const CCommandBuffer::STextParsingDesc difSep = {"$DIFSEP", '/'};
static const CCommandBuffer::STextParsingDesc diag = {"$DIAG", ','};


CTestCommandBuffer::CTestCommandBuffer(QObject *parent) : QObject(parent)
{
  buffer.append("$OK\r\n$ERR:256\r\n$TEST:9812,AABB,HELLO,1,2,3\r\n$DIFSEP:1/2/3\r\n");
}

void CTestCommandBuffer::testContructor()
{
  QByteArray tmpStr("HELLO_WORLD\r\n");
  CCommandBuffer cmdBuf1;
  CCommandBuffer cmdBuf2(tmpStr);

  QCOMPARE(cmdBuf1.size(), 0);
  QCOMPARE(cmdBuf2.size(), tmpStr.size());

  QCOMPARE(cmdBuf2.checkLine(), CCommandBuffer::LINE_COMPELETED);
  QCOMPARE(cmdBuf2.getLine(), QByteArray("HELLO_WORLD"));
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

void CTestCommandBuffer::testClear()
{
  this->buffer.clear();
  QCOMPARE(this->buffer.isEmpty(), true);
}

void CTestCommandBuffer::testDiagParse()
{
  this->buffer.clear();

  int cnt = 0;

  QList<QByteArray> samples = {
    "$DIAG:HSE,0",
    "$DIAG:RTC,0",
    "$DIAG:ShDt,0",
    "$DIAG:Flsh,0",
    "$DIAG:GSM,0",
    "$DIAG:SIM,8",
    "$DIAG:SMST,-1",
    "$DIAG:GPS,11",
    "$OK",
  };

  for (int i = 0; i < samples.size(); i++) {
    qDebug() << "Add sample: " << samples.at(i);
    this->buffer.append(samples[i]);
    this->buffer.append((char)0x00);
  }

   while (this->buffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {
    qDebug() << "Process line: " << this->buffer.getLine();

    if (this->buffer.parse(descrOk) == CCommandBuffer::PARSE_OK) {
      break;
    }

    QCOMPARE(this->buffer.parse(diag), CCommandBuffer::PARSE_OK);
    QCOMPARE(this->buffer.getLine(), samples.at(cnt));
    cnt++;

    this->buffer.releaseLine();
  }
}

