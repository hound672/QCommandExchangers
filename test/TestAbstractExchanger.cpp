#include <QTest>

#include "AnswerBuffer.h"

#include "QCommandExchangersGlobal.h"
#include "TestAbstractExchanger.h"

// ======================================================================

CTestAbstractExchanger::CTestAbstractExchanger(QObject *parent) : QObject(parent)
{
  mExchanger = new CExchanger();
  mSpyAnswer = new QSignalSpy(mExchanger, SIGNAL(signalGotAnswer(const CAnswerBuffer&)));
}

// ======================================================================

/**
  * @brief  Тест: получение "сырых" данных
  * @param  
  * @retval 
  */
void CTestAbstractExchanger::testGettingsRawData()
{
  QSignalSpy *spy = new QSignalSpy(mExchanger, SIGNAL(signalGotRawData(const QByteArray&)));
  
  QByteArray dataSend("TEST RAW DATA");
  mExchanger->putData(dataSend);
  QCOMPARE(spy->count(), 1);
  QByteArray dataRead = qvariant_cast<QByteArray>(spy->at(0).at(0));
  QCOMPARE(dataSend, dataRead);
  
  delete spy;
}

// ======================================================================

/**
  * @brief  Тест: получение ответа от класса
  * @param  
  * @retval 
  */
void CTestAbstractExchanger::testGettingAnswer()
{
  int cmdId = 123;
  
  static CCommandBuffer::STextParsingDesc descr = {"+RESP", ','};
  CCommandProcessor::SAnswerDescr answerDescr(cmdId, &descr);
  
  mExchanger->sendCommand("TEST_COMMAND", answerDescr);
  mExchanger->putData("\r\n+RESP:HELLO:123,HI\r\n");
  
  mSpyAnswer->wait(2000);
  QCOMPARE(mSpyAnswer->count(), 1);
  
  CAnswerBuffer answer = qvariant_cast<CAnswerBuffer>(mSpyAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), CAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE((int)answer.getCmdId(), cmdId);
  
  mSpyAnswer->clear();
}

void CTestAbstractExchanger::testCommandEnd()
{
  QSignalSpy *spy = new QSignalSpy(mExchanger, SIGNAL(signalCommandEnd(const TAnswersList&)));
  
  quint32 cmd1 = 1;
  quint32 cmd2 = 2;
  quint32 cmd3 = 3;
  
  static CCommandBuffer::STextParsingDesc descr1 = {"+CMD1", ','};
  static CCommandBuffer::STextParsingDesc descr2 = {"+CMD2", ','};
  static CCommandBuffer::STextParsingDesc descr3 = {"+CMD3", ','};
  
  CCommandProcessor::SAnswerDescr answerDescr1(cmd1, &descr1);
  CCommandProcessor::SAnswerDescr answerDescr2(cmd2, &descr2);
  CCommandProcessor::SAnswerDescr answerDescr3(cmd3, &descr3);
  
  // ======================================================================
  
  mExchanger->sendCommand("", answerDescr1);
  mExchanger->sendCommand("", answerDescr2);
  mExchanger->sendCommand("", answerDescr3);
  
  mExchanger->putData("\r\n+CMD1:1,2\r\n+CMD2:3,4\r\n");
  
  spy->wait(2000);
  QCOMPARE(spy->count(), 1);
  QCOMPARE(mSpyAnswer->count(), 3);
  
  CAbstractExchanger::TAnswersList answersList;
  answersList = qvariant_cast<CAbstractExchanger::TAnswersList>(spy->at(0).at(0));
  
  QCOMPARE(answersList.count(), 3);
  
  CAnswerBuffer answer1 = answersList.at(0);
  CAnswerBuffer answer2 = answersList.at(1);
  CAnswerBuffer answer3 = answersList.at(2);
  
  QCOMPARE(answer1.getResultStatus(), CAnswerBuffer::resOk);
  QCOMPARE(answer1.getResultCode(), (quint32)0);
  QCOMPARE(answer1.getCmdId(), cmd1);

  QCOMPARE(answer2.getResultStatus(), CAnswerBuffer::resOk);
  QCOMPARE(answer2.getResultCode(), (quint32)0);
  QCOMPARE(answer2.getCmdId(), cmd2);
  
  QCOMPARE(answer3.getResultStatus(), CAnswerBuffer::resTimeout);
  QCOMPARE(answer3.getResultCode(), (quint32)0);
  QCOMPARE(answer3.getCmdId(), cmd3);

  // ======================================================================
  spy->clear();
  mSpyAnswer->clear();
  
  delete spy;
}

// ======================================================================
