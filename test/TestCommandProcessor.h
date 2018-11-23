#ifndef TESTCOMMANDPROCESSOR_H
#define TESTCOMMANDPROCESSOR_H

#include <QObject>
#include <QSignalSpy>

#include "CommandProcessor.h"


Q_DECLARE_METATYPE(QAnswerBuffer)

class CTestCommandProcessor : public QObject
{
  Q_OBJECT
public:
  explicit CTestCommandProcessor(QObject *parent = 0);

private:
  CCommandProcessor *cmdProcessor;
  QSignalSpy *spyGotAnswer;

private slots:
  void testSignalTimeout();
  void testGettingResult();
  void testGettingAnswer();
  void testGettingAnswerResult();
  void testManyString();
  void testDifResult();
  void testUnexpected();
  void testManyCommands();
};

#endif // TESTCOMMANDPROCESSOR_H
