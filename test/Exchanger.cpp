#include "QCommandExchangersGlobal.h"

#include "Exchanger.h"

// ======================================================================

CExchanger::CExchanger(QObject *parent) : CAbstractExchanger(NULL, parent)
{
  
}

// ======================================================================

void CExchanger::putData(const QByteArray &data)
{
  this->gotIncomingData(data);
}

// ======================================================================
