#ifndef ABSTRACTEXCHANGER_H
#define ABSTRACTEXCHANGER_H

#include <QObject>

#include "CommandBuffer.h"
#include "AnswerBuffer.h"
#include "CommandProcessor.h"

// ======================================================================

// интерфейс для логирования запросов - ответов устройства
class ICommandLogger
{
public:
  virtual void writeRequest(const CCommandProcessor::SAnswerDescr &cmdAnswerDescr, const QByteArray &cmdText) = 0;
  virtual void writeAnswer(const CAnswerBuffer &answer) = 0;
  virtual void writeRawData(const QByteArray &data) = 0;
};

// ======================================================================

class CAbstractExchanger: public QObject
{
  Q_OBJECT
  
public:
  typedef QList<CAnswerBuffer> TAnswersList;
  
public:
  
  CAbstractExchanger(QObject *parent = NULL);
  CAbstractExchanger(CCommandProcessor::TAnswersDescrList *unexpectedAnswers, QObject *parent = NULL);
  CAbstractExchanger(ICommandLogger *commandLogger, QObject *parent = NULL);
  CAbstractExchanger(ICommandLogger *commandLogger, 
                     CCommandProcessor::TAnswersDescrList *unexpectedAnswers, QObject *parent = NULL);

  
public:
  virtual void connectDevice() = 0; // вирутальный метод для подключения к устройству
  virtual void disconnectDevice() = 0;
  virtual void sendData(const QByteArray &cmdToSend, bool waitAnswer = false) = 0; // виртуальный метод для отправки команды
  void sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr);
  bool isAnswersListEmpty();
  void clear();

private:
  CCommandProcessor *mCommandProcessor;
  TAnswersList mAnswersList;
  ICommandLogger *mCommandLogger;

private:
  void makeSignalSlots();

protected:
  void gotIncomingData(const QByteArray &answer); // вызывается наследником при получении данных от устройства

signals:
  void signalConnectionResult(bool result); // статус подключении к устройству (true - успешно)
  void signalDisconnected(); // обрыв связи с устройством
  void signalGotAnswer(const CAnswerBuffer &answer); // получен ответ на команду
  void signalCommandEnd(const TAnswersList &answersList); // все команды из очереди выполнены
  void signalGotRawData(const QByteArray &data); // "сырые" данные от устройства (после разбора наследником)

private slots:
  void slotGotAnswer(const CAnswerBuffer &answer);
};

#endif // ABSTRACTEXCHANGER_H
