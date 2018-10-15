#ifndef QCOMMANDEXCHANGERSGLOBAL_H
#define QCOMMANDEXCHANGERSGLOBAL_H

#include <QDebug>

#include "AnswerBuffer.h"
#include "AbstractExchanger.h"

Q_DECLARE_METATYPE(CAnswerBuffer)
Q_DECLARE_METATYPE(CAbstractExchanger::TAnswersList)

#define qDebugComExch if (qgetenv("QCOM_EXCH").isEmpty()); else qDebug

#endif // QCOMMANDEXCHANGERSGLOBAL_H
