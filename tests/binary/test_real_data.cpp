#include <QtTest>

#include <QStorable/qstorable.hpp>

static constexpr quint32 SEPARATOR = 0x52504553;
static constexpr quint16 CAN_ID = 0xA;
static constexpr quint8 HEADER_LENGTH = 12;

class DataHeader : public QStorable
{
    QSTORABLE
    QS_FIELD(quint32, separator)
    QS_FIELD(quint16, length)
    QS_FIELD(quint8, headerId)
    QS_FIELD(quint8, timeHigh)
    QS_FIELD(quint32, timeLow)

    quint64 time{};

public:

    DataHeader() = default;

    bool operator==(const DataHeader &other) const {
        return headerId == other.headerId and
               timeHigh == other.timeHigh and
                timeLow == other.timeLow;
    }
};

class DataCan : public QStorable
{
    QSTORABLE
    QS_OBJECT(DataHeader, header)
    QS_FIELD(quint32, dataId)
    QS_FIELD(quint8, dataLength)
    QS_ARRAY_FIXED(quint16, dataCan, 8)
public:

    DataCan() = default;
};

constexpr quint64 TIME = 0x1996250415;

class TestRealData : public QObject
{
    Q_OBJECT


private Q_SLOTS:

    void testData()
    {
        DataHeader header;
        header.separator = SEPARATOR;
        header.length = 4;
        header.headerId = CAN_ID;
        header.timeHigh = TIME >> 32;
        header.timeLow = TIME & 0xFFFFFFFF;

        DataCan data;

        data.header = header;
        data.dataId = 0x621;
        data.dataLength = 0x5;
        for (quint32 i = 0; i < data.dataLength; ++i)
            data.dataCan[i] = i;

        const auto result = data.toBinary();

        DataCan binaryData;
        binaryData.fromBinary(result);

        QCOMPARE(binaryData.header, header);
        QCOMPARE( binaryData.dataId, data.dataId );
        QCOMPARE( binaryData.dataLength, data.dataLength );

        for (quint32 i = 0; i < data.dataLength; ++i)
            QCOMPARE( data.dataCan[i], i );
    }
};

QTEST_APPLESS_MAIN(TestRealData)
#include "test_real_data.moc"