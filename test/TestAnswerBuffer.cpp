#include <QDebug>
#include <QTest>
#include "TestAnswerBuffer.h"

static const CCommandBuffer::STextParsingDesc diag = {"$DIAG", ','};
static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};


CTestAnswerBuffer::CTestAnswerBuffer(QObject *parent) :
  QObject(parent),
  buffer(0)
{

}

void CTestAnswerBuffer::testAppend()
{
  QList<QByteArray> samples = {
    "$DIAG:HSE,AABB",
    "$DIAG:RTC,0,1,2,3,4,5",
    "$DIAG:ShDt,1",
    "$DIAG:Flsh,0",
    "$DIAG:GSM,0",
    "$DIAG:SIM,8",
    "$DIAG:SMST,-1",
    "$DIAG:GPS,11",
    "$OK",
  };

  CCommandBuffer cmdBuf;

  for (int i = 0; i < samples.size(); i++) {
    cmdBuf.append(samples[i]);
    cmdBuf.append((char)0x00);
  }

  while (cmdBuf.checkLine() == CCommandBuffer::lineCompleted) {

   if (cmdBuf.parse(descrOk) == CCommandBuffer::parseOk) {
     break;
   }

   if (cmdBuf.parse(diag) == CCommandBuffer::parseOk) {
    this->buffer.append(cmdBuf.getLine(), diag);
   }

   cmdBuf.releaseLine();
 } // while

  QCOMPARE(this->buffer.size(), samples.size() - 1);

  int i = 0;
  foreach (CCommandBuffer buf, this->buffer) {
    QCOMPARE(buf.getLine(), samples.at(i++));
  }

}

void CTestAnswerBuffer::testParams()
{
  CCommandBuffer buf;

  buf = this->buffer.at(0);
  int sample1 = 0xAABB;
  int res1 = buf.getParamIntFromHex(1);
  QCOMPARE(sample1, res1);

  buf = this->buffer.at(1);
  QStringList sample2 = {"0", "1", "2", "3", "4", "5"};
  QStringList res2 = buf.getParamStringList(1);
  QCOMPARE(sample2, res2);

  buf = this->buffer.at(2);
  QString sample3("ShDt");
  QString res3 = buf.getParamString(0);
  QCOMPARE(sample3, res3);
}

void CTestAnswerBuffer::testFirstLast()
{
  QAnswerBuffer answerBuf;

  CCommandBuffer buf1 = answerBuf.first();
  CCommandBuffer buf2 = answerBuf.last();
}

