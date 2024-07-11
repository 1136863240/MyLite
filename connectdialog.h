#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QList>
#include "hobutton.h"

namespace Ui {
class ConnectDialog;
}

typedef struct DatabaseConfigure {
    QString serverType;
    QString serverIP;
    QString serverPort;
    QString userName;
    QString userPassword;
} DatabaseConfigure;

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

private slots:
    void save(int);

private:
    QJsonDocument parseJson(const QByteArray &jsonData);
    QList<DatabaseConfigure> parseJsonDocument(const QJsonDocument &document);
    void writeJsonToFile(const QString &fileName, const QJsonDocument &document);
    Ui::ConnectDialog *ui;
    HoButton *addButton;
    QList<DatabaseConfigure> configureList;
};

#endif // CONNECTDIALOG_H
