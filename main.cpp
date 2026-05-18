#include "mainwindow.h"
#include "form.h"
#include "macro.h"
#include "qnetworkaccessmanager.h"

#include <QApplication>
#include <QCoreApplication>
#include <Windows.h>
#include <QThread>

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "modules.h"

Macro *macro;
QThread *macroThread;

Modules *modules;
QThread *modulesThread;


HHOOK keyboardHook = nullptr;
HHOOK mouseHook = nullptr;

bool leftMouseHeld = false;
bool rightMouseHeld = false;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            int vkCode = pKeyBoard->vkCode;

            if (vkCode == macro->bindLeft && !leftMouseHeld)
            {
                QMetaObject::invokeMethod(macro, "startstopLeft", Qt::QueuedConnection);
                if(macro->holdLeft) leftMouseHeld = true;
            }
            if (vkCode == macro->bindRight && !rightMouseHeld)
            {
                QMetaObject::invokeMethod(macro, "startstopRight", Qt::QueuedConnection);
                if(macro->holdRight) rightMouseHeld = true;
            }
            if(vkCode == modules->fishingRodBind){
                modules->fishingRod();
            }
            if(vkCode == modules->bowPunchBind){
                modules->bowPunch();
            }
            if(vkCode == modules->enderPearlBind){
                modules->enderPearl();
            }
            if(vkCode == modules->snowBallBind){
                modules->snowBall();
            }

        } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        {
            int vkCode = pKeyBoard->vkCode;

            if ((vkCode == macro->bindLeft) && macro->holdLeft && leftMouseHeld)
            {
                QMetaObject::invokeMethod(macro, "startstopLeft", Qt::QueuedConnection);
                leftMouseHeld = false;
            }
            if ((vkCode == macro->bindRight) && macro->holdRight && rightMouseHeld)
            {
                QMetaObject::invokeMethod(macro, "startstopRight", Qt::QueuedConnection);
                rightMouseHeld = false;
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_XBUTTONDOWN)
        {
            WORD button = HIWORD(pMouseStruct->mouseData);
            if (button == macro->bindLeft && !leftMouseHeld)
            {
                // Boczne przycisk 1 (np. Back)
                QMetaObject::invokeMethod(macro, "startstopLeft", Qt::QueuedConnection);
                if(macro->holdLeft) leftMouseHeld = true;
            }
            if (button == macro->bindRight && !rightMouseHeld)
            {
                // Boczne przycisk 1 (np. Back)
                QMetaObject::invokeMethod(macro, "startstopRight", Qt::QueuedConnection);
                if(macro->holdRight) rightMouseHeld = true;

            }
        } else if (wParam == WM_XBUTTONUP)
            {
                WORD button = HIWORD(pMouseStruct->mouseData);
                if (button == macro->bindLeft && macro->holdLeft && leftMouseHeld)
                {
                    // Boczne przycisk 1 (np. Back)
                    QMetaObject::invokeMethod(macro, "startstopLeft", Qt::QueuedConnection);
                    leftMouseHeld = false;
                }
                if (button == macro->bindRight && macro->holdRight && rightMouseHeld)
                {
                    // Boczne przycisk 1 (np. Back)
                    QMetaObject::invokeMethod(macro, "startstopRight", Qt::QueuedConnection);
                    rightMouseHeld = false;

                }
            }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}


void installHook()
{
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

    if (!keyboardHook) {
        qDebug("Failed to install hook");
    }
}

void uninstallHook()
{
    if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
    }
    if(mouseHook){
        UnhookWindowsHookEx(mouseHook);
    }
}

QString readLicenseFromFile()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(path + "/license.lic");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString key = in.readLine().trimmed();
        file.close();
        return key;
    }
    return "";
}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    installHook();
    QObject::connect(&a, &QCoreApplication::aboutToQuit, []() {
        uninstallHook();
    });

    macro = new Macro();
    macroThread = new QThread();

    macro->moveToThread(macroThread);

    //QObject::connect(macroThread, &QThread::started, macro, &Macro::init);
    QObject::connect(macroThread, &QThread::finished, macro, &QObject::deleteLater);
    QObject::connect(macroThread, &QThread::finished, macroThread, &QObject::deleteLater);

    macroThread->start();

    modules = new Modules();
    modulesThread = new QThread();

    modules->moveToThread(modulesThread);

    QObject::connect(modulesThread, &QThread::finished, modules, &QObject::deleteLater);
    QObject::connect(modulesThread, &QThread::finished, modulesThread, &QObject::deleteLater);

    modulesThread->start();

    QString savedKey = readLicenseFromFile();

    MainWindow w;
    Form form;


    if (form.verifyLicenseKey(savedKey, "LOWICZ8")) {
        // form.checkLicenseWithServerDate();
        w.show();


    } else {
        form.show();
    }



    return a.exec();


}
