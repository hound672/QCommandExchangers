#ifndef ANSWERBUFFER_H
#define ANSWERBUFFER_H


#include "CommandBuffer.h"

class CAnswerBuffer : public CCommandBuffer
{
public:
  CAnswerBuffer();

  void setCmdId(quint32 cmdId) {this->cmdId = cmdId;}
  quint32 getCmdId() const {return this->cmdId;}
  void setResultStatus(quint32 resultStatus) {this->resultStatus = resultStatus;}
  quint32 getResultStatus() const {return this->resultStatus;}

private:
  quint32 cmdId;
  quint32 resultStatus;
};

#endif // ANSWERBUFFER_H
