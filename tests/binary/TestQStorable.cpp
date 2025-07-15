#include <QtTest/QtTest>

#include <QStorable/qstorable.hpp>

class TestSerializable final : public QStorable {
    QSTORABLE
    QS_FIELD(int, intValue)
    QS_FIELD(float, floatValue)
    QS_FIELD(QString, stringValue)

public:
    int normalValue = 0;
};

class BinarySerializationTest final : public QObject
{
    Q_OBJECT
private slots:
    static void initTestCase()
    {
        qDebug("Initializing binary serialization tests...");
    }

    static void test_basic_serialization() {
        TestSerializable obj;
        obj.setintValue(42); // Теперь правильно
        obj.setfloatValue(3.14f);
        obj.setstringValue("Test");
        obj.normalValue = 100; // Теперь доступно

        QByteArray data = obj.toBinary();

        TestSerializable obj2;
        obj2.fromBinary(data);

        QCOMPARE(obj2.intValue(), 42);
        QCOMPARE(obj2.floatValue(), 3.14f);
        QCOMPARE(obj2.stringValue(), QString("Test"));
        QCOMPARE(obj2.normalValue, 0);
    }

    static void test_data_integrity() {
        TestSerializable obj;
        obj.setintValue(123456);
        obj.setfloatValue(123.456f);
        obj.setstringValue("Тест с русскими символами");

        QByteArray data = obj.toBinary();
        TestSerializable obj2;
        obj2.fromBinary(data);

        QCOMPARE(obj2.intValue(), 123456);
        QCOMPARE(obj2.floatValue(), 123.456f);
        QCOMPARE(obj2.stringValue(), QString("Тест с русскими символами"));
    }
};

QTEST_APPLESS_MAIN(BinarySerializationTest)
#include "TestQStorable.moc"