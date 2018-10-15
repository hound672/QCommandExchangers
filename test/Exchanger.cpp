#include "QCommandExchangersGlobal.h"

#include "Exchanger.h"

// ======================================================================

CExchanger::CExchanger(QObject *parent) : CAbstractExchanger(parent)
{
  
}

// ======================================================================

void CExchanger::putData(const QByteArray &data)
{
  this->gotIncomingData(data);
}

// ======================================================================
