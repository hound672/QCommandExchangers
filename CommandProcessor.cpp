#include <QDebug>

#include "CommandProcessor.h"

// ---------------------------------------------------------------


static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};
static const CCommandBuffer::STextParsingDesc descrErr = {"$ERR", ','};

/* ***************** BEGIN *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

/**
  * @brief  Констурктор для структуры с описанием ответа на команду
  * @param  cmdId: Некий идентификатор команды
  * @param  answerDescr: структура с описанием разбора ответа
  * @param  waitResult: флаг ожидания ответа на выолнение команды, обычно OK/ERROR
  * @param  timeout: таймаут в мс ожидания ответа на команду
  * @retval
  */
CCommandProcessor::SAnswerDescr::SAnswerDescr(quint32 cmdId,
                                              const CCommandBuffer::STextParsingDesc *answerDescr,
                                              bool waitResult, quint32 timeout,
                                              const CCommandBuffer::STextParsingDesc *descrOk) :
  m_waitResult(waitResult),
  m_timeout(timeout),
  timeSend(QDateTime::currentMSecsSinceEpoch()),
  state(0),
  m_answerDescr(answerDescr),
  m_descrOk(descrOk),
  answer(cmdId)
{
  if (answerDescr) state |= ST_A_WAIT_ANSWER;
  if (waitResult) state |= ST_A_WAIT_RESULT;
}


CCommandProcessor::CCommandProcessor(QObject *parent) : QObject(parent)
{
  connect(&(this->timer), SIGNAL(timeout()),
          this, SLOT(slotTimeout()));

  this->timer.start(CCommandProcessor::TIMEOUT);
}

/**
  * @brief  Конструктор
  * @param
  * @retval
  */
CCommandProcessor::CCommandProcessor(const CCommandProcessor::TAnswersList &answersList, QObject *parent) :
  CCommandProcessor(parent)
{
  this->unexpectedList = answersList;
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
  if (this->commandsList.isEmpty()) {
    qDebug() << "Call gotFullAnswer when commandsList is empty!";
    return;
  }

  CCommandProcessor::SAnswerDescr answerDescr = this->commandsList[0];

  emit this->signalGotAnswer(answerDescr.answer);
  this->removeFirstCommand();
}

/**
  * @brief  Проверяет наличие в буфере ответа на не ожидаемую команду
  * @param
  * @retval
  */
void CCommandProcessor::checkUnexpected()
{
  foreach (SAnswerDescr answerDescr, this->unexpectedList) {
    const CCommandBuffer::STextParsingDesc *descr = answerDescr.m_answerDescr;

    if (descr && this->buffer.parse(*descr) == CCommandBuffer::PARSE_OK) {
      CAnswerBuffer answer = answerDescr.answer;
      answer.append(this->buffer.getLine(), *descr);
      emit this->signalGotAnswer(answer);
    }
  }
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
  this->buffer.append(data); // добавляем данные в буффер

  while (this->buffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {

    if (this->commandsList.isEmpty()) {
      this->checkUnexpected();
      this->buffer.releaseLine();
      continue;
    }

    CCommandProcessor::SAnswerDescr *answerDescr = &(this->commandsList[0]);
    const CCommandBuffer::STextParsingDesc *descr = answerDescr->m_answerDescr;

    if (descr && this->buffer.parse(*descr) == CCommandBuffer::PARSE_OK) {
      // ожидали ответ и получили его
      answerDescr->answer.append(this->buffer.getLine(), *descr);
      answerDescr->state ^= (answerDescr->state & EStateAnswer::ST_A_WAIT_ANSWER);
    }

    if (answerDescr->m_waitResult && answerDescr->state & EStateAnswer::ST_A_WAIT_RESULT) {
      // ожидаем ответ о выполнении команды
      const CCommandBuffer::STextParsingDesc *descrForOk;
      CCommandBuffer::EResultParse res;

      descrForOk = answerDescr->m_descrOk ? answerDescr->m_descrOk : &descrOk;

      if ((res = this->buffer.parse(*descrForOk)) == CCommandBuffer::PARSE_OK) {

        // получили статус выполнения, который ждали
        answerDescr->answer.setResultCode(0);

      } else if ((res = this->buffer.parse(descrErr)) == CCommandBuffer::PARSE_OK) {

        answerDescr->answer.setResultCode(this->buffer.getParamInt(0));
        answerDescr->answer.setResultStatus(CAnswerBuffer::EResultStatus::RS_ERROR);

      }

      if (res == CCommandBuffer::PARSE_OK) {
        answerDescr->state ^= EStateAnswer::ST_A_WAIT_RESULT;
      }
    }

    if (answerDescr->state == EStateAnswer::ST_A_DONE) {
      this->gotFullAnswer();
    }

    this->buffer.releaseLine();

  } // while
}

void CCommandProcessor::slotTimeout()
{
  if (this->commandsList.isEmpty()) {
    // список команд пуст, нечего ожидать
    return;
  }

  // если истек таймаут ожидания ответа на команду, сбрасываем буффер и удалчяем эту команду из списка
  CCommandProcessor::SAnswerDescr *answerDescr = &(this->commandsList[0]);
  quint32 currentTime = QDateTime::currentMSecsSinceEpoch();

  if (answerDescr->m_timeout + answerDescr->timeSend < currentTime) {
    answerDescr->answer.setResultStatus(CAnswerBuffer::EResultStatus::RS_TIMEOUT);
    qDebug() << "Error timeout: " << answerDescr->answer.getResultStatus();
    this->gotFullAnswer();
  }
}

