#ifndef EXCHANGER_H
#define EXCHANGER_H

#include <QObject>

#include "QAbstractExchanger.h"

class CExchanger : public QAbstractExchanger
{
  Q_OBJECT
public:
  explicit CExchanger(QObject *parent = 0);
  void connectDevice() {}
  void disconnectDevice() {}
  void sendData(const QByteArray &cmdToSend) {}
  
  void putData(const QByteArray &data);
  
signals:
  
public slots:
};

#endif // EXCHANGER_H
