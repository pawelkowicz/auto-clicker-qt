#include "modules.h"
#include "qsettings.h"

Modules::Modules() {

    QSettings settings("config.ini", QSettings::IniFormat);

    fishingRodBind = settings.value("fishingRodBind", 82).toInt();
    fishingRodSlot = settings.value("fishingRodSlot", 7).toInt();

    bowPunchBind = settings.value("bowPunchBind", 70).toInt();
    bowSlot = settings.value("bowPunchSlot", 1).toInt();

    enderPearlBind = settings.value("enderPearlBind", 86).toInt();
    enderPearlSlot = settings.value("enderPearlSlot", 2).toInt();

    snowBallBind = settings.value("snowBallBind", 90).toInt();
    snowBallSlot = settings.value("snowBallSlot", 3).toInt();

    autoBlock = settings.value("autoBlock", 0).toBool();
    blockSlot = settings.value("blockSlot", 4).toInt();

}

#include <Windows.h>
#include <QDebug>
#include <QTimer>

void Modules::pressKey(WORD keyCode) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = keyCode;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    qDebug() << "Wciśnięto klawisz o kodzie:" << keyCode;
}

// typButton: 0 = lewy, 1 = prawy, 2 = środkowy
void Modules::clickMouse(int typButton) {
    INPUT ip{};
    ip.type = INPUT_MOUSE;
    ip.mi.dx = 0;
    ip.mi.dy = 0;
    ip.mi.mouseData = 0;
    ip.mi.time = 0;
    ip.mi.dwExtraInfo = 0;

    switch (typButton) {
    case 0: ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN; break;
    case 1: ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN; break;
    case 2: ip.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN; break;
    default: return;
    }
    SendInput(1, &ip, sizeof(INPUT));

    switch (typButton) {
    case 0: ip.mi.dwFlags = MOUSEEVENTF_LEFTUP; break;
    case 1: ip.mi.dwFlags = MOUSEEVENTF_RIGHTUP; break;
    case 2: ip.mi.dwFlags = MOUSEEVENTF_MIDDLEUP; break;
    }
    SendInput(1, &ip, sizeof(INPUT));
}




void Modules::fishingRod()
{
    pressKey(0x30+fishingRodSlot);
    QTimer::singleShot(20, [this]() {
        clickMouse(1);
    });

    QTimer::singleShot(200, [this]() {
        pressKey(0x30+swordSlot);
    });

}

void Modules::bowPunch()
{
    pressKey(0x30+bowSlot);
    QTimer::singleShot(20, [this]() {
        INPUT ip = {0};
        ip.type = INPUT_MOUSE;

        ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        SendInput(1, &ip, sizeof(INPUT));

        QTimer::singleShot(200, this, [=]() {
            INPUT ipUp = {0};
            ipUp.type = INPUT_MOUSE;
            ipUp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            SendInput(1, &ipUp, sizeof(INPUT));

            QTimer::singleShot(20, [this]() {
                pressKey(0x30+swordSlot);

            });
        });


    });

}

void Modules::enderPearl()
{
    pressKey(0x30+enderPearlSlot);
    QTimer::singleShot(50, [this]() {
        clickMouse(1);

        QTimer::singleShot(50, [this]() {
            pressKey(0x30+swordSlot);
        });
    });


}

void Modules::snowBall()
{
    pressKey(0x30+snowBallSlot);
    QTimer::singleShot(50, [this]() {
        clickMouse(1);

        QTimer::singleShot(50, [this]() {
            pressKey(0x30+swordSlot);
        });
    });
}

void Modules::setSwordSlot()
{
    pressKey(0x30+swordSlot);
}

void Modules::setBlockSlot()
{
    pressKey(0x30+blockSlot);
}
