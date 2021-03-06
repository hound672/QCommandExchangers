#include <QDebug>

#include "QCommandExchangersGlobal.h"

#include "CommandBuffer.h"

// ======================================================================

CCommandBuffer::CCommandBuffer(const QByteArray &data):
  QByteArray(data),
  endString(0)
{
}

// ======================================================================

/**
  * @brief  Определяет наличие строки в буффере
  * @param
  * @retval
  */
CCommandBuffer::ELineStatus CCommandBuffer::checkLine()
{
  quint32 offset;
  quint32 sizeData;

  // ищем начало строки
  offset = 0;
  sizeData = (quint32)QByteArray::size();

  while (offset < sizeData &&
         QByteArray::at(offset) < ' ' &&
         QByteArray::at(offset) != 0x0D &&
         QByteArray::at(offset) != 0x0A) {
    offset++;
  }
  QByteArray::remove(0, offset); // удаляем мусор перед началом команды

  // ищем конец строки
  sizeData = (quint32)QByteArray::size();
  offset = 0;
  while (offset < sizeData &&
         QByteArray::at(offset) != 0x0D &&
         QByteArray::at(offset) != 0x0A &&
         QByteArray::at(offset) != 0x00) {
    offset++;
  }

  if (offset < sizeData) {
    QByteArray::operator [](offset) = 0x00;
    this->endString = offset;
    return ELineStatus::lineCompleted;
  } else if (offset) {
    return ELineStatus::lineNotCompleted;
  }

  return ELineStatus::lineError;
}

// ======================================================================

/**
  * @brief  Возвращает найденую ранее строку
  * @param
  * @retval
  */
QByteArray CCommandBuffer::getLine() const
{
  return QByteArray::mid(0, this->endString);
}

// ======================================================================

/**
  * @brief  Удаляет из буфере ранее найденую строку
  * @param
  * @retval
  */
void CCommandBuffer::releaseLine()
{
  QByteArray::remove(0, this->endString);
}

void CCommandBuffer::resetBuffer()
{
  this->endString = 0;
  this->splitData.clear();
  QByteArray::clear();
}

// ======================================================================

/**
  * @brief  Согласно переданному шаблону осуществляет поиск команды в буффере
  * @param  descr: описание для парсинга команды
  * @retval Результат выполнения парсинга
  */
CCommandBuffer::EResultParse CCommandBuffer::parse(const CCommandBuffer::STextParsingDesc &parseDescr)
{
  if (!this->getLine().startsWith(parseDescr.mPrefix)) {
    return EResultParse::parseErrorPrefix;
  }
  this->splitData = this->splitParams(parseDescr);
  mStartPayload = parseDescr.mPrefix.size();
  return EResultParse::parseOk;
}

// ======================================================================

/**
  * @brief  Возвращает параметр из текста команды
  * @param
  * @retval
  */
CCommandBuffer::EResultParse CCommandBuffer::getParam(quint32 index, QByteArray &data) const
{
  if (splitData.isEmpty()) {
    return EResultParse::parseNeedParse;
  }

  if (index >= (quint32)this->splitData.size()) {
    return EResultParse::parseErrorIndex;
  }

  data = this->getParam(this->splitData, index);

  return EResultParse::parseOk;
}

// ======================================================================

QByteArray CCommandBuffer::getParamArray(quint32 index) const
{
  QByteArray value;
  this->getParam(index, value);

  return value;
}

// ======================================================================

QString CCommandBuffer::getParamString(quint32 index) const
{
  QByteArray value;
  this->getParam(index, value);

  return QString::fromUtf8(value);
}

// ======================================================================

/**
  * @brief  Возвращает всю строку кроме заголовка описанного в prefix header
  * @param
  * @retval
  */
QString CCommandBuffer::getStringFromHeader() const
{
  return QString(QByteArray::mid(mStartPayload, QByteArray::size()));
}

// ======================================================================

/**
  * @brief  Возвращает список параметров
  * @param  index: начальный индекс с которого создавать список
  * @retval
  */
QStringList CCommandBuffer::getParamStringList(quint32 index) const
{
  QStringList data;

  for (quint32 i = index; i < (quint32)this->splitData.size(); i++) {
    data.append(this->getParamString(i));
  }
  return data;
}

// ======================================================================

/**
  * @brief  Возвращает параметр в виде числа
  * @param
  * @retval
  */
int CCommandBuffer::getParamInt(quint32 index, bool *ok) const
{
  QByteArray value;
  this->getParam(index, value);

  return value.toInt(ok);
}

// ======================================================================

/**
  * @brief  Возращает параметр в виде числа, который был представлен HEX строкой
  * @param
  * @retval
  */
int CCommandBuffer::getParamIntFromHex(quint32 index) const
{
  QByteArray value;
  this->getParam(index, value);

  return value.toUInt(NULL, 16);
}

// ======================================================================

/**
  * @brief  Возвращает параметр в виде числа с точкой
  * @param
  * @retval
  */
double CCommandBuffer::getParamDouble(quint32 index, bool *ok) const
{
  QByteArray value;

  this->getParam(index, value);
  return value.toDouble(ok);
}

// ======================================================================

/**
  * @brief  Из переданного списка возвращает запрашиваемый параметр
  * @param
  * @retval
  */
QByteArray CCommandBuffer::getParam(const QList<QByteArray> list, quint32 index) const
{
  if (index >= (quint32)list.size()) {
    return QByteArray();
  }

  return list[index];
}

// ======================================================================

/**
  * @brief  Возвращает список с разделенными параметрами
  * @param
  * @retval
  */
QList<QByteArray> CCommandBuffer::splitParams(const CCommandBuffer::STextParsingDesc &parseDescr) const
{
  char separator = parseDescr.mSeparator;
  int pos = parseDescr.mPrefix.size();
  QByteArray tmpData = this->getLine().mid(pos);
  return tmpData.split(separator);
}

// ======================================================================
