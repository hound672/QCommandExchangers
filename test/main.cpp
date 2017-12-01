#include <QCoreApplication>
#include <QTest>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "TestCommandBuffer.h"
#include "TestAnswerBuffer.h"

int main(int argc, char *argv[])
{
  std::freopen("testing.log", "w", stdout);
  QCoreApplication a(argc, argv);

  QTest::qExec(new CTestCommandBuffer, argc, argv);
  QTest::qExec(new CTestAnswerBuffer, argc, argv);

  return 0;
}