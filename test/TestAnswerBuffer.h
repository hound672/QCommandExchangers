#ifndef TESTANSWERBUFFER_H
#define TESTANSWERBUFFER_H

#include <QObject>

#include "AnswerBuffer.h"

class CTestAnswerBuffer : public QObject
{
  Q_OBJECT
public:
  explicit CTestAnswerBuffer(QObject *parent = 0);

private:
  CAnswerBuffer buffer;

  void makeStrings();
  void testShowStrings(const CAnswerBuffer &answer1);

private slots:
  void testAppendString();
  void testGetParam();
};

#endif // TESTANSWERBUFFER_H
