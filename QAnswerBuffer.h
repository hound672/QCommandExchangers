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

  void SetCmdId(quint32 cmdId) {mCmdId = cmdId;}
  quint32 GetCmdId() const {return mCmdId;}

  void SetResultCode(quint32 resultStatus) {mResultCode = resultStatus;}
  quint32 GetResultCode() const {return mResultCode;}

  void SetResultStatus(EResultStatus state) {mResultStatus = state;}
  EResultStatus GetResultStatus() const {return mResultStatus;}

private:
  quint32 mCmdId; // идентификатор команды
  quint32 mResultCode; // код результата выполнения команды
  EResultStatus mResultStatus; // Состояние выполнения команды
};

#endif // ANSWERBUFFER_H
