#include <QDebug>
#include <QSerialPortInfo>

#include "ComportExchanger.h"


// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

/**
  * @brief  Конструктор класса
  * @param
  * @retval
  */
CComPortExchanger::CComPortExchanger(QObject *parent):
  CAbstractExchanger(parent)
{
  ComPortThread::t_Settings settings;

  settings.countAttempts = 3;
  settings.timeoutWaitAnswer = 5000;
  settings.baudRate = 115200;

  this->comPort = new ComPortThread(settings);
  this->makeSingnalSlots();
}

/**
  * @brief  Переопределение абстрактного метода для подключения к устройству
  * @param
  * @retval
  */
void CComPortExchanger::connectDevice()
{
  qDebug() << "Try to open COM Port";
  QString portName = this->getPortName();

  if (portName.isEmpty()) {
    qDebug() << "Device not found";
    emit this->signalConnectionResult(false);
    return;
  }
  qDebug() << "Port name: " << portName;
  this->comPort->open(portName);
}

/**
  * @brief  Переопределение абстрактного метода для физической отправки данных на устройство
  * @param
  * @retval
  */
void CComPortExchanger::sendData(const QByteArray &cmdToSend, bool waitAnswer)
{
  QByteArray data = cmdToSend;
  data.append("\r\n");
  this->comPort->sendData(data, waitAnswer);
}

/**
  * @brief  Сохраняет переданный в установочных параметрах номер COM порта
  * @param  Или номер порта в формате ("COMXX") или его текстовый дескриптор
  * @retval
  */
void CComPortExchanger::setupComPort(const QString &portNameDescriptor)
{
  this->portNameDescriptor = portNameDescriptor;
}

/* ******************* END *****************************
   ****************** PUBLIC методы ********************
   ***************************************************** */

// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ***************** PRIVATE методы ********************
   ***************************************************** */

/**
  * @brief  Создаем связки сигналов и слотов
  * @param
  * @retval
  */
void CComPortExchanger::makeSingnalSlots()
{
  connect(this->comPort, SIGNAL(signalResultOpen(bool)),
          this, SLOT(slotPortOpenResult(bool)));
  connect(this->comPort, SIGNAL(signalClose()),
          this, SLOT(slotDisconnected()));
  connect(this->comPort, SIGNAL(signalIncomingData(const QByteArray&)),
          this, SLOT(slotIncomingData(const QByteArray&)));
}

/**
  * @brief  Из подключенных к ПК устройств по ком порту находит порт с подключенным к нему трекером
  * @param
  * @retval
  */
QString CComPortExchanger::getPortName()
{
  if (this->portNameDescriptor.startsWith("COM")) {
    return this->portNameDescriptor;
  }

  foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
    if (info.description() == this->portNameDescriptor) {
      return info.portName();
    }
  }
  return QString();
}

/* ******************* END *****************************
   ***************** PRIVATE методы ********************
   ***************************************************** */

// ---------------------------------------------------------------

/* ***************** BEGIN *****************************
   ***************** СЛОТЫ *****************************
   ***************************************************** */

void CComPortExchanger::slotPortOpenResult(bool result)
{
  qDebug() << "ComPort open result: " << result;
  emit this->signalConnectionResult(result);
}

void CComPortExchanger::slotDisconnected()
{
  qDebug() << "Com port is disconnected";
  emit this->signalDisconnected();
}

void CComPortExchanger::slotIncomingData(const QByteArray &data)
{
  this->gotIncomingData(data);
}

/* ******************* END *****************************
   ***************** СЛОТЫ *****************************
   ***************************************************** */

// ---------------------------------------------------------------
