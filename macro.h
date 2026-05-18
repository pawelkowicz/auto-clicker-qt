#ifndef MACRO_H
#define MACRO_H

#include "QTimer"
#include <QElapsedTimer>

class Macro : public QObject{
    Q_OBJECT
public:
    Macro(QObject *parent = nullptr);

    int bindLeft = 119;

    void leftClick();

    QTimer *timerLeft;
    bool isLeftOn = false;
    bool holdLeft = false;

    int minCPSLeft = 10;
    int maxCPSLeft = 15;

    int minJitterLeft = 0;
    int maxJitterLeft = 0;

    void parseLeftClick();

    bool pausesLeft = false;

    bool sinusLeft = false;

    QElapsedTimer startTimeLeft;

    void startConstantLeftClick(int cps);





    int bindRight = 118;

    void rightClick();

    QTimer *timerRight;
    bool isRightOn = false;
    bool holdRight = false;

    int minCPSRight = 10;
    int maxCPSRight = 15;

    int minJitterRight = 0;
    int maxJitterRight = 0;

    void parseRightClick();

    bool pausesRight = false;

    bool sinusRight = false;

    QElapsedTimer startTimeRight;

    void startConstantRightClick(int cps);

signals:
    void label8ColorChanged(const QString &color, const QString &text);
    void label9ColorChanged(const QString &color, const QString &text);


public slots:
    void startstopLeft();
    void startstopRight();
};

#endif // MACRO_H
