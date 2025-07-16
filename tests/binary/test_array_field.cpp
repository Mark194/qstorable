#include <QtTest>

#include <QStorable/qstorable.hpp>

class TestFixedArray : public QStorable
{
    QSTORABLE
    QS_ARRAY_FIXED(int, nums, 5)
};

class TestFixedCollection : public QStorable
{
    QSTORABLE
    QS_COLLECTION_FIXED(QList, int, numbers, 10)
};

class TestArray : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        qDebug() << "Initializing binary serialization tests...";
    }

    void testFixedArray()
    {
        TestFixedArray array;

        for (int i = 0; i < 5; ++i) {
            array.nums()[i] = i;
        }

        const auto rawData = array.toBinary();

        TestFixedArray array2;

        array2.fromBinary(rawData);

        for (int i = 0; i < 5; ++i) {
            QCOMPARE(array2.nums()[i], i);
        }
    }

    void testCollections()
    {
        TestFixedCollection collection;

        QList<int> numbers;
        for (int i = 0; i < 10; ++i) {
            numbers.append(i);
        }

        collection.setnumbers(numbers);

        auto rawData = collection.toBinary();

        TestFixedCollection collection2;
        collection2.fromBinary(rawData);

        for (int i = 0; i < 10; ++i) {
            QCOMPARE(collection2.numbers()[i], i);
        }
    }
};

QTEST_APPLESS_MAIN(TestArray)
#include "test_array_field.moc"