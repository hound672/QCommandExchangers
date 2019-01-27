#ifndef TESTABSTRACTEXCHANGER_H
#define TESTABSTRACTEXCHANGER_H

#include <QObject>
#include <QSignalSpy>

#include "Exchanger.h"

class CTestAbstractExchanger : public QObject
{
  Q_OBJECT
public:
  explicit CTestAbstractExchanger(QObject *parent = 0);

private:
  CExchanger *mExchanger;
  QSignalSpy *mSpyAnswer;
  
private slots:
  void testGettingsRawData();
  void testGettingAnswer();
  void testCommandEnd();
  
};

#endif // TESTABSTRACTEXCHANGER_H
