#include "form.h"
#include "ui_form.h"
#include <QtNetwork/QNetworkInterface>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include "macro.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDate>
#include <QTimer>
#include <QEventLoop>

extern Macro *macro;

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

    machineId = getMachineId();

    //ui->label->setText("ID Komputera: "+machineId);

    setWindowTitle("ŁowiczClicker - licencja");
}

Form::~Form()
{
    delete ui;
}


QString Form::getMachineId()
{
    foreach (const QNetworkInterface &netInterface, QNetworkInterface::allInterfaces()) {
        if (netInterface.flags().testFlag(QNetworkInterface::IsUp) &&
            !netInterface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            QString mac = netInterface.hardwareAddress();
            if (!mac.isEmpty()) {
                QByteArray macBytes = mac.toUtf8();
                QByteArray hash = QCryptographicHash::hash(macBytes, QCryptographicHash::Sha256);
                return QString(hash.toHex());
            }
        }
    }

    return "UNKNOWN";
}


// QString Form::generateLicenseKey(const QString &machineId, const QString &secret)
// {
//     QByteArray key = secret.toUtf8();
//     QByteArray message = machineId.toUtf8();
//     QByteArray result = QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256).toHex();
//     return QString(result);
// }

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

void saveLicenseToFile(const QString &licenseKey)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);

    QFile file(path + "/license.lic");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << licenseKey;
        file.close();
    }
}
#include <QProcess>


void restartApp() {
    QString appPath = QCoreApplication::applicationFilePath();
    QStringList arguments = QCoreApplication::arguments();

    QProcess::startDetached(appPath, arguments);

    QCoreApplication::quit();
}



bool Form::verifyLicenseKey(const QString &licenseKey, const QString &secret)
{
    QStringList parts = licenseKey.split(';');
    QString id, exp, sign;

    for (const QString &part : parts) {
        if (part.startsWith("ID=")) id = part.mid(3);
        else if (part.startsWith("EXP=")) exp = part.mid(4);
        else if (part.startsWith("SIGN=")) sign = part.mid(5);
    }

    if (id.isEmpty() || sign.isEmpty())
        return false;

    QString currentMachineId = getMachineId();
    if (id != currentMachineId){
        return false;

    }

    QString data = "ID=" + id;
    if (!exp.isEmpty())
        data += ";EXP=" + exp;

    QByteArray expectedSign = QMessageAuthenticationCode::hash(
                                  data.toUtf8(),
                                  secret.toUtf8(),
                                  QCryptographicHash::Sha256
                                  ).toHex();

    if (expectedSign != sign.toUtf8())
        return false;

    if(exp=="0000-00-00"){
        return true;
    }


    if (!exp.isEmpty()) {
        QDate expiry = QDate::fromString(exp, "yyyy-MM-dd");
        if (QDate::currentDate() > expiry){
            QMessageBox::critical(this, "Licencja wygasła", "Licencja ważna do: "+expiry.toString("yyyy-MM-dd"));
            return false;
        }
        QDate serverDate = getServerDate();
        if(serverDate.isValid()){
            if (serverDate > expiry){
                QMessageBox::critical(
                    this,
                    "Błąd Licencji",
                    "Wykryto manipulacje datą lub licencja wygasła.\n"
                    "Licencja ważna do: " + expiry.toString("yyyy-MM-dd") + "\n"
                    "Data z serwera: " + serverDate.toString("yyyy-MM-dd")
                    );

                return false;
            }
        }
    }

    return true;
}




QDate Form::getServerDate()
{
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://lowiczcoding.ct8.pl/date.php"));
    QNetworkReply* reply = manager.get(request);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(5000);

    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec();

    if (timer.isActive() && reply->error() == QNetworkReply::NoError) {
        QString serverDateStr = reply->readAll().trimmed();
        reply->deleteLater();
        QDate serverDate = QDate::fromString(serverDateStr, "yyyy-MM-dd");
        return serverDate;
    }

    reply->abort();
    reply->deleteLater();
    return QDate(); // isNull() = true
}



/*QString readLicenseExpFromFile()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(path + "/license.lic");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString key = in.readLine().trimmed();
        file.close();

        QStringList parts = key.split(';');
        QString id, exp, sign;

        for (const QString &part : parts) {
            if (part.startsWith("ID=")) id = part.mid(3);
            else if (part.startsWith("EXP=")) exp = part.mid(4);
            else if (part.startsWith("SIGN=")) sign = part.mid(5);
        }
        return exp;
    }
    return "";
}

void Form::checkLicenseWithServerDate()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply) {
        QString serverDateStr = reply->readAll().trimmed();
        reply->deleteLater();

        QDate serverDate = QDate::fromString(serverDateStr, "yyyy-MM-dd");

        if (!serverDate.isValid()) {
            return;
        }

        QDate expiry = QDate::fromString(readLicenseExpFromFile(), "yyyy-MM-dd"); // <- data z licencji

        if(expiry==QDate::fromString("0000-00-00", "yyyy-MM-dd")){
            return;
        }

        if (serverDate > expiry) {
            macro->bindLeft = 0;
            macro->bindRight = 0;
            QMessageBox::critical(this, "Licencja wygasła", "Wykryto manipulacje datą lub licencja wygasła");
            qApp->exit();
            return;
        }

        // Licencja ważna
        //QMessageBox::information(this, "OK", "Licencja ważna do " + expiry.toString("yyyy-MM-dd"));
    });

    manager->get(QNetworkRequest(QUrl("https://lowiczcoding.ct8.pl/date.php")));
}*/


void Form::on_checkLicense_clicked()
{
    ui->checkLicense->setEnabled(false);
    QString userKey = ui->lineEdit->text().trimmed();
    //QString expectedKey = generateLicenseKey(getMachineId(), "klucz");


    if (verifyLicenseKey(userKey, "klucz")) {
        saveLicenseToFile(userKey);

        QString expiry = userKey.section("EXP=", 1).section(';', 0, 0);
        if(expiry=="0000-00-00"){
            QMessageBox::information(this, "Sukces", "Licencja została aktywowana.\nLicencja jest bezterminowa (nigdy nie wygasa).");
        } else{
            QMessageBox::information(this, "Sukces", "Licencja została aktywowana.\nWażna do: " + expiry);
        }

        restartApp();

    } else {
        QMessageBox::critical(this, "Błąd", "Nieprawidłowy klucz licencyjny.");
    }
    ui->checkLicense->setEnabled(true);
}


void Form::on_copy_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(getMachineId());

}

