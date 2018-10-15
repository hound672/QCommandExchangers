#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QElapsedTimer>

#include "CommandBuffer.h"
#include "AnswerBuffer.h"

class CCommandProcessor : public QObject
{
  Q_OBJECT

private:

  // Состояния для получения ответа на команду (битовая маска)
  enum EStateAnswer {
    stWaitAnswer = 0x01,
    stWaitResult = 0x02,
    stDone = 0x00,
  };

public:
  
  // Структура описывающяя условия ответа на команду
  // Таймаут ее ожидания, ожидание статуса выполнения и т.п
  struct SAnswerDescr {
    
    bool mWaitResult; // флаг ожидания результата выполнения команды (обычно OK/ERROR после текста ответа)

    quint32 mTimeout; // таумайт для ожидания выполнения команды
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
    
    bool hasExpired() const;

  private:
    QElapsedTimer mTimer;
  };

public:
  typedef QList<SAnswerDescr> TAnswersList; // тип содержащий список структур SAnswerDescr

public:

  explicit CCommandProcessor(const TAnswersList *unexpectedAnswers, bool onlyIn = false, QObject *parent = 0);
  void addAnswerWait(const SAnswerDescr &answerDescr);
  bool isEmpty();
  void clear();

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

signals:
  void signalGotAnswer(const CAnswerBuffer &answerBuffer); // Сигнал о получении ответа на команду
  void signalUnknownCmd();

public slots:
  void slotIncomingData(const QByteArray &data);

private slots:
  void slotTimeout();

};

#endif // COMMANDPROCESSOR_H
