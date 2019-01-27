#include <QDebug>
#include <QTest>
#include "TestCommandBuffer.h"

static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};
static const CCommandBuffer::STextParsingDesc descrErr = {"$ERR", ','};
static const CCommandBuffer::STextParsingDesc testCmd = {"$TEST", ','};
static const CCommandBuffer::STextParsingDesc testCmd2 = {"$TST", ','};
static const CCommandBuffer::STextParsingDesc difSep = {"$DIFSEP", '/'};
static const CCommandBuffer::STextParsingDesc diag = {"$DIAG", ','};


CTestCommandBuffer::CTestCommandBuffer(QObject *parent) : QObject(parent)
{
  buffer.append("$OK\r\n$ERR:256\r\n$TEST:9812,AABB,HELLO,1,2,3\r\n$TST:0.58,e=1,123\r\n$DIFSEP:1/2/3\r\n");
}

void CTestCommandBuffer::testContructor()
{
  QByteArray tmpStr("HELLO_WORLD\r\n");
  CCommandBuffer cmdBuf1;
  CCommandBuffer cmdBuf2(tmpStr);

  QCOMPARE(cmdBuf1.size(), 0);
  QCOMPARE(cmdBuf2.size(), tmpStr.size());

  QCOMPARE(cmdBuf2.checkLine(), CCommandBuffer::lineCompleted);
  QCOMPARE(cmdBuf2.getLine(), QByteArray("HELLO_WORLD"));
}

void CTestCommandBuffer::checkLine()
{
  QCOMPARE(buffer.checkLine(), CCommandBuffer::lineCompleted);
}

void CTestCommandBuffer::getLine()
{
  QCOMPARE(buffer.getLine(), QByteArray("$OK"));
}

void CTestCommandBuffer::parse()
{
  QCOMPARE(buffer.parse(descrOk), CCommandBuffer::parseOk);
}

void CTestCommandBuffer::testError()
{
  bool gotError = false;
  bool gotCmdAnswer = false;
  bool gotCmd2Answer = false;
  bool gotDifSep = false;

  while (this->buffer.checkLine() == CCommandBuffer::lineCompleted) {

    if (this->buffer.parse(descrErr) == CCommandBuffer::parseOk) {
      gotError = true;
      QCOMPARE(this->buffer.startsWith("$ERR"), true);
      int errorCode = this->buffer.getParamInt(0);
      QCOMPARE(errorCode, 256);
    }

    if (this->buffer.parse(testCmd) == CCommandBuffer::parseOk) {
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

    if (this->buffer.parse(testCmd2) == CCommandBuffer::parseOk) {
      // $TST:0.58,e=1,123
      gotCmd2Answer = true;
      bool res;

      double val1 = this->buffer.getParamDouble(0, &res);
      QCOMPARE(val1, 0.58);
      QCOMPARE(res, true);

      int val2 = this->buffer.getParamInt(1, &res);
      QCOMPARE(res, false) ;

      int val3 = this->buffer.getParamInt(2, &res);
      QCOMPARE(val3, 123);
      QCOMPARE(res, true);
    }

    if (this->buffer.parse(difSep) == CCommandBuffer::parseOk) {
      gotDifSep = true;

      QStringList sampleList = {"1", "2", "3"};
      QStringList val1 = this->buffer.getParamStringList(0);
      QCOMPARE(val1, sampleList);
    }

    this->buffer.releaseLine();
  }

  QCOMPARE(gotError, true);
  QCOMPARE(gotCmdAnswer, true);
  QCOMPARE(gotCmd2Answer, true);
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

   while (this->buffer.checkLine() == CCommandBuffer::lineCompleted) {
    qDebug() << "Process line: " << this->buffer.getLine();

    if (this->buffer.parse(descrOk) == CCommandBuffer::parseOk) {
      break;
    }

    QCOMPARE(this->buffer.parse(diag), CCommandBuffer::parseOk);
    QCOMPARE(this->buffer.getLine(), samples.at(cnt));
    cnt++;

    this->buffer.releaseLine();
     }
}

QTEST_MAIN(CTestCommandBuffer)
