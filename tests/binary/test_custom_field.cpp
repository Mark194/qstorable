#include <QtTest>

#include <QStorable/qstorable.hpp>


class Range final : public QStorable {
    QSTORABLE
    QS_FIELD(float, x)
    QS_FIELD(float, y)

public:
    int normalValue = 0;
};

class CustomObject final : public QStorable {
    QSTORABLE
    QS_OBJECT(Range, range)
};

class TestCustomField final : public QObject
{
    Q_OBJECT
private slots:

    void initTestCase()
    {
        qDebug("Initializing binary serialization tests...");
    }

    void testCustomObject()
    {
        qRegisterMetaType<CustomObject>();
        qRegisterMetaType<Range>();

        Range range;
        range.setx(1.0);
        range.sety(2.0);

        CustomObject obj;
        obj.setrange(range);

        QByteArray data = obj.toBinary();

        CustomObject obj2;
        obj2.fromBinary(data);

        const auto rangeFromBytes = obj2.range();

        QCOMPARE(rangeFromBytes.x(), range.x());
        QCOMPARE(rangeFromBytes.y(), range.y());
    }
};

QTEST_APPLESS_MAIN(TestCustomField)
#include "test_custom_field.moc"