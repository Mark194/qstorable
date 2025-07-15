#pragma once

#ifdef QS_BINARY_SUPPORT
#include <QDataStream>
#endif

#define QS_META_OBJECT_METHOD \
    virtual const QMetaObject * metaObject() const { \
        return &this->staticMetaObject; \
    } \

#define QS_STORABLE QS_META_OBJECT_METHOD

#define QSTORABLE \
    Q_GADGET \
    QS_META_OBJECT_METHOD

class QStorable
{
    Q_GADGET
    QS_STORABLE
public:
    QStorable() = default;

    virtual ~QStorable() = default;
};