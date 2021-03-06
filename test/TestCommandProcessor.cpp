#include <QTest>
#include <QThread>

#include "TestCommandProcessor.h"

static quint32 id1 = 123;
static quint32 id2 = 456;
static quint32 id3 = 789;

CTestCommandProcessor::CTestCommandProcessor(QObject *parent) :
  QObject(parent)
{
  static CCommandBuffer::STextParsingDesc d1 = {"+NOT:", ','};
  static CCommandBuffer::STextParsingDesc d2 = {"+HI:", ','};
  static CCommandBuffer::STextParsingDesc d3 = {"+BYE:", ','};

  CCommandProcessor::TAnswersDescrList answersList;
  answersList.append(
          CCommandProcessor::SAnswerDescr(id1, &d1)
         );
  answersList.append(
          CCommandProcessor::SAnswerDescr(id2, &d2)
        );
  answersList.append(
          CCommandProcessor::SAnswerDescr(id3, &d3)
        );

  this->cmdProcessor = new CCommandProcessor(&answersList);
  this->spyGotAnswer = new QSignalSpy(this->cmdProcessor, SIGNAL(SignalGotAnswer(const QAnswerBuffer&)));
}

/**
  * @brief  Отправляем команду без ответа, проверка на возвращение объекта ответа с флагом ошибки таймаута
  * @param
  * @retval
  */
void CTestCommandProcessor::testSignalTimeout()
{
  quint32 idTest = 345;

  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, NULL, true)
        );

  QVERIFY(this->spyGotAnswer->wait(2000));
  QCOMPARE(this->spyGotAnswer->count(), 1);

  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));

  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resTimeout);
  QCOMPARE(answer.getCmdId(), idTest);

  this->spyGotAnswer->clear();
  QCOMPARE(this->spyGotAnswer->count(), 0);
}

/**
  * @brief  Тест для определения получения только резултата выполнения команды
  * @param
  * @retval
  */
void CTestCommandProcessor::testGettingResult()
{
  quint32 idTest = 245;

  // Проверяем получение ответа об успешном выполнении команды
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, NULL, true)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n$OK\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE(answer.getCmdId(), idTest);
  this->spyGotAnswer->clear();

  // Проверяем получение ответа об ошибчном выполнении команды
  idTest = 896;
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, NULL, true)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n$ERR:233\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resError);
  QCOMPARE(answer.getResultCode(), (quint32)233);
  QCOMPARE(answer.getCmdId(), idTest);

  this->spyGotAnswer->clear();
}

/**
  * @brief  Тест для получения ответа на команду, без резульата ее выполнения
  * @param
  * @retval
  */
void CTestCommandProcessor::testGettingAnswer()
{
  quint32 idTest = 1909;

  // Проверяем получение ответа об успешном выполнении команды
  static CCommandBuffer::STextParsingDesc descr = {"+RESP:", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, &descr, false)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n+RESP:HELLO\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE(answer.getCmdId(), idTest);
  QCOMPARE(answer.size(), 1);
  QCOMPARE(answer.at(0).getParamString(0), QString("HELLO"));

  this->spyGotAnswer->clear();
}

/**
  * @brief  Тест для получения ответа на команду и результата ее выполнения
  * @param
  * @retval
  */
void CTestCommandProcessor::testGettingAnswerResult()
{
  quint32 idTest = 964;

  // Проверяем получение ответа об успешном выполнении команды
  static CCommandBuffer::STextParsingDesc descr = {"+RESP:", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, &descr, true)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n+RESP:1,2,3,4,5\r\n$OK\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE(answer.getCmdId(), idTest);
  QCOMPARE(answer.size(), 1);
  QStringList sample = {"1", "2", "3", "4", "5"};
  QCOMPARE(answer.at(0).getParamStringList(0), sample);

  this->spyGotAnswer->clear();

  // Проверяем получение ответа и статуса ошибки выполнения команды
  static CCommandBuffer::STextParsingDesc descr2 = {"+RESP:", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, &descr2, true)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n+RESP:AADD\r\n$ERR:123\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resError);
  QCOMPARE(answer.getResultCode(), (quint32)123);
  QCOMPARE(answer.getCmdId(), idTest);
  QCOMPARE(answer.size(), 1);
  QCOMPARE((quint32)answer.at(0).getParamIntFromHex(0), (quint32)0xAADD);

  this->spyGotAnswer->clear();

  // Проверяем получение ответа без получения статуса, хотя его ожидали, ошибка таймаута
  static CCommandBuffer::STextParsingDesc descr3 = {"+RESP:", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, &descr3, true)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n+RESP:AADD\r\n");
  this->spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resTimeout);
  QCOMPARE(answer.getCmdId(), idTest);

  this->spyGotAnswer->clear();
}

/**
  * @brief  Тесты для получения многострочных ответов
  * @param
  * @retval
  */
void CTestCommandProcessor::testManyString()
{
  quint32 idTest = 964;

  // Проверяем получение ответа об успешном выполнении команды
  static CCommandBuffer::STextParsingDesc descr = {"+RESP:", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, &descr, true)
        );

  this->cmdProcessor->SlotIncomingData("+RESP:1,2,3,4,5\r\nSOME_SHIT\r\n"
                                       "+RESP:HELLO\r\n"
                                       "SHIT_AGAIN\r\n"
                                       "+RESP:DDFF\r\n"
                                       "+RESP:123\r\n"
                                       "$OK\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE(answer.getCmdId(), idTest);
  QCOMPARE(answer.size(), 4);
  QStringList sampleList = {"1", "2", "3", "4", "5"};
  QCOMPARE(answer.at(0).getParamStringList(0), sampleList);
  QCOMPARE(answer.at(1).getParamString(0), QString("HELLO"));
  QCOMPARE((quint32)answer.at(2).getParamIntFromHex(0), (quint32)0xDDFF);
  QCOMPARE((quint32)answer.at(3).getParamInt(0), (quint32)123);

  this->spyGotAnswer->clear();
}

/**
  * @brief  Тест для получения не стандартного результата выполнения команды
  * @param
  * @retval
  */
void CTestCommandProcessor::testDifResult()
{
  quint32 idTest = 131234;

  // Проверяем получение ответа об успешном выполнении команды
  static CCommandBuffer::STextParsingDesc descrOk = {"OK!", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest, NULL, true, 1000, &descrOk)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nOK!\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);

  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE(answer.getCmdId(), idTest);
  this->spyGotAnswer->clear();
}

/**
  * @brief  Тест для получения не ожидаемого ответа
  * @param
  * @retval
  */
void CTestCommandProcessor::testUnexpected()
{
  quint32 idTest = id3;

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\nAGAIN\r\n+BYE:32,44\r\n");

  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);
  QAnswerBuffer answer = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QCOMPARE(answer.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer.getResultCode(), (quint32)0);
  QCOMPARE(answer.getCmdId(), idTest);

  CCommandBuffer answerString = answer.first();
  QCOMPARE(answerString.getParamInt(0), 32);
  QCOMPARE(answerString.getParamInt(1), 44);

  this->spyGotAnswer->clear();
}

/**
  * @brief  Тест на обработку последовательно отправленных команд
  * @param
  * @retval
  */
void CTestCommandProcessor::testManyCommands()
{
  quint32 idTest1 = 111;
  quint32 idTest2 = 222;

  // Проверяем получение ответа об успешном выполнении команды
  static CCommandBuffer::STextParsingDesc descr1 = {"+RESP:", ','};
  static CCommandBuffer::STextParsingDesc descr2 = {"+THERE:", ','};
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest1, &descr1, false, 5000)
        );
  this->cmdProcessor->addAnswerWait(
          CCommandProcessor::SAnswerDescr(idTest2, &descr2, true, 5000)
        );

  this->cmdProcessor->SlotIncomingData("SOME_SHIT\r\n+RESP:1,2,3\r\nAGAIN_TRASH");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 1);
  QCOMPARE(this->cmdProcessor->isEmpty(), false);

  this->cmdProcessor->SlotIncomingData("AGAIN_TRASH\r\n+THERE:HELLO\r\n$OK\r\n");
  spyGotAnswer->wait(2000);
  QCOMPARE(spyGotAnswer->count(), 2);

  QAnswerBuffer answer1 = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(0).at(0));
  QAnswerBuffer answer2 = qvariant_cast<QAnswerBuffer>(spyGotAnswer->at(1).at(0));
  QCOMPARE(answer1.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer1.getResultCode(), (quint32)0);
  QCOMPARE(answer1.getCmdId(), idTest1);
  QCOMPARE(answer1.size(), 1);
  QCOMPARE(answer1.at(0).getParamString(0), QString("1"));
  QCOMPARE(answer1.at(0).getParamInt(1), 2);

  QCOMPARE(answer2.getResultStatus(), QAnswerBuffer::resOk);
  QCOMPARE(answer2.getResultCode(), (quint32)0);
  QCOMPARE(answer2.getCmdId(), idTest2);
  QCOMPARE(answer2.size(), 1);
  QCOMPARE(answer2.at(0).getParamString(0), QString("HELLO"));

  QCOMPARE(this->cmdProcessor->isEmpty(), true);

  this->spyGotAnswer->clear();
}


