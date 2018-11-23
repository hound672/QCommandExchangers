#ifndef TESTANSWERBUFFER_H
#define TESTANSWERBUFFER_H

#include <QObject>

#include "QAnswerBuffer.h"

class CTestAnswerBuffer : public QObject
{
  Q_OBJECT
public:
  explicit CTestAnswerBuffer(QObject *parent = 0);

private:
  QAnswerBuffer buffer;


private slots:
  void testAppend();
  void testParams();
  void testFirstLast();
};

#endif // TESTANSWERBUFFER_H
