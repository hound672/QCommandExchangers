#ifndef COMPORTEXCHANGER_H
#define COMPORTEXCHANGER_H

#include "AbstractExchanger.h"

#include "ComPortThread/comportthread.h"

class CComPortExchanger : public CAbstractExchanger
{
  Q_OBJECT

public:
  CComPortExchanger(QObject *parent = 0);

  void connectDevice();
  void sendData(const QByteArray &cmdToSend, bool waitAnswer = false);
  void setupParams(const QString &portNameDescriptor);

// ************** PUBLIC **************

private:
  ComPortThread *comPort;
  QByteArray answerBuffer; // буффер для хранения ответа от трекера
  QString portNameDescriptor;

  void makeSingnalSlots();
  QString getPortName();

// ************** PRIVATE **************

private slots:
  void slotPortOpenResult(bool result);
  void slotDisconnected();
  void slotIncomingData(const QByteArray &data);

// ************** PRIVATE SLOTS **************

};

#endif // COMPORTEXCHANGER_H
