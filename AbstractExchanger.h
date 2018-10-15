#ifndef ABSTRACTEXCHANGER_H
#define ABSTRACTEXCHANGER_H

#include <QObject>

#include "CommandBuffer.h"
#include "AnswerBuffer.h"
#include "CommandProcessor.h"

class CAbstractExchanger: public QObject
{
  Q_OBJECT
  
public:
  typedef QList<CAnswerBuffer> TAnswersList;
  
public:
  CAbstractExchanger(CCommandProcessor::TAnswersList *unexpectedAnswers, QObject *parent = 0);

  virtual void connectDevice() = 0; // вирутальный метод для подключения к устройству
  virtual void disconnectDevice() = 0;
  virtual void sendData(const QByteArray &cmdToSend, bool waitAnswer = false) = 0; // виртуальный метод для отправки команды
  void sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr);
  bool isAnswersListEmpty();
  void clear();

private:
  CCommandProcessor *mCommandProcessor;
  TAnswersList mAnswersList;

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
