#ifndef ANSWERBUFFER_H
#define ANSWERBUFFER_H

#include <QList>

#include "CommandBuffer.h"

class QAnswerBuffer : public QList<CCommandBuffer>
{

public:
  
  enum EResultStatus {
    resOk = 0,  // успешное выполнение команды
    resError,   // ошибка выполнения команды, подробности должны быть в поле resultCode
    resTimeout, // ошибка таймаута ожидания ответа на команду
    resUnknown, // неизвестное состояние
  };

public:
  QAnswerBuffer(qint32 mCmdId = 0);
  void append(const QByteArray &dataToAdd, const CCommandBuffer::STextParsingDesc &parseDescr);
  CCommandBuffer first() const;
  CCommandBuffer last() const;

  void getCmdId(quint32 cmdId) {mCmdId = cmdId;}
  quint32 getCmdId() const {return mCmdId;}

  void setResultCode(quint32 resultStatus) {mResultCode = resultStatus;}
  quint32 getResultCode() const {return mResultCode;}

  void setResultStatus(EResultStatus state) {mResultStatus = state;}
  EResultStatus getResultStatus() const {return mResultStatus;}

private:
  quint32 mCmdId; // идентификатор команды
  quint32 mResultCode; // код результата выполнения команды
  EResultStatus mResultStatus; // Состояние выполнения команды
};

#endif // ANSWERBUFFER_H
