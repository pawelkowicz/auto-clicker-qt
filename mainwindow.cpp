#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <Windows.h>
#include <macro.h>
#include <QSettings>
#include <QButtonGroup>
#include "modules.h"

extern Macro *macro;
extern Modules *modules;

#include <QString>

QString vkToString(int vkCode)
{
    if(vkCode == 0){
        return "NONE";
    }

    if(vkCode == XBUTTON1){
        return "XBUTTON1";
    }
    if(vkCode == XBUTTON2){
        return "XBUTTON2";
    }

    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

    switch (vkCode) {
    case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
    case VK_PRIOR: case VK_NEXT: case VK_END: case VK_HOME:
    case VK_INSERT: case VK_DELETE: case VK_DIVIDE: case VK_NUMLOCK:
        scanCode |= 0x0100;
        break;
    }

    LONG lParam = (scanCode << 16);
    char keyName[128] = {0};
    if (GetKeyNameTextA(lParam, keyName, sizeof(keyName))) {
        return QString::fromLocal8Bit(keyName);
    }

    return QString("Unknown VK (0x%1)").arg(vkCode, 0, 16).toUpper();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ŁowiczClicker");

    QButtonGroup* group1 = new QButtonGroup(this);
    group1->addButton(ui->toggleLPM);
    group1->addButton(ui->holdLPM);

    QButtonGroup* group2 = new QButtonGroup(this);
    group2->addButton(ui->togglePPM);
    group2->addButton(ui->holdPPM);

    ui->bindLPM->setText(vkToString(macro->bindLeft));

    if(macro->holdLeft){
        ui->holdLPM->setChecked(1);
    } else{
        ui->toggleLPM->setChecked(1);
    }

    //ui->cpsLPM->setValue(macro->cpsLeft);

    ui->maxLPM->setValue(macro->maxCPSLeft);
    ui->minLPM->setValue(macro->minCPSLeft);

    if(macro->sinusLeft){
        ui->sinusLPM->setChecked(1);
    } else{
        ui->sinusLPM->setChecked(0);
    }

    ui->minJitterLPM->setValue(macro->minJitterLeft);
    ui->maxJitterLPM->setValue(macro->maxJitterLeft);

    //ui->LPM->setCurrentIndex(1);
    ui->checkBox->setChecked(macro->pausesLeft);



    ui->bindPPM->setText(vkToString(macro->bindRight));

    if(macro->holdRight){
        ui->holdPPM->setChecked(1);
    } else{
        ui->togglePPM->setChecked(1);
    }

    //ui->cpsPPM->setValue(macro->cpsRight);

    ui->maxPPM->setValue(macro->maxCPSRight);
    ui->minPPM->setValue(macro->minCPSRight);

    if(macro->sinusRight){
        ui->sinusPPM->setChecked(1);
    } else{
        ui->sinusPPM->setChecked(0);
    }

    ui->minJitterPPM->setValue(macro->minJitterRight);
    ui->maxJitterPPM->setValue(macro->maxJitterRight);

    //ui->PPM->setCurrentIndex(1);
    ui->checkBox_2->setChecked(macro->pausesRight);

    connect(macro, &Macro::label8ColorChanged, this,
            [this](const QString &color, const QString &text) {
                ui->label_8->setText(
                    "Lewy przycisk myszy - <span style='color:" + color + ";'>" + text + "</span>"
                    );
            });

    connect(macro, &Macro::label9ColorChanged, this,
            [this](const QString &color, const QString &text) {
                ui->label_9->setText(
                    "Prawy przycisk myszy - <span style='color:" + color + ";'>" + text + "</span>"
                    );
            });


    ui->fishingRodBind->setText(vkToString(modules->fishingRodBind));
    ui->fishSlot->setValue(modules->fishingRodSlot);

    ui->bowPunchBind->setText(vkToString(modules->bowPunchBind));
    ui->bowSlot->setValue(modules->bowSlot);

    ui->enderPearlBind->setText(vkToString(modules->enderPearlBind));
    ui->enderPearlSlot->setValue(modules->enderPearlSlot);

    ui->snowBallBind->setText(vkToString(modules->snowBallBind));
    ui->snowBallSlot->setValue(modules->snowBallSlot);

    ui->blockSlot->setValue(modules->blockSlot);

    if(modules->autoBlock){
        ui->blockON->setChecked(1);
    } else {
        ui->blockOFF->setChecked(1);
    }

}

MainWindow::~MainWindow()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    settings.setValue("bindLeft", macro->bindLeft);
    settings.setValue("holdLeft", macro->holdLeft);
    //settings.setValue("cpsLeft", macro->cpsLeft);
    settings.setValue("sinusLeft", macro->sinusLeft);
    settings.setValue("minCPSLeft", macro->minCPSLeft);
    settings.setValue("maxCPSLeft", macro->maxCPSLeft);
    settings.setValue("minJitterLeft", macro->minJitterLeft);
    settings.setValue("maxJitterLeft", macro->maxJitterLeft);
    settings.setValue("pausesLeft", macro->pausesLeft);

    settings.setValue("bindRight", macro->bindRight);
    settings.setValue("holdRight", macro->holdRight);
    //settings.setValue("cpsRight", macro->cpsRight);
    settings.setValue("sinusRight", macro->sinusRight);
    settings.setValue("minCPSRight", macro->minCPSRight);
    settings.setValue("maxCPSRight", macro->maxCPSRight);
    settings.setValue("minJitterRight", macro->minJitterRight);
    settings.setValue("maxJitterRight", macro->maxJitterRight);
    settings.setValue("pausesRight", macro->pausesRight);

    settings.setValue("fishingRodBind", modules->fishingRodBind);
    settings.setValue("fishingRodSlot", modules->fishingRodSlot);

    settings.setValue("bowPunchBind", modules->bowPunchBind);
    settings.setValue("bowPunchSlot", modules->bowSlot);

    settings.setValue("enderPearlBind", modules->enderPearlBind);
    settings.setValue("enderPearlSlot", modules->enderPearlSlot);

    settings.setValue("snowBallBind", modules->snowBallBind);
    settings.setValue("snowBallSlot", modules->snowBallSlot);

    settings.setValue("autoBlock", modules->autoBlock);
    settings.setValue("blockSlot", modules->blockSlot);


    delete ui;
}


void MainWindow::on_bindLPM_clicked()
{
    listeningLPM = true;
    ui->bindLPM->setText("...");
}



void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(listeningLPM){
        int vkCode = event->nativeVirtualKey();
        if(macro->bindRight == vkCode) return;

        qDebug() << "VK Code:" << vkCode;
        qDebug() << VK_F7;
        ui->bindLPM->setText(vkToString(vkCode));
        macro->bindLeft = vkCode;
        listeningLPM = false;
    }
    if(listeningPPM){
        int vkCode = event->nativeVirtualKey();
        if(macro->bindLeft == vkCode) return;


        qDebug() << "VK Code:" << vkCode;
        qDebug() << VK_F7;
        ui->bindPPM->setText(vkToString(vkCode));
        macro->bindRight = vkCode;
        listeningPPM = false;
    }
    if(listeningFishingRod){
        int vkCode = event->nativeVirtualKey();
        ui->fishingRodBind->setText(vkToString(vkCode));
        modules->fishingRodBind = vkCode;
        listeningFishingRod = false;
    }
    if(listeningBowPunch){
        int vkCode = event->nativeVirtualKey();
        ui->bowPunchBind->setText(vkToString(vkCode));
        modules->bowPunchBind = vkCode;
        listeningBowPunch = false;
    }
    if(listeningEnderPearl){
        int vkCode = event->nativeVirtualKey();
        ui->enderPearlBind->setText(vkToString(vkCode));
        modules->enderPearlBind = vkCode;
        listeningEnderPearl = false;
    }
    if(listeningSnowBall){
        int vkCode = event->nativeVirtualKey();
        ui->snowBallBind->setText(vkToString(vkCode));
        modules->snowBallBind = vkCode;
        listeningSnowBall = false;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(listeningBowPunch){
            listeningBowPunch = false;
            ui->bowPunchBind->setText("NONE");
            modules->bowPunchBind = 0;
        }else if(listeningFishingRod){
            listeningFishingRod = false;
            ui->fishingRodBind->setText("NONE");
            modules->fishingRodBind = 0;
        }else if(listeningEnderPearl){
            listeningEnderPearl = false;
            ui->enderPearlBind->setText("NONE");
            modules->enderPearlBind = 0;
        }else if(listeningSnowBall){
            listeningSnowBall = false;
            ui->snowBallBind->setText("NONE");
            modules->snowBallBind = 0;
        }else if(listeningLPM){
            listeningLPM = false;
            ui->bindLPM->setText("NONE");
            macro->bindLeft = 0;
        }else if(listeningPPM){
            listeningPPM = false;
            ui->bindPPM->setText("NONE");
            macro->bindRight = 0;
        }


    }

    if (listeningLPM) {
        int button = event->button();
        QString buttonName;


        switch (button) {
        case Qt::XButton1:
            if(macro->bindRight == XBUTTON1) return;
            buttonName = "XBUTTON1";
            qDebug() << XBUTTON1;
            macro->bindLeft = XBUTTON1;
            break;
        case Qt::XButton2:
            if(macro->bindRight == XBUTTON2) return;
            buttonName = "XBUTTON2";
            macro->bindLeft = XBUTTON2;
            break;
        }

        ui->bindLPM->setText(buttonName);
        listeningLPM = false;
    }
    if (listeningPPM) {
        int button = event->button();
        QString buttonName;

        switch (button) {
        case Qt::XButton1:
            if(macro->bindLeft == XBUTTON1) return;

            buttonName = "XBUTTON1";
            macro->bindRight = XBUTTON1;
            break;
        case Qt::XButton2:
            if(macro->bindLeft == XBUTTON2) return;
            buttonName = "XBUTTON2";
            macro->bindRight = XBUTTON2;
            break;
        }

        ui->bindPPM->setText(buttonName);
        listeningPPM = false;
    }
}



// void MainWindow::on_cpsLPM_valueChanged(int arg1)
// {
//     macro->cpsLeft = arg1;
// }




void MainWindow::on_minLPM_valueChanged(int arg1)
{
    macro->minCPSLeft = arg1;
    if(arg1==ui->maxLPM->value()){
        ui->sinusLPM->setEnabled(0);
    } else {
        ui->sinusLPM->setEnabled(1);
    }
}


void MainWindow::on_maxLPM_valueChanged(int arg1)
{
    macro->maxCPSLeft = arg1;
    if(arg1==ui->minLPM->value()){
        ui->sinusLPM->setEnabled(0);
    } else {
        ui->sinusLPM->setEnabled(1);
    }
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    //qDebug() << arg1;
    if(arg1==2){
        macro->pausesLeft = true;
    } else if(arg1==0){
        macro->pausesLeft = false;
    }
}


void MainWindow::on_bindPPM_clicked()
{
    listeningPPM = true;
    ui->bindPPM->setText("...");
}


// void MainWindow::on_cpsPPM_valueChanged(int arg1)
// {
//     macro->cpsRight = arg1;
// }




void MainWindow::on_minPPM_valueChanged(int arg1)
{
    macro->minCPSRight = arg1;
    if(arg1==ui->maxPPM->value()){
        ui->sinusPPM->setEnabled(0);
    } else {
        ui->sinusPPM->setEnabled(1);
    }
}


void MainWindow::on_maxPPM_valueChanged(int arg1)
{
    macro->maxCPSRight = arg1;
    if(arg1==ui->minPPM->value()){
        ui->sinusPPM->setEnabled(0);
    } else {
        ui->sinusPPM->setEnabled(1);
    }
}


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1==2){
        macro->pausesRight = true;
    } else if(arg1==0){
        macro->pausesRight = false;
    }

}


// void MainWindow::on_normalLPM_clicked()
// {
//     macro->legitLeft = false;
//     ui->LPM->setCurrentWidget(ui->normalPageLPM);
// }


// void MainWindow::on_legitLPM_clicked()
// {
//     macro->legitLeft = true;
//     ui->LPM->setCurrentWidget(ui->legitPageLPM);
// }


// void MainWindow::on_normalPPM_clicked()
// {
//     macro->legitRight = false;
//     ui->PPM->setCurrentWidget(ui->normalPagePPM);
// }


// void MainWindow::on_legitPPM_clicked()
// {
//     macro->legitRight = true;
//     ui->PPM->setCurrentWidget(ui->legitPagePPM);
// }


void MainWindow::on_toggleLPM_clicked()
{
    macro->holdLeft = false;
}


void MainWindow::on_holdLPM_clicked()
{
    macro->holdLeft = true;
}


void MainWindow::on_togglePPM_clicked()
{
    macro->holdRight = false;
}


void MainWindow::on_holdPPM_clicked()
{
    macro->holdRight = true;
}


void MainWindow::on_minJitterLPM_valueChanged(int arg1)
{
    macro->minJitterLeft = arg1;
}


void MainWindow::on_maxJitterLPM_valueChanged(int arg1)
{
    macro->maxJitterLeft = arg1;
}


void MainWindow::on_minJitterPPM_valueChanged(int arg1)
{
    macro->minJitterRight = arg1;
}


void MainWindow::on_maxJitterPPM_valueChanged(int arg1)
{
    macro->maxJitterRight = arg1;
}


void MainWindow::on_sinusLPM_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked){
        macro->sinusLeft = true;
    } else{
        macro->sinusLeft = false;
    }
}


void MainWindow::on_sinusPPM_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked){
        macro->sinusRight = true;
    } else{
        macro->sinusRight = false;
    }
}





void MainWindow::on_fishSlot_valueChanged(int arg1)
{
    modules->fishingRodSlot = arg1;
}


void MainWindow::on_macro_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page);
}


void MainWindow::on_pvp_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_2);

}





void MainWindow::on_fishingRodBind_clicked()
{
    ui->fishingRodBind->setText("...");
    listeningFishingRod = true;
}





void MainWindow::on_bowSlot_valueChanged(int arg1)
{
    modules->bowSlot = arg1;
}


void MainWindow::on_bowPunchBind_clicked()
{
    ui->bowPunchBind->setText("...");
    listeningBowPunch = true;
}


void MainWindow::on_enderPearlSlot_valueChanged(int arg1)
{
    modules->enderPearlSlot = arg1;
}


void MainWindow::on_enderPearlBind_clicked()
{
    ui->enderPearlBind->setText("...");
    listeningEnderPearl = true;
}


void MainWindow::on_snowBallSlot_valueChanged(int arg1)
{
    modules->snowBallSlot = arg1;
}


void MainWindow::on_snowBallBind_clicked()
{
    ui->snowBallBind->setText("...");
    listeningSnowBall = true;
}


void MainWindow::on_blockON_clicked()
{
    modules->autoBlock = true;
}


void MainWindow::on_blockOFF_clicked()
{
    modules->autoBlock = false;
}


void MainWindow::on_blockSlot_valueChanged(int arg1)
{
    modules->blockSlot = arg1;
}

