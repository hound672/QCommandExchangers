#ifndef TESTCOMMANDBUFFER_H
#define TESTCOMMANDBUFFER_H

#include <QObject>

#include "CommandBuffer.h"

class CTestCommandBuffer : public QObject
{
  Q_OBJECT
public:
  explicit CTestCommandBuffer(QObject *parent = 0);

private:
  CCommandBuffer buffer;

private slots:
  void checkLine();
  void getLine();
  void parse();
  void testError();
};

#endif // TESTCOMMANDBUFFER_H
