#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();


    QString machineId;
    static QString getMachineId();

    static QString generateLicenseKey(const QString &machineId, const QString &secret);
    void checkLicense();

    bool verifyLicenseKey(const QString &licenseKey, const QString &secret);

    //void checkLicenseWithServerDate();

    QDate getServerDate();

private slots:

    void on_checkLicense_clicked();

    void on_copy_clicked();

private:
    Ui::Form *ui;
};

#endif // FORM_H
