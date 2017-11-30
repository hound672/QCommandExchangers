#ifndef ANSWERBUFFER_H
#define ANSWERBUFFER_H


#include "CommandBuffer.h"

class CAnswerBuffer : public CCommandBuffer
{

public:
  enum EResultStatus {
    RS_OK = 0,  // успешное выполнение команды
    RS_ERROR,   // ошибка выполнения команды, подробности должны быть в поле resultCode
    RS_TIMEOUT, // ошибка таймаута ожидания ответа на команду
    RS_UNKNOWN, // неизвестное состояние
  };

public:
  CAnswerBuffer();

  void setCmdId(quint32 cmdId) {this->cmdId = cmdId;}
  quint32 getCmdId() const {return this->cmdId;}
  void setResultCode(quint32 resultStatus) {this->resultCode = resultStatus;}
  quint32 getResultCode() const {return this->resultCode;}
  void setResultStatus(EResultStatus state) {this->resultStatus = state;}
  EResultStatus getResultStatus() const {return this->resultStatus;}

private:
  quint32 cmdId; // идентификатор команды
  quint32 resultCode; // код результата выполнения команды
  EResultStatus resultStatus; // Состояние выполнения команды
};

#endif // ANSWERBUFFER_H
