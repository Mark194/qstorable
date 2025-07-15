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

#ifdef QS_BINARY_SUPPORT

public:

    void setByteOrder(QDataStream::ByteOrder byteOrder)
    {
        m_byteOrder = byteOrder;
    }

    void setPrecision(QDataStream::Precision precision)
    {
      m_precision = precision;
    }

    QByteArray toBinary() const
    {
        QByteArray data;

        QDataStream stream(&data, QIODevice::WriteOnly);

        qBinarySupport::serializeFieldTo(stream, m_byteOrder, m_precision);

        for (int i = 0; i < metaObject()->propertyCount(); ++i)
        {
            QMetaProperty prop = metaObject()->property(i);

            if(prop.userType() != qMetaTypeId<QByteArray>()) {
                continue;
            }

            QByteArray data = prop.read(this).toByteArray();
            stream << data;
        }
    }

    void fromBinary(const QByteArray& data)
    {
        QDataStream stream(data);

        qBinarySupport::serializeFieldTo(stream, m_byteOrder, m_precision);

        const QMetaObject* meta = metaObject();

        for(int i = 0; i < meta->propertyCount(); ++i) {
            QMetaProperty prop = meta->property(i);

            if(prop.userType() == qMetaTypeId<QByteArray>() &&
               prop.name().startsWith("byteArray_"))
            {
                QByteArray value;
                stream >> value;
                prop.write(this, value);
            }
        }
    }

protected:

    QDataStream::ByteOrder m_byteOrder;

    QDataStream::Precision m_precision;
#endif
};

#ifdef QS_BINARY_SUPPORT

namespace qBinarySupport
{
    inline void configureStream(QDataStream &stream,
                                QDataStream::ByteOrder byteOrder,
                                QDataStream::Precision precision)
    {
        stream.setByteOrder(byteOrder);
        stream.setPrecision(precision);
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
}
#endif

#define GET(prefix, name) ##prefix##_##name
#define SET(prefix, name) set_##prefix##_##name

#define QS_BINARY_FIELD(type, name) \
    Q_PROPERTY(QJsonValue name READ GET(json, name) WRITE SET(json,name)) \
    private: \
        QByteArray GET(byteArray, name)() const { \
            QByteArray data; \
            qBinarySupport::serializeFieldTo(data, name, m_byteOrder, m_precision); \
            return data; \
        } \
        void SET(byteArray, name)(const QByteArray& data) { \
            name = qBinarySupport::deserializeFieldFrom<type>(data, m_byteOrder, m_precision); \
        }
#else
    #define QS_BINARY_FIELD(type, name)
#endif

