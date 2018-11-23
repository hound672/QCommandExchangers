#ifndef QCOMMANDEXCHANGERSGLOBAL_H
#define QCOMMANDEXCHANGERSGLOBAL_H

#include <QDebug>

#include "QAnswerBuffer.h"
#include "QAbstractExchanger.h"

Q_DECLARE_METATYPE(QAnswerBuffer)
Q_DECLARE_METATYPE(QAbstractExchanger::TAnswersList)

#define qDebugComExch if (qgetenv("QCOM_EXCH").isEmpty()); else qDebug

#endif // QCOMMANDEXCHANGERSGLOBAL_H
