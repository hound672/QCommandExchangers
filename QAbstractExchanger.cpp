#include <QDebug>
#include "QCommandExchangersGlobal.h"

#include "QAbstractExchanger.h"


// ======================================================================

QAbstractExchanger::QAbstractExchanger(QObject *parent):
  QAbstractExchanger(NULL, NULL, parent) {}

QAbstractExchanger::QAbstractExchanger(CCommandProcessor::TAnswersDescrList *unexpectedAnswers, QObject *parent) :
  QAbstractExchanger(NULL, unexpectedAnswers, parent) {}

QAbstractExchanger::QAbstractExchanger(ICommandLogger *commandLogger, QObject *parent):
  QAbstractExchanger(commandLogger, NULL, parent) {}

QAbstractExchanger::QAbstractExchanger(ICommandLogger *commandLogger, 
                                       CCommandProcessor::TAnswersDescrList *unexpectedAnswers, QObject *parent):
  QObject(parent),
  mCommandLogger(commandLogger)
{
  mCommandProcessor = new CCommandProcessor(unexpectedAnswers, false, parent);
  makeSignalSlots();
  
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
void QAbstractExchanger::sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr)
{
	QByteArray data = cmdToSend;
	// add \r\n at the end of data
  data.append("\r\n");

	this->sendData(data);
  this->mCommandProcessor->addAnswerWait(answerDescr);
  
  if (mCommandLogger) {
    mCommandLogger->WriteRequest(answerDescr, cmdToSend);
  }
}

// ======================================================================

/**
  * @brief  Возвращает true если список на очедеь ожидания ответов на команду пустой
  * @param
  * @retval
  */
bool QAbstractExchanger::isAnswersListEmpty()
{
  return mCommandProcessor->isEmpty();
}

// ======================================================================

void QAbstractExchanger::clear()
{
	mCommandProcessor->clear();
}

// ======================================================================

/**
	* @brief  Меняет логгер для вывода информации об обмене
	* @param  
	* @retval 
	*/
void QAbstractExchanger::setCommandLogger(ICommandLogger *commandLogger)
{
	mCommandLogger = commandLogger;
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
void QAbstractExchanger::makeSignalSlots()
{
  connect(this->mCommandProcessor, SIGNAL(SignalGotAnswer(const QAnswerBuffer&)),
          this, SLOT(SlotGotAnswer(const QAnswerBuffer&)));
}

// ======================================================================

// ======================================================================
//  protected                       
// ======================================================================

/**
  * @brief  Получили данные от устройства
  * @param
  * @retval
  */
void QAbstractExchanger::gotIncomingData(const QByteArray &answer)
{
  mCommandProcessor->SlotIncomingData(answer);
  emit this->SignalGotRawData(answer);
  
  if (mCommandLogger) {
    mCommandLogger->WriteRawData(answer);
  }
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
void QAbstractExchanger::SlotGotAnswer(const QAnswerBuffer &answer)
{
  emit SignalGotAnswer(answer);
  mAnswersList.append(answer);
  
  if (this->isAnswersListEmpty()) {
    emit SignalCommandEnd(mAnswersList);
    mAnswersList.clear();
  }
  
  if (mCommandLogger) {
    mCommandLogger->WriteAnswer(answer);
  }
}

// ---------------------------------------------------------------
