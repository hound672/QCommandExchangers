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

private:

  /*********************************************
   *  Состояния для получения ответа на команду
   *  Битовая маска
   *********************************************/
  enum EStateAnswer {
    ST_A_WAIT_ANSWER = 0x01,
    ST_A_WAIT_RESULT = 0x02,
    ST_A_DONE = 0x00,
  };

// ************** PRIVATE **************

public:

  typedef QList<CCommandBuffer::STextParsingDesc> TListTextParsingDesc;

  /*********************************************
   *  Структура описывающяя условия ответа на команду
   *  Таймаут ее ожидания, ожидание статуса выполнения и т.п
   *********************************************/
  struct SAnswerDescr {
    quint32 m_cmdId; // произвольный id команды
    const CCommandBuffer::STextParsingDesc *m_answerDescr; // описание для парсинга ответа команды,
                                                           // если NULL то ответа не ожидаем
    const CCommandBuffer::STextParsingDesc *m_descrOk;     // описание для парсинга результата выполнения команды
                                                           // если NULL то используем стандартное
    bool m_waitResult; // флаг ожидания результата выполнения команды (обычно OK/ERROR после текста ответа)
    quint32 m_timeout; // таумайт для ожидания выполнения команды

    CAnswerBuffer answer; // буфер под ответ на команду
    quint32 timeSend; // время когда была отправленна команда
    quint8 state;

    SAnswerDescr(quint32 cmdId, const CCommandBuffer::STextParsingDesc *answerDescr = NULL,
                 bool waitResult = false, quint32 timeout = 1000,
                 const CCommandBuffer::STextParsingDesc *descrOk = NULL);

  };

// ************** PUBLIC **************

public:

  explicit CCommandProcessor(QObject *parent = 0);
  void addAnswerWait(const SAnswerDescr &answerDescr);

// ************** PUBLIC **************

private:
  void gotFullAnswer();

  static const int TIMEOUT = 100; // время интервала тика для таймера
  QTimer timer;
  QList<SAnswerDescr> commandsList; // список ожидаемых ответов на команду
  CCommandBuffer buffer;
  void removeFirstCommand();

// ************** PRIVATE **************

signals:
  void signalGotAnswer(const CAnswerBuffer &answerBuffer); // Сигнал о получении ответа на команду

// ************** SIGNALS **************

public slots:
  void slotIncomingData(const QByteArray &data);

// *********** PUBLIC SLOTS ************

private slots:
  void slotTimeout();

// *********** PRIVATE SLOTS ***********

};

#endif // COMMANDPROCESSOR_H
