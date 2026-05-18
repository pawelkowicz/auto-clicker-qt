#include "macro.h"
#include "QTimer"
#include "QDebug"
#include "qthread.h"
#include <Windows.h>
#include <QRandomGenerator>
#include <QSettings>
#include "modules.h"

extern Modules *modules;

Macro::Macro(QObject *parent)
    : QObject(parent) {
    timerLeft = nullptr;
    timerRight = nullptr;

    QSettings settings("config.ini", QSettings::IniFormat);

    bindLeft = settings.value("bindLeft", 119).toInt();
    holdLeft = settings.value("holdLeft", false).toBool();
    sinusLeft = settings.value("sinusLeft", false).toBool();
    minCPSLeft = settings.value("minCPSLeft", 10).toInt();
    maxCPSLeft = settings.value("maxCPSLeft", 15).toInt();
    minJitterLeft = settings.value("minJitterLeft", 0).toInt();
    maxJitterLeft = settings.value("maxJitterLeft", 0).toInt();
    pausesLeft = settings.value("pausesLeft", false).toBool();

    bindRight = settings.value("bindRight", 118).toInt();
    holdRight = settings.value("holdRight", false).toBool();
    sinusRight = settings.value("sinusRight", false).toBool();
    minCPSRight = settings.value("minCPSRight", 10).toInt();
    maxCPSRight = settings.value("maxCPSRight", 15).toInt();
    minJitterRight = settings.value("minJitterRight", 0).toInt();
    maxJitterRight = settings.value("maxJitterRight", 0).toInt();
    pausesRight = settings.value("pausesRight", false).toBool();
}

void Macro::startstopLeft()
{
    if(!isLeftOn) {
        qDebug() << "Macro ON";
        isLeftOn = true;
        if(minCPSLeft==maxCPSLeft){
            startConstantLeftClick(minCPSLeft);
        } else{
            parseLeftClick();
            startTimeLeft.start();
        }
        emit label8ColorChanged("#50fa7b", "ON");

    } else {
        qDebug() << "Macro OFF";
        isLeftOn = false;

        emit label8ColorChanged("#ff5555", "OFF");
    }
}

void Macro::leftClick()
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));

    if(isLeftOn && (minCPSLeft != maxCPSLeft)) parseLeftClick();
}


#include <algorithm>


void Macro::parseLeftClick()
{
    int delayMs = 0;

    if(minCPSLeft > maxCPSLeft){
        std::swap(minCPSLeft, maxCPSLeft);
    }

    if(sinusLeft){
        double t = startTimeLeft.elapsed() / 1000.0;
        double freqHz = 0.2;

        double safeMaxCPS = maxCPSLeft;
        if ((maxCPSLeft - minCPSLeft) < 7.0)
            safeMaxCPS -= 0.5;


        double minDelay = 1000.0 / safeMaxCPS;
        double maxDelay = 1000.0 / minCPSLeft;

        double normalized = (std::sin(2 * M_PI * freqHz * t) + 1.0) / 2.0;
        double preDelay = minDelay + normalized * (maxDelay - minDelay);
        preDelay = std::max(preDelay, 1000.0 / maxCPSLeft);

        delayMs = static_cast<int>(std::ceil(preDelay));
    } else{
        int cps = QRandomGenerator::global()->bounded(minCPSLeft, maxCPSLeft + 1);
        delayMs = 1000 / cps;
    }

    if(minJitterLeft > maxJitterLeft){
        std::swap(minJitterLeft, maxJitterLeft);
    }

    delayMs += QRandomGenerator::global()->bounded(minJitterLeft, maxJitterLeft + 1);


    if(pausesLeft){
        if (QRandomGenerator::global()->bounded(0, 20) == 0) {
            delayMs += QRandomGenerator::global()->bounded(100, 300);
        }
    }

    if (delayMs < 1) delayMs = 1;


    QTimer::singleShot(delayMs, this, &Macro::leftClick);
}

#include <QtConcurrent/QtConcurrent>

void Macro::startConstantLeftClick(int cps)
{
    if (cps <= 0) return;


    int baseDelayUs = 1000000 / cps;


    if (minJitterLeft > maxJitterLeft)
        std::swap(minJitterLeft, maxJitterLeft);

    QtConcurrent::run([=]() {
        QElapsedTimer timer;
        timer.start();
        qint64 nextClickTime = timer.nsecsElapsed() + (baseDelayUs * 1000LL); // ns

        while (isLeftOn) {
            leftClick();


            int jitterMs = QRandomGenerator::global()->bounded(minJitterLeft, maxJitterLeft + 1);

            if (pausesLeft) {
                if (QRandomGenerator::global()->bounded(0, 20) == 0) {
                    jitterMs += QRandomGenerator::global()->bounded(100, 300);
                }
            }


            int totalDelayUs = baseDelayUs + jitterMs * 1000;

            nextClickTime += totalDelayUs * 1000LL; // ns


            while (timer.nsecsElapsed() < nextClickTime) {
                QThread::usleep(50); // minimalny odpoczynek
            }
        }
    });
}




void Macro::startstopRight()
{
    if(!isRightOn) {
        qDebug() << "Macro ON";
        isRightOn = true;
        if(minCPSRight == maxCPSRight){
            startConstantRightClick(minCPSRight);
        } else{
            parseRightClick();
            startTimeRight.start();
        }
        emit label9ColorChanged("#50fa7b", "ON");
        if(modules->autoBlock){
            modules->setBlockSlot();
        }
    } else {
        qDebug() << "Macro OFF";
        isRightOn = false;
        emit label9ColorChanged("#ff5555", "OFF");
        if(modules->autoBlock){
            modules->setSwordSlot();
        }
    }
}

void Macro::rightClick()
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &input, sizeof(INPUT));

    if(isRightOn && (minCPSRight != maxCPSRight)) parseRightClick();
}


void Macro::parseRightClick()
{
    int delayMs = 0;

    if(minCPSRight > maxCPSRight){
        std::swap(minCPSRight, maxCPSRight);
    }

    if(sinusRight){
        double t = startTimeRight.elapsed() / 1000.0;
        double freqHz = 0.2;

        double safeMaxCPS = maxCPSRight;
        if ((maxCPSRight - minCPSRight) < 7.0)
            safeMaxCPS -= 0.5;


        double minDelay = 1000.0 / safeMaxCPS;
        double maxDelay = 1000.0 / minCPSRight;

        double normalized = (std::sin(2 * M_PI * freqHz * t) + 1.0) / 2.0;
        double preDelay = minDelay + normalized * (maxDelay - minDelay);
        preDelay = std::max(preDelay, 1000.0 / maxCPSRight);

        delayMs = static_cast<int>(std::ceil(preDelay));
    } else{
        int cps = QRandomGenerator::global()->bounded(minCPSRight, maxCPSRight + 1);
        delayMs = 1000 / cps;
    }

    if(minJitterRight > maxJitterRight){
        std::swap(minJitterRight, maxJitterRight);
    }

    delayMs += QRandomGenerator::global()->bounded(minJitterRight, maxJitterRight + 1);

    if(pausesRight){
        if (QRandomGenerator::global()->bounded(0, 20) == 0) {
            delayMs += QRandomGenerator::global()->bounded(100, 300);
        }
    }

    if (delayMs < 1) delayMs = 1;


    QTimer::singleShot(delayMs, this, &Macro::rightClick);
}

void Macro::startConstantRightClick(int cps)
{
    if (cps <= 0) return;

    int baseDelayUs = 1000000 / cps;

    if (minJitterRight > maxJitterRight)
        std::swap(minJitterRight, maxJitterRight);

    QtConcurrent::run([=]() {
        QElapsedTimer timer;
        timer.start();
        qint64 nextClickTime = timer.nsecsElapsed() + (baseDelayUs * 1000LL); // ns

        while (isRightOn) {
            rightClick();

            int jitterMs = QRandomGenerator::global()->bounded(minJitterRight, maxJitterRight + 1);

            if (pausesRight) {
                if (QRandomGenerator::global()->bounded(0, 20) == 0) {
                    jitterMs += QRandomGenerator::global()->bounded(100, 300);
                }
            }

            int totalDelayUs = baseDelayUs + jitterMs * 1000;

            nextClickTime += totalDelayUs * 1000LL; // ns

            while (timer.nsecsElapsed() < nextClickTime) {
                QThread::usleep(50); // minimalny odpoczynek
            }
        }
    });
}

