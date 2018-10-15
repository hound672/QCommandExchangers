#include <QDebug>

#include "QCommandExchangersGlobal.h"

#include "CommandProcessor.h"

// ---------------------------------------------------------------
static const CCommandBuffer::STextParsingDesc descrOk = {"$OK", ','};
static const CCommandBuffer::STextParsingDesc descrErr = {"$ERR", ','};
// ======================================================================

// ======================================================================
//  CCommandProcessor::SAnswerDescr                      
// ======================================================================

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
  mState(0),
  mAnswerDescr(answerDescr),
  mDescrOk(descrOk),
  mAnswer(cmdId)
{
  if (answerDescr) mState |= stWaitAnswer;
  if (waitResult) mState |= stWaitResult;
  
  mTimer.start();
}

// ======================================================================

bool CCommandProcessor::SAnswerDescr::hasExpired() const
{
  return mTimer.hasExpired(mTimeout);
}

// ======================================================================

// ======================================================================
//  CCommandProcessor                       
// ======================================================================

// ======================================================================

/**
  * @brief
  * @param  onlyIn: при true работает только для входищих команд
  *                 не включает таймер и генерирует ошибку, если команды не было в списке не ожидаемых команд
  * @retval
  */
CCommandProcessor::CCommandProcessor(const TAnswersDescrList *unexpectedAnswers, bool onlyIn, QObject *parent) :
  QObject(parent),
  mOnlyIn(onlyIn)
{
  connect(&mTimer, SIGNAL(timeout()),
          this, SLOT(slotTimeout()));

  if (unexpectedAnswers) {
    mUnexpectedList = *unexpectedAnswers;
  }

  if (!onlyIn) {
    mTimer.start(CCommandProcessor::TIMEOUT);
  }
}

// ======================================================================

/**
  * @brief  Добавляет в очередь ожидание ответа на команду
  * @param
  * @retval
  */
void CCommandProcessor::addAnswerWait(const CCommandProcessor::SAnswerDescr &answerDescr)
{
  this->mCommandsList.append(answerDescr);
}

// ======================================================================

/**
  * @brief  Возвращает true если список на очедеь ожидания ответов на команду пустой
  * @param
  * @retval true: если больше не ждем ответов на команду
  */
bool CCommandProcessor::isEmpty()
{
  return mCommandsList.isEmpty();
}

// ======================================================================

void CCommandProcessor::clear()
{
  mBuffer.clear();
}

// ======================================================================

/**
  * @brief  Получен полный ответ на команду
  * @param
  * @retval
  */
void CCommandProcessor::gotFullAnswer()
{
  if (this->mCommandsList.isEmpty()) {
    qDebugComExch() << "Call gotFullAnswer when commandsList is empty!";
    return;
  }

  CAnswerBuffer answer = mCommandsList.first().mAnswer;

  this->removeFirstCommand();
  emit this->signalGotAnswer(answer);
}

// ======================================================================

/**
  * @brief  Проверяет наличие в буфере ответа на не ожидаемую команду
  * @param
  * @retval
  */
void CCommandProcessor::checkUnexpected()
{
  foreach (SAnswerDescr answerDescr, this->mUnexpectedList) {
    const CCommandBuffer::STextParsingDesc *descr = answerDescr.mAnswerDescr;

    if (descr && this->mBuffer.parse(*descr) == CCommandBuffer::parseOk) {
      CAnswerBuffer answer = answerDescr.mAnswer;
      answer.append(this->mBuffer.getLine(), *descr);
      mBuffer.releaseLine();
      emit this->signalGotAnswer(answer);
      return;
    }
  }

  if (mOnlyIn) {
    // команда не была найдена в списке не ожидаемых и режим работы только входящие команды
    emit this->signalUnknownCmd();
  }
}

// ======================================================================

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

// ======================================================================

/**
  * @brief  Слот для обработки входящих данных
  * @param
  * @retval
  */
void CCommandProcessor::slotIncomingData(const QByteArray &data)
{
  this->mBuffer.append(data); // добавляем данные в буффер

  while (this->mBuffer.checkLine() == CCommandBuffer::lineCompleted) {

    if (mBuffer.getLine().isEmpty()) {
      mBuffer.releaseLine();
      continue;
    }

    this->checkUnexpected();

    if (this->mCommandsList.isEmpty()) {
      this->mBuffer.releaseLine();
      continue;
    }

    CCommandProcessor::SAnswerDescr *answerDescr = &(this->mCommandsList[0]);
    const CCommandBuffer::STextParsingDesc *descr = answerDescr->mAnswerDescr;

    if (descr && this->mBuffer.parse(*descr) == CCommandBuffer::parseOk) {
      // ожидали ответ и получили его
      answerDescr->mAnswer.append(this->mBuffer.getLine(), *descr);
      answerDescr->mState ^= (answerDescr->mState & EStateAnswer::stWaitAnswer);
    }

    if (answerDescr->mWaitResult && answerDescr->mState & EStateAnswer::stWaitResult) {
      // ожидаем ответ о выполнении команды
      const CCommandBuffer::STextParsingDesc *descrForOk;
      CCommandBuffer::EResultParse res;

      descrForOk = answerDescr->mDescrOk ? answerDescr->mDescrOk : &descrOk;

      if ((res = this->mBuffer.parse(*descrForOk)) == CCommandBuffer::parseOk) {

        // получили статус выполнения, который ждали
        answerDescr->mAnswer.setResultCode(0);

      } else if ((res = this->mBuffer.parse(descrErr)) == CCommandBuffer::parseOk) {

        answerDescr->mAnswer.setResultCode(this->mBuffer.getParamInt(0));
        answerDescr->mAnswer.setResultStatus(CAnswerBuffer::EResultStatus::resError);

      }

      if (res == CCommandBuffer::parseOk) {
        answerDescr->mState ^= EStateAnswer::stWaitResult;
      }
    }

    if (answerDescr->mState == EStateAnswer::stDone) {
      this->gotFullAnswer();
    }

    this->mBuffer.releaseLine();

  } // while
}

// ======================================================================

void CCommandProcessor::slotTimeout()
{
  if (this->mCommandsList.isEmpty()) {
    // список команд пуст, нечего ожидать
    return;
  }

  // если истек таймаут ожидания ответа на команду, сбрасываем буффер и удалчяем эту команду из списка
  CCommandProcessor::SAnswerDescr *answerDescr = &(this->mCommandsList[0]);

  if (answerDescr->hasExpired()) {
    answerDescr->mAnswer.setResultStatus(CAnswerBuffer::EResultStatus::resTimeout);
    qDebugComExch() << "Error timeout: " << answerDescr->mAnswer.getCmdId();
    this->gotFullAnswer();
  }
}

// ======================================================================
