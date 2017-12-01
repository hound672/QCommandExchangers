#include <QDebug>
#include <QTest>
#include "TestAnswerBuffer.h"

static const CCommandBuffer::STextParsingDesc diag = {"$DIAG", ','};


CTestAnswerBuffer::CTestAnswerBuffer(QObject *parent) :
  QObject(parent),
  buffer(0, &diag)
{

}

void CTestAnswerBuffer::makeStrings()
{
  CCommandBuffer cmdBuffer;

  CAnswerBuffer answer(0, &diag);

  QList<QByteArray> samples = {
    "$DIAG:HSE,0,1,2",
    "$DIAG:RTC,0,1,2",
    "$DIAG:ShDt,0,1,2",
    "$DIAG:Flsh,0,1,2",
    "$DIAG:GSM,0,1,2",
    "$DIAG:SIM,8,1,2",
    "$DIAG:SMST,-1,1,2",
    "$DIAG:GPS,11,1,2",
  };

  for (int i = 0; i < samples.size(); i++) {
    qDebug() << "Add sample: " << samples.at(i);
    cmdBuffer.append(samples[i]);
    cmdBuffer.append((char)0x00);
  }

  while (cmdBuffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {
    qDebug() << "Process line: " << cmdBuffer.getLine();

    if (cmdBuffer.parse(diag) == CCommandBuffer::PARSE_OK) {
      answer.appendString(cmdBuffer.getLine());
    }

   cmdBuffer.releaseLine();
  }

  this->testShowStrings(answer);
}

void CTestAnswerBuffer::testShowStrings(const CAnswerBuffer &answer1)
{
  CAnswerBuffer answer = answer1;
  QCOMPARE(answer.getParamString(0), QString("HSE"));

  do {
    qDebug() << "Get param: " << answer.getParamArray(0)
             << ". List: " << answer.getParamStringList(1);
    answer.releaseLine();
  } while (answer.checkLine() == CCommandBuffer::LINE_COMPELETED);
}

void CTestAnswerBuffer::testAppendString()
{
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
  };

  for (int i = 0; i < samples.size(); i++) {
    qDebug() << "Add sample: " << samples.at(i);
    this->buffer.appendString(samples[i]);
  }

  QCOMPARE(buffer.checkLine(), CCommandBuffer::LINE_COMPELETED);
  while (this->buffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {
   qDebug() << "Process line: " << this->buffer.getLine();
   qDebug() << "Get param: " << this->buffer.getParamArray(0);

   QCOMPARE(this->buffer.parse(diag), CCommandBuffer::PARSE_OK);
   QCOMPARE(this->buffer.getLine(), samples.at(cnt));
   cnt++;

   this->buffer.releaseLine();
 }

}

void CTestAnswerBuffer::testGetParam()
{
  this->makeStrings();
}
