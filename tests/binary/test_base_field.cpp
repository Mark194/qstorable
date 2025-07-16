#include <QtTest>

#include <QStorable/qstorable.hpp>

class TestSerializable final : public QStorable {
    QSTORABLE
    QS_FIELD(int, intValue)
    QS_FIELD(float, floatValue)
    QS_FIELD(QString, stringValue)

public:
    int normalValue = 0;
};

class TestBaseField final : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        qDebug("Initializing binary serialization tests...");
    }

    void test_basic_serialization() {
        TestSerializable obj;
        obj.intValue = 42;
        obj.floatValue = 3.14f;
        obj.stringValue = "Test";
        obj.normalValue = 100;

        QByteArray data = obj.toBinary();

        TestSerializable obj2;
        obj2.fromBinary(data);

        QCOMPARE(obj2.intValue, 42);
        QCOMPARE(obj2.floatValue, 3.14f);
        QCOMPARE(obj2.stringValue, QString("Test"));
        QCOMPARE(obj2.normalValue, 0);
    }

    void test_data_integrity() {
        TestSerializable obj;
        obj.intValue = 123456;
        obj.floatValue = 123.456f;
        obj.stringValue = "Тест с русскими символами";

        QByteArray data = obj.toBinary();
        TestSerializable obj2;
        obj2.fromBinary(data);

        QCOMPARE(obj2.intValue, 123456);
        QCOMPARE(obj2.floatValue, 123.456f);
        QCOMPARE(obj2.stringValue, QString("Тест с русскими символами"));
    }
};

QTEST_APPLESS_MAIN(TestBaseField)
#include "test_base_field.moc"