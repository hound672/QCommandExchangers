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

  CAbstractExchanger(QObject *parent = 0);

  virtual void connectDevice() = 0; // абстрактный метод для подключения к устройству
  virtual void sendData(const QByteArray &cmdToSend, bool waitAnswer = false) = 0; // абстрактный метод для отправки команды
  void sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr);

// ************** PUBLIC **************

private:
  CCommandProcessor *commandProcessor;

// ************** PRIVATE **************

protected:
  void gotIncomingData(const QByteArray &answer);

// ************** PROTECTED **************

signals:
  void signalConnectionResult(bool result);
  void signalDisconnected();
  void signalGotAnswer(const CAnswerBuffer &answer);
  void signalGotRawData(const QByteArray &data);
  void signalErrorTimeout(const CAnswerBuffer &answerBuffer);

// ************** SIGNALS **************

private slots:
  void slotGotAnswer(const CAnswerBuffer &answer);
  void slotErrorTimeout(const CAnswerBuffer &answer);
};

#endif // ABSTRACTEXCHANGER_H
