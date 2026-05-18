#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:

    void on_bindLPM_clicked();



    //void on_cpsLPM_valueChanged(int arg1);


    void on_minLPM_valueChanged(int arg1);

    void on_maxLPM_valueChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_bindPPM_clicked();

    //void on_cpsPPM_valueChanged(int arg1);


    void on_minPPM_valueChanged(int arg1);

    void on_maxPPM_valueChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    // void on_normalLPM_clicked();

    // void on_legitLPM_clicked();

    // void on_normalPPM_clicked();

    // void on_legitPPM_clicked();

    void on_toggleLPM_clicked();

    void on_holdLPM_clicked();

    void on_togglePPM_clicked();

    void on_holdPPM_clicked();

    void on_minJitterLPM_valueChanged(int arg1);

    void on_maxJitterLPM_valueChanged(int arg1);

    void on_minJitterPPM_valueChanged(int arg1);

    void on_maxJitterPPM_valueChanged(int arg1);

    void on_sinusLPM_checkStateChanged(const Qt::CheckState &arg1);

    void on_sinusPPM_checkStateChanged(const Qt::CheckState &arg1);


    void on_fishSlot_valueChanged(int arg1);

    void on_macro_clicked();

    void on_pvp_clicked();


    void on_fishingRodBind_clicked();


    void on_bowSlot_valueChanged(int arg1);

    void on_bowPunchBind_clicked();

    void on_enderPearlSlot_valueChanged(int arg1);

    void on_enderPearlBind_clicked();

    void on_snowBallSlot_valueChanged(int arg1);

    void on_snowBallBind_clicked();

    void on_blockON_clicked();

    void on_blockOFF_clicked();

    void on_blockSlot_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;

    bool listeningLPM = false;
    bool listeningPPM = false;

    bool listeningFishingRod = false;
    bool listeningBowPunch = false;
    bool listeningEnderPearl = false;
    bool listeningSnowBall = false;



    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
};
#endif // MAINWINDOW_H
