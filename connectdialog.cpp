#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <QMessageBox>

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    this->addButton = new HoButton(this);
    this->addButton->setPosSize(10, 130, 381, 50);
    this->addButton->setBackgroundColor(QBrush(QColor(0, 255, 0)));
    this->addButton->setText("添加");
    this->addButton->setTextColor(QColor(255, 255, 255));
    this->addButton->show();
    this->connect(this->addButton, &HoButton::onClick, this, &ConnectDialog::save);
}

ConnectDialog::~ConnectDialog()
{
    delete this->addButton;
    delete ui;
}

void ConnectDialog::save(int) {
    QString ip = this->ui->serverIP->text();
    QString port = this->ui->serverPort->text();
    QString username = this->ui->userName->text();
    QString password = this->ui->userPassword->text();
    QFile file(QApplication::applicationDirPath() + "/connect.txt");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "系统错误", "无法写入文件connect.txt");
        return;
    }

    QByteArray ba = file.readAll();
    file.close();
    QJsonObject jsonObject;
    if (!ba.isEmpty()) {
        QJsonDocument docRead = this->parseJson(ba);
        if (!docRead.isNull()) {
            jsonObject = docRead.object();
        }
    }
    QJsonObject o;
    QJsonArray allArray = jsonObject.value("all").toArray();
    QJsonObject item_o;
    item_o["ip"] = ip;
    item_o["port"] = port;
    item_o["username"] = username;
    item_o["password"] = password;
    allArray.append(item_o);
    o["all"] = allArray;
    QJsonDocument doc(o);
    this->writeJsonToFile(QApplication::applicationDirPath() + "/connect.txt", doc);
    this->close();
}

QJsonDocument ConnectDialog::parseJson(const QByteArray &jsonData) {
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
    }

    return document;
}

QList<DatabaseConfigure> ConnectDialog::parseJsonDocument(const QJsonDocument &document) {
    if (!document.isObject()) {
        qWarning() << "Document is not an object";
        return QList<DatabaseConfigure>();
    }

    QJsonObject jsonObject = document.object();
    QList<DatabaseConfigure> list;

    // 获取数组
    QJsonArray phonesArray = jsonObject.value("all").toArray();
    for (const QJsonValue &value : phonesArray) {
        DatabaseConfigure dc;
        QJsonObject o = value.toObject();
        dc.serverIP = o.value("ip").toString();
        dc.serverPort = o.value("port").toString();
        dc.userName = o.value("username").toString();
        dc.userPassword = o.value("password").toString();
        list.append(dc);
    }
    return list;
}

void ConnectDialog::writeJsonToFile(const QString &fileName, const QJsonDocument &document) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
}
