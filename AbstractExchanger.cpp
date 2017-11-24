#include <QDebug>

#include "AbstractExchanger.h"


// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

CAbstractExchanger::CAbstractExchanger(QObject *parent) : QObject(parent)
{
  this->commandProcessor = new CCommandProcessor();

  connect(this->commandProcessor, SIGNAL(signalGotAnswer(const CAnswerBuffer&)),
          this, SLOT(slotGotAnswer(const CAnswerBuffer&)));
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
}

void CAbstractExchanger::slotGotAnswer(const CAnswerBuffer &answer)
{
  qDebug() << "Got answer: " << answer;
  emit this->signalGotAnswer(answer);
}

/* ******************* END *****************************
   ****************** PROTECTED методы ********************
   ***************************************************** */

// ---------------------------------------------------------------
