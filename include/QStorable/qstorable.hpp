#pragma once

#if defined(QS_BINARY_SUPPORT) || defined(QS_COMPACT_BINARY_SUPPORT)
#include <QDataStream>
#endif

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

#if defined(QS_BINARY_SUPPORT) || defined(QS_COMPACT_BINARY_SUPPORT)
namespace qBinarySupport
{
    inline void configureStream( QDataStream & stream, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        stream.setByteOrder( byteOrder );
        stream.setFloatingPointPrecision( precision );
    }

    template <typename Container, typename T>
    void serializeFieldTo( Container & data, const T & value, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        QDataStream stream( &data, QIODevice::WriteOnly );
        configureStream( stream, byteOrder, precision );
        stream << value;
    }

    template <typename T, typename Container>
    T deserializeFieldFrom( const Container & data, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        QDataStream stream( data );
        configureStream( stream, byteOrder, precision );
        T value;
        stream >> value;
        return value;
    }

    template <typename T, typename Container>
    void serializeArray( Container & data, const T & array, int size, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        QDataStream stream( &data, QIODevice::WriteOnly );
        configureStream( stream, byteOrder, precision );
        for ( int i = 0; i < size; ++i )
        {
            stream << array[i];
        }
    }

    template <typename T, typename Container>
    void deserializeArray( const Container & data, T & array, int size, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        QDataStream stream( data );
        configureStream( stream, byteOrder, precision );
        for ( int i = 0; i < size; ++i )
        {
            stream >> array[i];
        }
    }

    template <template<typename> class Collection, typename ItemType>
    void serializeCollection( QByteArray & data, const Collection<ItemType> & array, int size, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        QDataStream stream( &data, QIODevice::WriteOnly );
        configureStream( stream, byteOrder, precision );
        for ( int i = 0; i < qMin( size, array.size() ); ++i )
        {
            stream << array[i];
        }
    }

    template <template<typename> class Collection, typename ItemType>
    void deserializeCollection( const QByteArray & data, Collection<ItemType> & array, int size, QDataStream::ByteOrder byteOrder, QDataStream::FloatingPointPrecision precision )
    {
        QDataStream stream( data );
        configureStream( stream, byteOrder, precision );
        for ( int i = 0; i < size; ++i )
        {
            ItemType item;
            stream >> item;
            array << item;
        }
    }
};
#endif

class QStorable
{
    Q_GADGET QS_STORABLE

public:
    QStorable() = default;
    virtual ~QStorable() = default;

#if defined(QS_BINARY_SUPPORT) || defined(QS_COMPACT_BINARY_SUPPORT)

public:
    void setByteOrder( QDataStream::ByteOrder byteOrder )
    {
        m_byteOrder = byteOrder;
    }

    void setPrecision( QDataStream::FloatingPointPrecision precision )
    {
        m_precision = precision;
    }
#endif

#ifdef QS_BINARY_SUPPORT
    virtual QByteArray toBinary() const
    {
        QByteArray data;
        QDataStream stream( &data, QIODevice::WriteOnly );
        qBinarySupport::configureStream( stream, m_byteOrder, m_precision );

        const QMetaObject * meta = metaObject();
        for ( int i = 0; i < meta->propertyCount(); ++i )
        {
            QMetaProperty prop = meta->property( i );
            if ( prop.userType() == qMetaTypeId<QByteArray>() )
            {
                QByteArray value = prop.readOnGadget( this ).toByteArray();
                stream << value;
            }
        }
        return data;
    }

    virtual void fromBinary( const QByteArray & data )
    {
        QDataStream stream( data );
        qBinarySupport::configureStream( stream, m_byteOrder, m_precision );

        const QMetaObject * meta = metaObject();
        for ( int i = 0; i < meta->propertyCount(); ++i )
        {
            QMetaProperty prop = meta->property( i );
            if ( prop.userType() == qMetaTypeId<QByteArray>() )
            {
                QByteArray value;
                stream >> value;
                prop.writeOnGadget( this, value );
            }
        }
    }
#endif

#ifdef QS_BINARY_COMPACT_SUPPORT
    virtual QByteArray toCompact() const {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);

        const QMetaObject* metaObj = metaObject();
        for (int i = 0; i < metaObj->propertyCount(); ++i) {
            QMetaProperty prop = metaObj->property(i);
            if (prop.isReadable()) {
                QVariant value = prop.readOnGadget(this);
                if (value.isValid()) {
                    const int typeSize = QMetaType::sizeOf(value.userType());
                    if (typeSize > 0) {
                        stream.writeRawData(static_cast<const char*>(value.constData()), typeSize);
                    }
                }
            }
        }
        return data;
    }

    virtual void fromCompact(const QByteArray& data) {
        QDataStream stream(data);
        stream.setByteOrder(QDataStream::LittleEndian);

        const QMetaObject* metaObj = metaObject();
        for (int i = 0; i < metaObj->propertyCount(); ++i) {
            QMetaProperty prop = metaObj->property(i);
            if (prop.isWritable()) {
                const int typeId = prop.userType();
                const int typeSize = QMetaType::sizeOf(typeId);
                if (typeSize > 0) {
                    QVariant var(typeId, nullptr);
                    stream.readRawData(static_cast<char*>(var.data()), typeSize);
                    prop.writeOnGadget(this, var);
                }
            }
        }
    }
#endif

#if defined(QS_BINARY_SUPPORT) || defined(QS_COMPACT_BINARY_SUPPORT)

protected:
    QDataStream::ByteOrder m_byteOrder = QDataStream::BigEndian;
    QDataStream::FloatingPointPrecision m_precision = QDataStream::DoublePrecision;
#endif
};

#define GET(prefix, name) get_##prefix##_##name
#define SET(prefix, name) set_##prefix##_##name

#define QS_DECLARE_MEMBER(type, name) \
    public: \
        type name{};
#define QS_DECLARE_RVALUE_MEMBER(type, name) \
    public: \
        type name{};

#define QS_DECLARE_ARRAY_MEMBER(itemType, name, size) \
    public: \
        itemType name[(size)] = {};

#define QS_DECLARE_LIST_MEMBER(collection, itemType, name, size) \
    public: \
        collection<itemType> name;

#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_FIELD(type, name) \
    Q_PROPERTY(QByteArray name READ GET( binary, name ) WRITE SET( binary, name )) \
    protected: \
        QByteArray GET( binary, name )() const { \
            QByteArray data; \
            qBinarySupport::serializeFieldTo(data, name, m_byteOrder, m_precision); \
            return data; \
        } \
        void SET( binary, name )(const QByteArray& data) { \
            name = qBinarySupport::deserializeFieldFrom<type>(data, m_byteOrder, m_precision); \
        }
#else
#define QS_BINARY_FIELD(type, name)
#endif

#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_OBJECT(type, name) \
    Q_PROPERTY(QByteArray name READ GET(binary, name) WRITE SET(binary, name))                  \
    private:                                                                                \
        QByteArray GET(binary, name)() const {                                              \
            return name.toBinary();                                                             \
        }                                                                                       \
        void SET(binary, name)(const QByteArray & data) {                                      \
            name.fromBinary(data);                                                             \
        }
#else
#define QS_BINARY_OBJECT(type, name)
#endif

#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_ARRAY(itemType, name, size) \
    Q_PROPERTY(QByteArray name READ GET(binary, name) WRITE SET(binary, name)) \
    private: \
        QByteArray GET(binary, name)() const { \
            QByteArray data; \
            qBinarySupport::serializeArray(data, name, (size), m_byteOrder, m_precision); \
            return data; \
        } \
        void SET(binary, name)(const QByteArray & data) { \
            qBinarySupport::deserializeArray(data, name, (size), m_byteOrder, m_precision);\
        }
#else
#define QS_BINARRY_ARRAY(itemType, name, size)
#endif

#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_COLLECTION(collection, itemType, name, size) \
    Q_PROPERTY(QByteArray name READ GET(binary, name) WRITE SET(binary, name)) \
    private: \
        QByteArray GET(binary, name)() const { \
            QByteArray data; \
            auto arraySize = (size); \
            qBinarySupport::serializeCollection(data, name, arraySize, m_byteOrder, m_precision); \
            return data; \
        } \
        void SET(binary, name)(const QByteArray & data) { \
            auto arraySize = (size); \
            qBinarySupport::deserializeCollection(data, name, arraySize, m_byteOrder, m_precision);\
        }
#else
#define QS_BINARY_COLLECTION(collection, itemType, name, size)
#endif


// TODO: Binary array objects
#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_ARRAY_OBJECT(itemType, name, size) \
    Q_PROPERTY(QByteArray name READ GET(binary, name) WRITE SET(binary, name)) \
    QByteArray GET(binary, name)() const { \
        QByteArray result; \
        QDataStream stream(&result, QIODevice::WriteOnly); \
        qBinarySupport::configureStream(stream, m_byteOrder, m_precision); \
        for (int i = 0; i < (size); ++i) { \
            stream << name[i].toBinary(); \
        } \
        return result; \
    } \
    void SET(binary, name)(const QByteArray & data) { \
        QDataStream stream(data); \
        qBinarySupport::configureStream(stream, m_byteOrder, m_precision); \
        for (int i = 0; i < (size); ++i) { \
            QByteArray itemData; \
            stream >> itemData; \
            name[i].fromBinary(itemData); \
        } \
    }
#else
#define QS_BINARY_ARRAY_OBJECT(itemType, name, size)
#endif

#ifdef QS_BINARY_SUPPORT
#define QS_BINARY_COLLECTION_OBJECT(collection, itemType, name, size) \
    Q_PROPERTY(QByteArray name READ GET(binary, name) WRITE SET(binary, name)) \
    private: \
    QByteArray GET(binary, name)() const { \
        QByteArray result; \
        QDataStream stream(&result, QIODevice::WriteOnly); \
        for (auto i = 0; i < (size); ++i) \
            stream << name[i].toBinary(); \
        return result; \
    } \
    void SET(binary, name)(const QByteArray & data) { \
        QDataStream stream(data); \
        for (int i = 0; i < (size); ++i) { \
            QByteArray itemData; \
            stream >> itemData; \
            itemType obj; \
            obj.fromBinary(itemData); \
            name.append(obj); \
        } \
    }
#else
#define QS_BINARY_COLLECTION_OBJECT(collection, itemType, name, size)
#endif

#ifdef QS_BINARY_COMPACT_SUPPORT
#define QS_COMPACT_FIELD(type, name) \
    Q_PROPERTY( type name READ GET(compact, name) WRITE SET(compact, name) ) \
    type GET(compact, name)() const { return name; } \
    void SET(compact, name)(const type & value) { name = value; }
#else
#define QS_COMPACT_FIELD(type, name)
#endif


#define QS_FIELD(type, name) \
    QS_DECLARE_MEMBER(type, name) \
    QS_BINARY_FIELD(type, name) \
    QS_COMPACT_FIELD(type, name)

#define QS_OBJECT(type, name) \
    QS_DECLARE_RVALUE_MEMBER(type, name) \
    QS_BINARY_OBJECT(type, name)

#define QS_ARRAY_FIXED(type, name, size) \
    QS_DECLARE_ARRAY_MEMBER(type, name, size) \
    QS_BINARY_ARRAY(type, name, size)

#define QS_COLLECTION_FIXED(collection, itemType, name, size) \
    QS_DECLARE_LIST_MEMBER(collection, itemType, name, size) \
    QS_BINARY_COLLECTION(collection, itemType, name, size)

#define QS_OBJECT_ARRAY_FIXED(type, name, size) \
    QS_DECLARE_ARRAY_MEMBER(type, name, size) \
    QS_BINARY_ARRAY_OBJECT(type, name, size)

#define QS_OBJECT_COLLECTION_FIXED(collection, itemType, name, size) \
    QS_DECLARE_LIST_MEMBER(collection, itemType, name, size) \
    QS_BINARY_COLLECTION_OBJECT(collection, itemType, name, size)
