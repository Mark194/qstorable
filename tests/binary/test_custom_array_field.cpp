#include <QtTest>

#include <QStorable/qstorable.hpp>

#include "compares.hpp"

class Range : public QStorable
{
    QSTORABLE
    QS_FIELD(double, x)
    QS_FIELD(double, y)
    QS_FIELD(double, z)
public:
    bool operator==(const Range& other) const {
        return almostEqual(x, other.x) and
               almostEqual(y, other.y) and
               almostEqual(z, other.z);
    }
};

class CustomObject : public QStorable
{
    QSTORABLE
    QS_FIELD(QString, name)
    QS_FIELD(QString, description)
    QS_OBJECT_ARRAY_FIXED(Range, ranges, 3)
};

class CustomObject2 : public QStorable
{
    QSTORABLE
    QS_FIELD(QString, name)
    QS_FIELD(QString, description)
    QS_OBJECT_COLLECTION_FIXED(QVector, Range, ranges, 3)
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
        range.x = 0.15;
        range.y = 0.5;
        range.z = 0.75;

        Range range2;
        range2.x = 1.15;
        range2.y = 1.50;
        range2.z = 1.75;

        Range range3;
        range3.x = 2.15;
        range3.y = 2.5;
        range3.z = 2.75;

        CustomObject customObject;
        customObject.name = "Marker One";
        customObject.description = "Marker in space";

        int i = 0;

        for (auto & temp : {range, range2, range3}) {
            customObject.ranges[i] = temp;
            ++i;
        }

        auto array = customObject.toBinary();

        CustomObject customObject2;
        customObject2.fromBinary(array);

        QCOMPARE(customObject2.name, customObject.name);
        QCOMPARE(customObject2.description, customObject.description);


        i = 0;

        for (auto & temp : {range, range2, range3}) {
            QCOMPARE(customObject2.ranges[i], temp);
            ++i;
        }

    }

    void testFixedListObject()
    {
        Range range;
        range.x = 4.15;
        range.y = 5.50;
        range.z = 6.75;

        Range range2;
        range2.x = 7.15;
        range2.y = 8.50;
        range2.z = 9.75;

        Range range3;
        range3.x = 2.15;
        range3.y = 4.50;
        range3.z = 1.75;

        CustomObject2 customObject;
        customObject.name = "Marker Two";
        customObject.description = "Marker in other space";

        customObject.ranges = {range, range2, range3};

        const auto array = customObject.toBinary();

        CustomObject2 customObject2;
        customObject2.fromBinary(array);

        QCOMPARE(customObject2.name, customObject.name);
        QCOMPARE(customObject2.description, customObject.description);

        const QVector ranges = {range, range2, range3};

        for (int i = 0; i < 3; ++i)

            QCOMPARE(customObject2.ranges[i], ranges[i]);

    }
};

QTEST_APPLESS_MAIN(TestCustomArrayField)
#include "test_custom_array_field.moc"