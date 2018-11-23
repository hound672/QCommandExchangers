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
  std::freopen("QCommandExchangers_UnitTest.log", "w", stdout);
  QCoreApplication a(argc, argv);
  
  qputenv("QCOM_EXCH", QByteArray("1"));

  qRegisterMetaType<QAnswerBuffer>("QAnswerBuffer");
//  qRegisterMetaType<QAbstractExchanger::TAnswersList>("QAbstractExchanger::TAnswersList");
  
  QTest::qExec(new CTestCommandBuffer, argc, argv);
  QTest::qExec(new CTestAnswerBuffer, argc, argv);
  QTest::qExec(new CTestCommandProcessor, argc, argv);
  QTest::qExec(new CTestAbstractExchanger, argc, argv);

  return 0;
}
