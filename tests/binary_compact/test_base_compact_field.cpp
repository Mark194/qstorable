#include <QtTest>

constexpr quint64 TIME = 0x1996250415;


#include <QStorable/qstorable.hpp>

static constexpr quint32 SEPARATOR = 0x52504553;
static constexpr quint8 HEADER_LENGTH = 12;

class CompactHeader : public QStorable
{
    QSTORABLE
    QS_FIELD(quint32, separator)
    QS_FIELD(quint16, length)
    QS_FIELD(quint8, headerId)
    QS_FIELD(quint8, timeHigh)
    QS_FIELD(quint32, timeLow)

    quint64 time{};

public:

    CompactHeader() = default;

    bool operator==(const CompactHeader &other) const {
        return headerId == other.headerId and
               timeHigh == other.timeHigh and
                timeLow == other.timeLow;
    }
};

class TestBaseField final : public QObject
{
    Q_OBJECT //

private Q_SLOTS:
    void initTestCase() {}

    void testBaseFields()
    {
        CompactHeader header{};
        header.separator = SEPARATOR;
        header.length = 12;
        header.headerId = 0xFF;
        header.timeHigh = TIME >> 32;
        header.timeLow = TIME & 0xFFFFFFFF;

        auto data = header.toCompact();

        CompactHeader header2{};

        header2.fromCompact( data );

        QCOMPARE( header, header2 );
    }
};

QTEST_APPLESS_MAIN( TestBaseField )
#include "test_base_compact_field.moc"
