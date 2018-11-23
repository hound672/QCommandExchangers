#ifndef ABSTRACTEXCHANGER_H
#define ABSTRACTEXCHANGER_H

#include <QObject>

#include "CommandBuffer.h"
#include "QAnswerBuffer.h"
#include "CommandProcessor.h"

// ======================================================================

// интерфейс для логирования запросов - ответов устройства
class ICommandLogger
{
public:
  virtual void WriteRequest(const CCommandProcessor::SAnswerDescr &cmdAnswerDescr, const QByteArray &cmdText) = 0;
  virtual void WriteAnswer(const QAnswerBuffer &answer) = 0;
  virtual void WriteRawData(const QByteArray &data) = 0;
};

// ======================================================================

class QAbstractExchanger: public QObject
{
  Q_OBJECT
  
public:
  typedef QList<QAnswerBuffer> TAnswersList;

// ======================================================================
	
public:
  QAbstractExchanger(QObject *parent = NULL);
  QAbstractExchanger(CCommandProcessor::TAnswersDescrList *unexpectedAnswers, QObject *parent = NULL);
  QAbstractExchanger(ICommandLogger *commandLogger, QObject *parent = NULL);
  QAbstractExchanger(ICommandLogger *commandLogger, 
                     CCommandProcessor::TAnswersDescrList *unexpectedAnswers, QObject *parent = NULL);

// ======================================================================
	
public:
  virtual void ConnectDevice() = 0;			// метод для подключения к устройству
	virtual void DisconnectDevice() = 0;	// метод для отключения от устройства
  virtual void SendData(const QByteArray &cmdToSend) = 0; // виртуальный метод для отправки команды
	// ======================================================================
  void SendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr);
  bool IsAnswersListEmpty();
  void Clear();
	void SetCommandLogger(ICommandLogger *commandLogger);
	
// ======================================================================

protected:
  CCommandProcessor *mCommandProcessor;
  TAnswersList mAnswersList;
  ICommandLogger *mCommandLogger;
	
// ======================================================================

protected:
  void MakeSignalSlots();
  void GotIncomingData(const QByteArray &answer); // вызывается наследником при получении данных от устройства
	
// ======================================================================

signals:
  void SignalConnectionResult(bool result); // статус подключении к устройству (true - успешно)
  void SignalDisconnected(); // обрыв связи с устройством
  void SignalGotAnswer(const QAnswerBuffer &answer); // получен ответ на команду
  void SignalCommandEnd(const TAnswersList &answersList); // все команды из очереди выполнены
  void SignalGotRawData(const QByteArray &data); // "сырые" данные от устройства (после разбора наследником)
	
// ======================================================================

protected slots:
  void SlotGotAnswer(const QAnswerBuffer &answer);
};

#endif // ABSTRACTEXCHANGER_H
