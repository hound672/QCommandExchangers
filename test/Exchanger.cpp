#include "QCommandExchangersGlobal.h"

#include "Exchanger.h"

// ======================================================================

CExchanger::CExchanger(QObject *parent) : QAbstractExchanger(parent)
{
  
}

// ======================================================================

void CExchanger::putData(const QByteArray &data)
{
  GotIncomingData(data);
}

// ======================================================================
