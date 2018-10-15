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
    parseOk = 0,        
    parseErrorPrefix,
    parseNeedParse,
    parseErrorIndex,
  };

  enum ELineStatus {
    lineCompleted = 0,
    lineNotCompleted,
    lineError,
  };

  // Структура описывающяя формат разбора команды                      
  struct STextParsingDesc
  {
    QByteArray mPrefix; // префикс команды
    char mSeparator; // разделитель параметров команды
  };

public:
  CCommandBuffer(const QByteArray &data = QByteArray());
  ELineStatus checkLine();
  QByteArray getLine() const;
  void releaseLine();
  void resetBuffer();
  EResultParse parse(const STextParsingDesc &parseDescr);
  
  EResultParse getParam(quint32 index, QByteArray &data) const;
  QByteArray getParamArray(quint32 index) const;
  QString getParamString(quint32 index) const;
  QString getStringFromHeader() const;
  QStringList getParamStringList(quint32 index) const;
  int getParamInt(quint32 index, bool *ok = NULL) const;
  int getParamIntFromHex(quint32 index) const;
  double getParamDouble(quint32 index, bool *ok) const;

protected:
  QByteArray getParam(const QList<QByteArray> list, quint32 index) const;
  QList<QByteArray> splitParams(const STextParsingDesc &parseDescr) const;
  QList<QByteArray> splitData; // данные после метода parse разделенные согласно переданому ранее сепаратору
  quint32 mStartPayload; // индекс начала данных после заголовка
  quint32 endString; // индекс окончания строки

};

#endif // COMMANDBUFFER_H
