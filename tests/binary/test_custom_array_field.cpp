#include <QtTest>

#include <QStorable/qstorable.hpp>

class Range : public QStorable
{
    QSTORABLE
    QS_FIELD(double, x)
    QS_FIELD(double, y)
    QS_FIELD(double, z)
public:
    bool operator==(const Range& other) const {
        return x() == other.x() and y() == other.y() and z() == other.z();
    }
};

class CustomObject : public QStorable
{
    QSTORABLE
    QS_FIELD(QString, name)
    QS_FIELD(QString, description)
    QS_OBJECT_ARRAY_FIXED(Range, ranges, 3)
};

class TestCustomArrayField : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qDebug() << "Initializing binary serialization tests...";
    }

    void testFixedArrayObject()
    {
        Range range;
        range.setx(0.15);
        range.sety(0.5);
        range.setz(0.75);

        Range range2;
        range2.setx(1.15);
        range2.sety(1.5);
        range2.setz(1.75);

        Range range3;
        range3.setx(2.15);
        range3.sety(2.5);
        range3.setz(2.75);

        CustomObject customObject;
        customObject.setname("Marker One");
        customObject.setdescription("Marker in space");

        int i = 0;

        for (auto & temp : {range, range2, range3}) {
            customObject.ranges()[i] = temp;
            ++i;
        }

        auto array = customObject.toBinary();

        CustomObject customObject2;
        customObject2.fromBinary(array);

        QCOMPARE(customObject2.name(), customObject.name());
        QCOMPARE(customObject2.description(), customObject.description());


        i = 0;

        for (auto & temp : {range, range2, range3}) {
            QCOMPARE(customObject2.ranges()[i], temp);
            ++i;
        }

    }
};

QTEST_APPLESS_MAIN(TestCustomArrayField)
#include "test_custom_array_field.moc"