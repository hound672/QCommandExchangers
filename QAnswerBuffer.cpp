#include <QDebug>

#include "QAnswerBuffer.h"

QAnswerBuffer::QAnswerBuffer(qint32 cmdId) :
  mCmdId(cmdId),
  mResultCode(0),
  mResultStatus(EResultStatus::resOk)
{
}

// ======================================================================

/**
  * @brief  Добавляет строку в список ответов и разбирает ее согласно переданному описанию
  * @param
  * @retval
  */
void QAnswerBuffer::append(const QByteArray &dataToAdd, const CCommandBuffer::STextParsingDesc &parseDescr)
{
  CCommandBuffer cmdBuffer(dataToAdd);

  cmdBuffer.append((char)0x00);
  cmdBuffer.checkLine();
  cmdBuffer.parse(parseDescr);

  QList<CCommandBuffer>::append(cmdBuffer);
}

// ======================================================================

/**
  * @brief  Переопределяет метод родительского класса для получения первого элемента.
  *         С проверкой на пустой список
  * @param
  * @retval
  */
CCommandBuffer QAnswerBuffer::first() const
{
  if (QList<CCommandBuffer>::isEmpty()) {
    return CCommandBuffer();
  }

  return QList<CCommandBuffer>::first();
}

// ======================================================================

/**
  * @brief  Переопределяет метод родительского класса для получения последнего элемента.
  *         С проверкой на пустой список
  * @param
  * @retval
  */
CCommandBuffer QAnswerBuffer::last() const
{
  if (QList<CCommandBuffer>::isEmpty()) {
    return CCommandBuffer();
  }

  return QList<CCommandBuffer>::last();
}

// ======================================================================
