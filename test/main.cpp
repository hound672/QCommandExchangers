#include <QDebug>
#include <QCoreApplication>
#include <QTest>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "TestCommandBuffer.h"
#include "TestAnswerBuffer.h"
#include "TestCommandProcessor.h"
#include "TestAbstractExchanger.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
  qputenv("QCOM_EXCH", QByteArray("1"));

  qRegisterMetaType<QAnswerBuffer>("QAnswerBuffer");
	bool noError = true;
  
  noError &= QTest::qExec(new CTestCommandBuffer, argc, argv) == 0;
  noError &= QTest::qExec(new CTestAnswerBuffer, argc, argv) == 0;
  noError &= QTest::qExec(new CTestCommandProcessor, argc, argv) == 0;
  noError &= QTest::qExec(new CTestAbstractExchanger, argc, argv) == 0;
	
	qDebug("No errors: %d", noError);

  return 0;
}
