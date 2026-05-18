#ifndef MODULES_H
#define MODULES_H

#include <QObject>
#include <windows.h>

class Modules : public QObject{
    Q_OBJECT
public:
    Modules();

    void pressKey(WORD keyCode);
    void clickMouse(int typButton);

    int swordSlot = 1;

    void fishingRod();

    int fishingRodBind = 0x52;

    int fishingRodSlot = 7;


    void bowPunch();

    int bowPunchBind = 0x46;

    int bowSlot = 3;


    void enderPearl();

    int enderPearlBind = 0x56;

    int enderPearlSlot = 7;


    void snowBall();

    int snowBallBind = 'Z';

    int snowBallSlot = 8;


    bool autoBlock = true;

    void setSwordSlot();

    void setBlockSlot();

    int blockSlot = 4;

};

#endif // MODULES_H
