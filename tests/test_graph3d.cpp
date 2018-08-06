#include <QTest>

#include "../src/graph3d.hpp"

class TestGraph3D: public Graph3D
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        qDebug("called before everything else");
    }
    void test()
    {
        QVERIFY(1 == 1);
    }
    void cleanupTestCase()
    {
        qDebug("called after myFirstTest and mySecondTest");
    }
};
