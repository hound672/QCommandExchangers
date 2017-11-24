/**
  * @version  0.1
  * @brief    Буффер для работы с командами
  */
#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include <QList>
#include <QByteArray>

class CCommandBuffer: public QByteArray
{

public:

  enum EResultParse {
    PARSE_OK = 0,
    PARSE_ERROR_PREFIX,
    PARSE_ERROR_NEED_PARSE,
    PARSE_ERROR_INDEX,
  };

  enum ELineStatus {
    LINE_COMPELETED = 0,
    LINE_NOT_COMPELETED,
    LINE_ERROR,
  };

  /*********************************************
   *  Структура описывающяя формат разбора команды
   *********************************************/
  struct STextParsingDesc
  {
    QByteArray m_prefix; // префикс команды
    char m_separator; // разделитель параметров команды
  };

public:
  CCommandBuffer();
  ELineStatus checkLine();
  QByteArray getLine() const;
  void releaseLine();
  void resetBuffer();
  EResultParse parse(const STextParsingDesc &parseDescr);
  EResultParse getParam(quint32 index, QByteArray &data);

private:
  QList<QByteArray> splitData; // данные после метода parse разделенные согласно переданому ранее сепаратору
  quint32 endString; // индекс окончания строки

};

#endif // COMMANDBUFFER_H
