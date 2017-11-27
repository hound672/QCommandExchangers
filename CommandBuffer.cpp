#include <QDebug>

#include "CommandBuffer.h"

CCommandBuffer::CCommandBuffer():
  endString(0)
{

}

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
         QByteArray::at(offset) != 0x0A) {
    offset++;
  }

  if (offset < sizeData) {
    QByteArray::operator [](offset) = 0x00;
    this->endString = offset;
    return ELineStatus::LINE_COMPELETED;
  } else if (offset) {
    return ELineStatus::LINE_NOT_COMPELETED;
  }

  return ELineStatus::LINE_ERROR;
}

/**
  * @brief  Возвращает найденую ранее строку
  * @param
  * @retval
  */
QByteArray CCommandBuffer::getLine() const
{
  return QByteArray::mid(0, this->endString);
}

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

/**
  * @brief  Согласно переданному шаблону осуществляет поиск команды в буффере
  * @param  descr: описание для парсинга команды
  * @retval Результат выполнения парсинга
  */
CCommandBuffer::EResultParse CCommandBuffer::parse(const CCommandBuffer::STextParsingDesc &parseDescr)
{
  if (!QByteArray::startsWith(parseDescr.m_prefix)) {
    return EResultParse::PARSE_ERROR_PREFIX;
  }

  char separator = parseDescr.m_separator;
  int pos = parseDescr.m_prefix.size() + 1;
  QByteArray tmpData = this->getLine().mid(pos);
  this->splitData = tmpData.split(separator);

  return EResultParse::PARSE_OK;
}

/**
  * @brief  Возвращает параметр из текста команды
  * @param
  * @retval
  */
CCommandBuffer::EResultParse CCommandBuffer::getParam(quint32 index, QByteArray &data)
{
  if (splitData.isEmpty()) {
    return EResultParse::PARSE_ERROR_NEED_PARSE;
  }

  if (index >= (quint32)this->splitData.size()) {
    return EResultParse::PARSE_ERROR_INDEX;
  }

  data = this->splitData[index];

  return EResultParse::PARSE_OK;
}

/**
  * @brief  Возвращает параметр в виде числа
  * @param
  * @retval
  */
int CCommandBuffer::getParamInt(quint32 index)
{
  QByteArray value;
  this->getParam(index, value);

  return value.toInt();
}
