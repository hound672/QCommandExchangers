#include <QDebug>

#include "AbstractExchanger.h"


// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

CAbstractExchanger::CAbstractExchanger(QObject *parent) :
  QObject(parent)
{
  this->commandProcessor = new CCommandProcessor(parent);
  this->makeSignalSlots();
}

CAbstractExchanger::CAbstractExchanger(const CCommandProcessor::TAnswersList &answersList, QObject *parent) :
  QObject(parent)
{
  this->commandProcessor = new CCommandProcessor(answersList, parent);
  this->makeSignalSlots();
}

/**
  * @brief  Отправляет команду на устройство и добавлет в очередь ожидание на ее ответ
  * @param
  * @retval
  */
void CAbstractExchanger::sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr)
{
  this->sendData(cmdToSend, false);
  this->commandProcessor->addAnswerWait(answerDescr);
}

/* ******************* END *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

// ---------------------------------------------------------------

/**
  * @brief  Создаем связки сигналов слотов
  * @param
  * @retval
  */
void CAbstractExchanger::makeSignalSlots()
{
  connect(this->commandProcessor, SIGNAL(signalGotAnswer(const CAnswerBuffer&)),
          this, SLOT(slotGotAnswer(const CAnswerBuffer&)));
}

// ------------------------------------------------

/* ***************** BEGIN *****************************
   ****************** PROTECTED методы ********************
   ***************************************************** */

/**
  * @brief  Получили данные от устройства
  * @param
  * @retval
  */
void CAbstractExchanger::gotIncomingData(const QByteArray &answer)
{
  this->commandProcessor->slotIncomingData(answer);
  emit this->signalGotRawData(answer);
}

void CAbstractExchanger::slotGotAnswer(const CAnswerBuffer &answer)
{
  emit this->signalGotAnswer(answer);
}

/* ******************* END *****************************
   ****************** PROTECTED методы ********************
   ***************************************************** */

// ---------------------------------------------------------------
