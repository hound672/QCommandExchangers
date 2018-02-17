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

  /*********************************************
   *  Структура описывающяя условия ответа на команду
   *  Таймаут ее ожидания, ожидание статуса выполнения и т.п
   *********************************************/
  struct SAnswerDescr {
    bool mWaitResult; // флаг ожидания результата выполнения команды (обычно OK/ERROR после текста ответа)

    quint64 mTimeout; // таумайт для ожидания выполнения команды
    quint8 mState;

    const CCommandBuffer::STextParsingDesc *mAnswerDescr; // описание для парсинга ответа команды,
                                                           // если NULL то ответа не ожидаем
    const CCommandBuffer::STextParsingDesc *mDescrOk;     // описание для парсинга результата выполнения команды
                                                           // если NULL то используем стандартное
    CAnswerBuffer mAnswer; // буфер под ответ на команду

    SAnswerDescr() {}
    SAnswerDescr(quint32 cmdId, const CCommandBuffer::STextParsingDesc *answerDescr = NULL,
                 bool waitResult = false, quint32 timeout = 1000,
                 const CCommandBuffer::STextParsingDesc *descrOk = NULL);

  };

public:
  typedef QList<SAnswerDescr> TAnswersList; // тип содержащий список структур SAnswerDescr

// ************** PUBLIC **************

public:

  explicit CCommandProcessor(const TAnswersList *unexpectedAnswers, bool onlyIn = false, QObject *parent = 0);
  void addAnswerWait(const SAnswerDescr &answerDescr);
  bool isEmpty();
  void clear();

// ************** PUBLIC **************

private:
  void gotFullAnswer();
  void checkUnexpected();
  void removeFirstCommand();

  static const int TIMEOUT = 100; // время интервала тика для таймера
  bool mOnlyIn;
  QTimer mTimer;
  TAnswersList mCommandsList; // список ожидаемых ответов на команду
  TAnswersList mUnexpectedList; // список описаний для не ожидаемых ответов
  CCommandBuffer mBuffer;

// ************** PRIVATE **************

signals:
  void signalGotAnswer(const CAnswerBuffer &answerBuffer); // Сигнал о получении ответа на команду
  void signalUnknownCmd();

// ************** SIGNALS **************

public slots:
  void slotIncomingData(const QByteArray &data);

// *********** PUBLIC SLOTS ************

private slots:
  void slotTimeout();

// *********** PRIVATE SLOTS ***********

};

#endif // COMMANDPROCESSOR_H
