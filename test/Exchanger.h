#ifndef EXCHANGER_H
#define EXCHANGER_H

#include <QObject>

#include "AbstractExchanger.h"

class CExchanger : public CAbstractExchanger
{
  Q_OBJECT
public:
  explicit CExchanger(QObject *parent = 0);
  void connectDevice() {}
  void disconnectDevice() {}
  void sendData(const QByteArray &cmdToSend, bool waitAnswer) {}
  
  void putData(const QByteArray &data);
  
signals:
  
public slots:
};

#endif // EXCHANGER_H
