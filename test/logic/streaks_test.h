#ifndef __STREAKS_TEST_H
#define __STREAKS_TEST_H

#include <QObject>

class StreaksTest : public QObject
{
    Q_OBJECT

private slots:
    void testSanity();
    void test00();
};

#endif /* __STREAKS_TEST_H */
