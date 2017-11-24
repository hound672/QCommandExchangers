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

  enum ETypeCommand {
    T_CMD_REQUEST = 0, // запрос параметров
    T_CMD_EXEC, // применение команды
    T_CMD_SET, // изменение параметров
    T_CMD_TERMINAL, // команда отправленная из окна терминала
    T_CMD_INTERNAL_REQUEST, // внутренник формат команд, запрос параметров ($LD? - пример)
    T_CMD_INTERNAL_SET, // // внутренник формат команд, изменение параметров
    T_CMD_INTERNAL_SET_BINDATA // внутренник формат команд, изменение параметров + бинарные данные
  };

  CAbstractExchanger(QObject *parent = 0);

  virtual void connectDevice() = 0; // абстрактный метод для подключения к устройству
  virtual void sendData(const QByteArray &cmdToSend, bool waitAnswer = false) = 0; // абстрактный метод для отправки команды
  void sendCommand(const QString &cmdName, ETypeCommand cmdType = T_CMD_REQUEST,
                   const QString &cmdParams = QString(),
                   const QByteArray &cmdBinData = QByteArray());
  void sendCommand(const QByteArray &cmdToSend, const CCommandProcessor::SAnswerDescr &answerDescr);
  void setTrackerPassword(const QString password);

  virtual void setupParams(const QString &portName) {}

// ************** PUBLIC **************

private:
  QString trackerPassword;
  CCommandProcessor *commandProcessor;

// ************** PRIVATE **************

protected:
  void gotIncomingData(const QByteArray &answer);

// ************** PROTECTED **************

signals:
  void signalConnectionResult(bool result);
  void signalDisconnected();
  void signalGotAnswer(const QByteArray &answer);

// ************** SIGNALS **************

private slots:
  void slotGotAnswer(const CAnswerBuffer &answer);
};

#endif // ABSTRACTEXCHANGER_H
