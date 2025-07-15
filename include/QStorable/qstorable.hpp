#pragma once

#ifdef QS_BINARY_SUPPORT
#include <QDataStream>
#endif

/* META OBJECT SYSTEM */
#include <QVariant>
#include <QMetaProperty>
#include <QMetaObject>
#include <QMetaType>

#define QS_META_OBJECT_METHOD \
    virtual const QMetaObject* metaObject() const { \
        return &staticMetaObject; \
    }

#define QS_STORABLE QS_META_OBJECT_METHOD

#define QSTORABLE \
    Q_GADGET \
    QS_META_OBJECT_METHOD

#ifdef QS_BINARY_SUPPORT
namespace qBinarySupport
{
inline void configureStream(QDataStream &stream,
                          QDataStream::ByteOrder byteOrder,
                          QDataStream::FloatingPointPrecision precision)
{
    stream.setByteOrder(byteOrder);
    stream.setFloatingPointPrecision(precision);
}

template <typename Container, typename T>
void serializeFieldTo(Container& data, const T& value,
                    QDataStream::ByteOrder byteOrder,
                    QDataStream::FloatingPointPrecision precision)
{
    QDataStream stream(&data, QIODevice::WriteOnly);
    configureStream(stream, byteOrder, precision);
    stream << value;
}

template <typename T, typename Container>
T deserializeFieldFrom(const Container& data,
                     QDataStream::ByteOrder byteOrder,
                     QDataStream::FloatingPointPrecision precision)
{
    QDataStream stream(data);
    configureStream(stream, byteOrder, precision);
    T value;
    stream >> value;
    return value;
}
};
#endif

class QStorable
{
    Q_GADGET
    QS_STORABLE

public:
    QStorable() = default;
    virtual ~QStorable() = default;

#ifdef QS_BINARY_SUPPORT
public:
    void setByteOrder(QDataStream::ByteOrder byteOrder) {
        m_byteOrder = byteOrder;
    }

    void setPrecision(QDataStream::FloatingPointPrecision precision) {
        m_precision = precision;
    }

    QByteArray toBinary() const {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        qBinarySupport::configureStream(stream, m_byteOrder, m_precision);

        const QMetaObject* meta = metaObject();
        for (int i = 0; i < meta->propertyCount(); ++i) {
            QMetaProperty prop = meta->property(i);
            if (prop.userType() == qMetaTypeId<QByteArray>()) {
                QByteArray value = prop.readOnGadget(this).toByteArray();
                stream << value;
            }
        }
        return data;
    }

    void fromBinary(const QByteArray& data) {
        QDataStream stream(data);
        qBinarySupport::configureStream(stream, m_byteOrder, m_precision);

        const QMetaObject* meta = metaObject();
        for (int i = 0; i < meta->propertyCount(); ++i) {
            QMetaProperty prop = meta->property(i);
            if (prop.userType() == qMetaTypeId<QByteArray>()) {
                QByteArray value;
                stream >> value;
                prop.writeOnGadget(this, value);
            }
        }
    }

protected:
    QDataStream::ByteOrder m_byteOrder = QDataStream::BigEndian;
    QDataStream::FloatingPointPrecision m_precision = QDataStream::DoublePrecision;
#endif
};

#define GET(prefix, name) get_##prefix##_##name
#define SET(prefix, name) set_##prefix##_##name

#define QS_DECLARE_MEMBER(type, name) \
    private: \
        type m_##name{}; \
    public: \
        type name() const { return m_##name; } \
    void set##name(type value) { m_##name = value; }

#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_FIELD(type, name) \
    Q_PROPERTY(QByteArray name READ GET( binary, name ) WRITE SET( binary, name )) \
    protected: \
        QByteArray GET( binary, name )() const { \
            QByteArray data; \
            qBinarySupport::serializeFieldTo(data, m_##name, m_byteOrder, m_precision); \
            return data; \
        } \
        void SET( binary, name )(const QByteArray& data) { \
            m_##name = qBinarySupport::deserializeFieldFrom<type>(data, m_byteOrder, m_precision); \
        }
#else
#define QS_BINARY_FIELD(type, name)
#endif

#define QS_FIELD(type, name) \
    QS_DECLARE_MEMBER(type, name) \
    QS_BINARY_FIELD(type, name)
