#ifndef ANSWERBUFFER_H
#define ANSWERBUFFER_H

#include <QList>

#include "CommandBuffer.h"

class CAnswerBuffer : public QList<CCommandBuffer>
{

public:
  enum EResultStatus {
    RS_OK = 0,  // успешное выполнение команды
    RS_ERROR,   // ошибка выполнения команды, подробности должны быть в поле resultCode
    RS_TIMEOUT, // ошибка таймаута ожидания ответа на команду
    RS_UNKNOWN, // неизвестное состояние
  };

public:
  CAnswerBuffer(qint32 mCmdId = 0);
  void append(const QByteArray &dataToAdd, const CCommandBuffer::STextParsingDesc &parseDescr);
  CCommandBuffer first() const;
  CCommandBuffer last() const;

  void setCmdId(quint32 cmdId) {mCmdId = cmdId;}
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
