#include <QDebug>
#include "QCommandExchangersGlobal.h"

#include "AbstractExchanger.h"


// ---------------------------------------------------------------

CAbstractExchanger::CAbstractExchanger(CCommandProcessor::TAnswersList *unexpectedAnswers, QObject *parent) :
  QObject(parent)
{
  this->mCommandProcessor = new CCommandProcessor(unexpectedAnswers, false, parent);
  this->makeSignalSlots();
  
  qRegisterMetaType<TAnswersList>("TAnswersList");
}

// ======================================================================

// ======================================================================
//  public                       
// ======================================================================


/**
  * @brief  Отправляет команду на устройство и добавлет в очередь ожидание на ее ответ
  * @param
  * @retval
  */
void CAbstractExchanger::sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr)
{
  this->sendData(cmdToSend, false);
  this->mCommandProcessor->addAnswerWait(answerDescr);
}

// ======================================================================

/**
  * @brief  Возвращает true если список на очедеь ожидания ответов на команду пустой
  * @param
  * @retval
  */
bool CAbstractExchanger::isAnswersListEmpty()
{
  return mCommandProcessor->isEmpty();
}

// ======================================================================

void CAbstractExchanger::clear()
{
  mCommandProcessor->clear();
}

// ---------------------------------------------------------------

// ======================================================================
//  private                       
// ======================================================================

/**
  * @brief  Создаем связки сигналов слотов
  * @param
  * @retval
  */
void CAbstractExchanger::makeSignalSlots()
{
  connect(this->mCommandProcessor, SIGNAL(signalGotAnswer(const CAnswerBuffer&)),
          this, SLOT(slotGotAnswer(const CAnswerBuffer&)));
}

// ------------------------------------------------

// ======================================================================
//  protected                       
// ======================================================================

/**
  * @brief  Получили данные от устройства
  * @param
  * @retval
  */
void CAbstractExchanger::gotIncomingData(const QByteArray &answer)
{
  this->mCommandProcessor->slotIncomingData(answer);
  emit this->signalGotRawData(answer);
}

// ======================================================================

// ======================================================================
//  private slots                       
// ======================================================================

/**
  * @brief  Слот для обработки получения результата выпонения команды
  * @param  
  * @retval 
  */
void CAbstractExchanger::slotGotAnswer(const CAnswerBuffer &answer)
{
  emit this->signalGotAnswer(answer);
  mAnswersList.append(answer);
  
  if (this->isAnswersListEmpty()) {
    emit this->signalCommandEnd(mAnswersList);
    mAnswersList.clear();
  }
}

// ---------------------------------------------------------------
