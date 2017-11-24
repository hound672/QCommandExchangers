#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

#include "CommandBuffer.h"
#include "AnswerBuffer.h"

class CCommandProcessor : public QObject
{
  Q_OBJECT

public:

  /*********************************************
   *  Структура описывающяя условия ответа на команду
   *  Таймаут ее ожидания, ожидание статуса выполнения и т.п
   *********************************************/
  struct SAnswerDescr {
    quint32 m_cmdId; // произвольный id команды
    const CCommandBuffer::STextParsingDesc *m_answerDescr; // описание для парсинга ответа команды, если NULL то ответа не ожидаем
    bool m_waitResult; // флаг ожидания результата выполнения команды (обычно OK/ERROR после текста ответа)
    quint32 m_timeout; // таумайт для ожидания выполнения команды

    CAnswerBuffer answer; // буфер под ответ на команду
    quint32 timeSend; // время когда была отправленна команда

    SAnswerDescr(quint32 cmdId, const CCommandBuffer::STextParsingDesc *answerDescr = NULL,
                 bool waitResult = false, quint32 timeout = 1000) :
      m_cmdId(cmdId),
      m_answerDescr(answerDescr),
      m_waitResult(waitResult),
      m_timeout(timeout),
      timeSend(QDateTime::currentMSecsSinceEpoch())
    {answer.setCmdId(cmdId);}

  };


public:

  explicit CCommandProcessor(QObject *parent = 0);
  void sendCommand(const QByteArray &command, const SAnswerDescr &answerDescr);
  void addAnswerWait(const SAnswerDescr &answerDescr);

private:
  void gotFullAnswer();

  static const int TIMEOUT = 100;
  QTimer timer;
  QList<SAnswerDescr> commandsList; // список ожидаемых ответов на команду
  CCommandBuffer buffer;
  void removeFirstCommand();

signals:
  void signalGotAnswer(const CAnswerBuffer &answerBuffer);

public slots:
  void slotIncomingData(const QByteArray &data);

private slots:
  void slotTimeout();
};

#endif // COMMANDPROCESSOR_H
