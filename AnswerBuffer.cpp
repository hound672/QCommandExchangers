#include <QDebug>

#include "AnswerBuffer.h"

CAnswerBuffer::CAnswerBuffer(qint32 cmdId, const STextParsingDesc *answerDescr) :
  resultCode(0),
  resultStatus(EResultStatus::RS_OK),
  cmdId(cmdId)
{
  if (answerDescr) {
    qDebug() << "There is answerDescr add it: " << answerDescr->m_prefix;
    this->answerDescr = *(answerDescr);
  } else {
    qDebug() << "There is no answerDescr add it";
  }
}

CCommandBuffer::EResultParse CAnswerBuffer::getParam(quint32 index, QByteArray &data) const
{
  QList<QByteArray> list = this->splitParams(this->answerDescr);
  data = CCommandBuffer::getParam(list, index);
}

/**
  * @brief  Добавляет разобранную строку в буффер с добавлением символа терминирования строки
  * @param
  * @retval
  */
void CAnswerBuffer::appendString(const QByteArray &dataToAdd)
{
  this->append(dataToAdd);
  this->append((char)0x00);
  this->checkLine();
}
