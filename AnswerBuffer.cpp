#include <QDebug>

#include "AnswerBuffer.h"

CAnswerBuffer::CAnswerBuffer(qint32 cmdId) :
  mCmdId(cmdId),
  mResultCode(0),
  mResultStatus(EResultStatus::RS_OK)
{
}

/**
  * @brief  Добавляет строку в список ответов и разбирает ее согласно переданному описанию
  * @param
  * @retval
  */
void CAnswerBuffer::append(const QByteArray &dataToAdd, const CCommandBuffer::STextParsingDesc &parseDescr)
{
  CCommandBuffer cmdBuffer(dataToAdd);

  cmdBuffer.append((char)0x00);
  cmdBuffer.checkLine();
  cmdBuffer.parse(parseDescr);

  QList<CCommandBuffer>::append(cmdBuffer);
}

/**
  * @brief  Переопределяет метод родительского класса для получения первого элемента.
  *         С проверкой на пустой список
  * @param
  * @retval
  */
CCommandBuffer CAnswerBuffer::first() const
{
  if (QList<CCommandBuffer>::isEmpty()) {
    return CCommandBuffer();
  }

  return QList<CCommandBuffer>::first();
}

/**
  * @brief  Переопределяет метод родительского класса для получения последнего элемента.
  *         С проверкой на пустой список
  * @param
  * @retval
  */
CCommandBuffer CAnswerBuffer::last() const
{
  if (QList<CCommandBuffer>::isEmpty()) {
    return CCommandBuffer();
  }

  return QList<CCommandBuffer>::last();
}
