#include <QDebug>

#include "AbstractExchanger.h"


// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

CAbstractExchanger::CAbstractExchanger(QObject *parent) :
  QObject(parent),
  directMode(false)
{
  this->commandProcessor = new CCommandProcessor();

  connect(this->commandProcessor, SIGNAL(signalGotAnswer(const CAnswerBuffer&)),
          this, SLOT(slotGotAnswer(const CAnswerBuffer&)));
  connect(this->commandProcessor, SIGNAL(signalErrorTimeout(const CAnswerBuffer&)),
          this, SLOT(slotErrorTimeout(const CAnswerBuffer&)));
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

/**
  * @brief  Изменяет режим прямой отправки данных, при котором все данные отправляются "как есть"
  *         и принимаются "как есть"
  * @param
  * @retval
  */
void CAbstractExchanger::setDirectMode(bool mode)
{
  this->directMode = mode;
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
  if (this->directMode) {
    CAnswerBuffer data;
    data.append(answer);
    emit this->signalGotAnswer(data);
    return;
  }

  this->commandProcessor->slotIncomingData(answer);
}

void CAbstractExchanger::slotGotAnswer(const CAnswerBuffer &answer)
{
  emit this->signalGotAnswer(answer);
}

void CAbstractExchanger::slotErrorTimeout(const CAnswerBuffer &answer)
{
  emit this->signalErrorTimeout(answer);
}

/* ******************* END *****************************
   ****************** PROTECTED методы ********************
   ***************************************************** */

// ---------------------------------------------------------------
