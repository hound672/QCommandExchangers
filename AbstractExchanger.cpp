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
  * @brief  Формирует и отправляет команду на трекер
  * @param  cmdName: имя команды (после AT+)
  * @param  cmdType: тип команды (запрос, выполнение, изменение параметров)
  * @param  params: параметры команды (после '=' знака ровно) без параметров - запрос параметров
  * @retval
  */
void CAbstractExchanger::sendCommand(const QString &cmdName, CAbstractExchanger::ETypeCommand cmdType, const QString &cmdParams, const QByteArray &cmdBinData)
{
  QByteArray dataToSend;
  bool waitAnswer = true;

  switch (cmdType) {
    case ETypeCommand::T_CMD_REQUEST:
      dataToSend = QString("AT+%1=?").arg(cmdName).toUtf8();
      break;

    case ETypeCommand::T_CMD_EXEC:
      dataToSend = QString("AT+%1=%2").arg(cmdName).arg(this->trackerPassword).toUtf8();
      break;

    case ETypeCommand::T_CMD_SET:
      dataToSend = QString("AT+%1=%2,%3").arg(cmdName).arg(this->trackerPassword).arg(cmdParams).toUtf8();
      break;

    case ETypeCommand::T_CMD_TERMINAL:
      qDebug() << "TERMINAL!!!";
      waitAnswer = false;
      dataToSend = cmdName.toUtf8();
      break;

    case ETypeCommand::T_CMD_INTERNAL_REQUEST:
      dataToSend = QString("$%1?").arg(cmdName).toUtf8();
      break;

    case ETypeCommand::T_CMD_INTERNAL_SET:
      dataToSend = QString("$%1=%2").arg(cmdName).arg(cmdParams).toUtf8();
      break;

    case ETypeCommand::T_CMD_INTERNAL_SET_BINDATA: {
      dataToSend = QString("$%1=%2,").arg(cmdName).arg(cmdParams).toUtf8();
      dataToSend.append(cmdBinData);
      break;
    }
  }

  qDebug() << "Send command to device: " << dataToSend;
  static const CCommandBuffer::STextParsingDesc descr = {"PIO", ','};
  this->commandProcessor->addAnswerWait(CCommandProcessor::SAnswerDescr(1, &descr, true));
  this->sendData("$PIO?\r\n", false);
  //  this->sendData(dataToSend, waitAnswer);
}

/**
  * @brief  Отправляет команду на устройство и добавлет в очередь ожидание на ее ответ
  * @param
  * @retval
  */
void CAbstractExchanger::sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr)
{
  this->sendData(cmdToSend);
  this->commandProcessor->addAnswerWait(answerDescr);
}

/**
  * @brief  Изменяем пароль трекера в классе
  * @param
  * @retval
  */
void CAbstractExchanger::setTrackerPassword(const QString password)
{
  this->trackerPassword = password;
}

/* ******************* END *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ****************** PROTECTED методы ********************
   ***************************************************** */

/**
  * @brief  Получили полный ответ от устройства
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
}

/* ******************* END *****************************
   ****************** PROTECTED методы ********************
   ***************************************************** */

// ---------------------------------------------------------------
