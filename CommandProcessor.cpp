#include <QDebug>

#include "CommandProcessor.h"


static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};

CCommandProcessor::CCommandProcessor(QObject *parent) : QObject(parent)
{
  connect(&(this->timer), SIGNAL(timeout()),
          this, SLOT(slotTimeout()));

  this->timer.start(CCommandProcessor::TIMEOUT);
}

void CCommandProcessor::sendCommand(const QByteArray &command, const CCommandProcessor::SAnswerDescr &answerDescr)
{
  // send command to device
  this->commandsList.append(answerDescr);
}

/**
  * @brief  Добавляет в очередь ожидание ответа на команду
  * @param
  * @retval
  */
void CCommandProcessor::addAnswerWait(const CCommandProcessor::SAnswerDescr &answerDescr)
{
  this->commandsList.append(answerDescr);
}

/**
  * @brief  Получен полный ответ на команду
  * @param
  * @retval
  */
void CCommandProcessor::gotFullAnswer()
{

  CCommandProcessor::SAnswerDescr answerDescr = this->commandsList[0];
  qDebug() << "Got full answer: " << answerDescr.answer;

//  this->answerBuffer.setCmdId(answerDescr.m_cmdId);
  emit this->signalGotAnswer(answerDescr.answer);
  this->removeFirstCommand();
}

/**
  * @brief  Удаляет из списка команд первую в очереди (с проверкой наличия команд в очереди)
  * @param
  * @retval
  */
void CCommandProcessor::removeFirstCommand()
{
  if (!this->commandsList.isEmpty()) {
    this->commandsList.removeFirst();
  }
}

/**
  * @brief  Слот для обработки входящих данных
  * @param
  * @retval
  */
void CCommandProcessor::slotIncomingData(const QByteArray &data)
{
  //if (this->commandsList.isEmpty()) {
    // пришли данные, когда не было ожидания команды
    // как вариант в этом случае разбирать по шаблону ассинхронных данных
    // такие как внезапные сообщения
    // и наверное разбирать их имеет смысл даже до определения есть ли в очереди команды
//    qDebug() << "Got data when we do not wait it";
//    return;
//  }

  this->buffer.append(data); // добавляем данные в буффер

  while (this->buffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {

    if (this->commandsList.isEmpty()) {
      return;
    }

    CCommandProcessor::SAnswerDescr *answerDescr = &(this->commandsList[0]);
    const CCommandBuffer::STextParsingDesc *descr = answerDescr->m_answerDescr;

    qDebug() << "Have completed line. Parse";
    qDebug() << "Line: " << this->buffer.getLine();

    // TODO сделать рефакторинг по умнее раскидать все
    bool hasAnswer = !answerDescr->answer.isEmpty();
    bool hasStatus = !answerDescr->m_waitResult;
    bool needAnswer = descr && answerDescr->answer.isEmpty();
    qDebug() << "Need answer: " << needAnswer;

    if (descr) {

      if (this->buffer.parse(*descr) == CCommandBuffer::PARSE_OK) {
        qDebug() << "Parse ok!";
        answerDescr->answer.append(this->buffer.getLine());
        hasAnswer = true;
      }

    } else {
      // не ожидаем ответа
      hasAnswer = true;
    } // if (descr)

    // проверка на получение всех ответов на команду
    if (hasAnswer && hasStatus) {
      this->gotFullAnswer();
    }

    if (this->buffer.parse(descrOk) == CCommandBuffer::PARSE_OK) {
      hasStatus = true;
      this->gotFullAnswer();
    }

    this->buffer.releaseLine();

  } // while

  qDebug() << "No more line data";
}

void CCommandProcessor::slotTimeout()
{
  if (this->commandsList.isEmpty()) {
    // список команд пуст, нечего ожидать
    return;
  }

  // если истек таймаут ожидания ответа на команду, сбрасываем буффер и удалчяем эту команду из списка
  CCommandProcessor::SAnswerDescr answerDescr = this->commandsList[0];
  quint32 currentTime = QDateTime::currentMSecsSinceEpoch();

  if (answerDescr.m_timeout + answerDescr.timeSend < currentTime) {
    qDebug() << "Error timeout!!!";
    this->removeFirstCommand();
  }
}
