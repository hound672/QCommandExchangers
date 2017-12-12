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
  mWaitResult(waitResult),
  mTimeout(timeout),
  mTimeSend(QDateTime::currentMSecsSinceEpoch()),
  mState(0),
  mAnswerDescr(answerDescr),
  mDescrOk(descrOk),
  mAnswer(cmdId)
{
  if (answerDescr) mState |= ST_A_WAIT_ANSWER;
  if (waitResult) mState |= ST_A_WAIT_RESULT;
}


CCommandProcessor::CCommandProcessor(TAnswersList *unexpectedAnswers, QObject *parent) : QObject(parent)
{
  connect(&mTimer, SIGNAL(timeout()),
          this, SLOT(slotTimeout()));

  if (unexpectedAnswers) {
    mUnexpectedList = *unexpectedAnswers;
  }

  mTimer.start(CCommandProcessor::TIMEOUT);
}

/**
  * @brief  Добавляет в очередь ожидание ответа на команду
  * @param
  * @retval
  */
void CCommandProcessor::addAnswerWait(const CCommandProcessor::SAnswerDescr &answerDescr)
{
  this->mCommandsList.append(answerDescr);
}

/**
  * @brief  Возвращает true если список на очедеь ожидания ответов на команду пустой
  * @param
  * @retval true: если больше не ждем ответов на команду
  */
bool CCommandProcessor::isEmpty()
{
  return mCommandsList.isEmpty();
}

/**
  * @brief  Получен полный ответ на команду
  * @param
  * @retval
  */
void CCommandProcessor::gotFullAnswer()
{
  if (this->mCommandsList.isEmpty()) {
    qDebug() << "Call gotFullAnswer when commandsList is empty!";
    return;
  }

  CAnswerBuffer answer = mCommandsList.first().mAnswer;

  this->removeFirstCommand();
  emit this->signalGotAnswer(answer);
}

/**
  * @brief  Проверяет наличие в буфере ответа на не ожидаемую команду
  * @param
  * @retval
  */
void CCommandProcessor::checkUnexpected()
{
  foreach (SAnswerDescr answerDescr, this->mUnexpectedList) {
    const CCommandBuffer::STextParsingDesc *descr = answerDescr.mAnswerDescr;

    if (descr && this->mBuffer.parse(*descr) == CCommandBuffer::PARSE_OK) {
      CAnswerBuffer answer = answerDescr.mAnswer;
      answer.append(this->mBuffer.getLine(), *descr);
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
  if (!this->mCommandsList.isEmpty()) {
    this->mCommandsList.removeFirst();
  }
}

/**
  * @brief  Слот для обработки входящих данных
  * @param
  * @retval
  */
void CCommandProcessor::slotIncomingData(const QByteArray &data)
{
  this->mBuffer.append(data); // добавляем данные в буффер

  while (this->mBuffer.checkLine() == CCommandBuffer::LINE_COMPELETED) {

    if (mBuffer.getLine().isEmpty()) {
      mBuffer.releaseLine();
      continue;
    }

    if (this->mCommandsList.isEmpty()) {
      this->checkUnexpected();
      this->mBuffer.releaseLine();
      continue;
    }

    CCommandProcessor::SAnswerDescr *answerDescr = &(this->mCommandsList[0]);
    const CCommandBuffer::STextParsingDesc *descr = answerDescr->mAnswerDescr;

    if (descr && this->mBuffer.parse(*descr) == CCommandBuffer::PARSE_OK) {
      // ожидали ответ и получили его
      answerDescr->mAnswer.append(this->mBuffer.getLine(), *descr);
      answerDescr->mState ^= (answerDescr->mState & EStateAnswer::ST_A_WAIT_ANSWER);
    }

    if (answerDescr->mWaitResult && answerDescr->mState & EStateAnswer::ST_A_WAIT_RESULT) {
      // ожидаем ответ о выполнении команды
      const CCommandBuffer::STextParsingDesc *descrForOk;
      CCommandBuffer::EResultParse res;

      descrForOk = answerDescr->mDescrOk ? answerDescr->mDescrOk : &descrOk;

      if ((res = this->mBuffer.parse(*descrForOk)) == CCommandBuffer::PARSE_OK) {

        // получили статус выполнения, который ждали
        answerDescr->mAnswer.setResultCode(0);

      } else if ((res = this->mBuffer.parse(descrErr)) == CCommandBuffer::PARSE_OK) {

        answerDescr->mAnswer.setResultCode(this->mBuffer.getParamInt(0));
        answerDescr->mAnswer.setResultStatus(CAnswerBuffer::EResultStatus::RS_ERROR);

      }

      if (res == CCommandBuffer::PARSE_OK) {
        answerDescr->mState ^= EStateAnswer::ST_A_WAIT_RESULT;
      }
    }

    if (answerDescr->mState == EStateAnswer::ST_A_DONE) {
      this->gotFullAnswer();
    }

    this->mBuffer.releaseLine();

  } // while
}

void CCommandProcessor::slotTimeout()
{
  if (this->mCommandsList.isEmpty()) {
    // список команд пуст, нечего ожидать
    return;
  }

  // если истек таймаут ожидания ответа на команду, сбрасываем буффер и удалчяем эту команду из списка
  CCommandProcessor::SAnswerDescr *answerDescr = &(this->mCommandsList[0]);
  quint32 currentTime = QDateTime::currentMSecsSinceEpoch();

  if (answerDescr->mTimeout + answerDescr->mTimeSend < currentTime) {
    answerDescr->mAnswer.setResultStatus(CAnswerBuffer::EResultStatus::RS_TIMEOUT);
    qDebug() << "Error timeout: " << answerDescr->mAnswer.getResultStatus();
    this->gotFullAnswer();
  }
}

