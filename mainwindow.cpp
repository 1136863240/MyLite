#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->tab = 1;

    this->tabVec.append(new HoButton(this));
    this->tabVec[0]->setId(0);
    this->tabVec[0]->setText("连接管理");
    this->tabVec[0]->setTextFont(QFont("", 25));
    this->tabVec[0]->setTextColor(QColor(255, 255, 255));
    this->tabVec[0]->setBackgroundColor(QBrush(QColor(0, 255, 0)));
    this->tabVec[0]->setTextColor(QColor(0, 0, 0));
    this->tabVec[0]->setBackgroundColor(QBrush(QColor(255, 255, 255)));
    this->tabVec[0]->setPosSize(0, 0, 200, 100);
    this->tabVec[0]->setRadius(20);
    this->tabVec[0]->show();

    this->connect(this->tabVec[0], &HoButton::onClick, this, &MainWindow::onConnectTabClicked);

    this->tabVec.append(new HoButton(this));
    this->tabVec[1]->setId(1);
    this->tabVec[1]->setText("表");
    this->tabVec[1]->setTextFont(QFont("", 25));
    this->tabVec[1]->setTextColor(QColor(255, 255, 255));
    this->tabVec[1]->setBackgroundColor(QBrush(QColor(0, 255, 0)));
    this->tabVec[1]->setPosSize(200, 0, 100, 100);
    this->tabVec[1]->setRadius(20);
    this->tabVec[1]->show();

    this->tabVec.append(new HoButton(this));
    this->tabVec[2]->setId(2);
    this->tabVec[2]->setText("SQL");
    this->tabVec[2]->setTextFont(QFont("", 25));
    this->tabVec[2]->setTextColor(QColor(0, 0, 0));
    this->tabVec[2]->setBackgroundColor(QBrush(QColor(255, 255, 255)));
    this->tabVec[2]->setPosSize(300, 0, 100, 100);
    this->tabVec[2]->setRadius(20);
    this->tabVec[2]->show();

    for (HoButton *btn : this->tabVec) {
        this->connect(btn, &HoButton::onMouseEnter, this, &MainWindow::onTabMouseEnter);
        this->connect(btn, &HoButton::onMouseLeave, this, &MainWindow::onTabMouseLeave);
    }

    this->connListWidget = new QListWidget(this);
    if (!this->refreshList()) {
        exit(0);
    }
    this->connListWidget->setGeometry(0, 100, this->width() * 0.3, this->height() - 100);
    this->connListWidget->show();

    this->connDialog = new ConnectDialog(this);
}

MainWindow::~MainWindow()
{
    for (HoButton *btn : this->tabVec) {
        delete btn;
    }
    delete this->connListWidget;
    delete this->connDialog;
    delete ui;
}

bool MainWindow::refreshList() {
    QFile file(QApplication::applicationDirPath() + "/connect.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "系统错误", "无法访问文件connect.txt");
        return false;
    }
    QByteArray ba = file.readAll();
    file.close();
    if (ba.isEmpty()) {
        return true;
    }
    QJsonDocument doc = this->parseJson(ba);
    if (!doc.isNull()) {
        this->configureList = this->parseJsonDocument(doc);
        for (const DatabaseConfigure &dc : this->configureList) {
            this->connListWidget->addItem(
                dc.serverType + "://" +
                dc.serverIP + ":" +
                dc.serverPort + "/" +
                dc.userName);
        }
    }
    return true;
}

QJsonDocument MainWindow::parseJson(const QByteArray &jsonData) {
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        exit(0);
    }

    return document;
}

QList<DatabaseConfigure> MainWindow::parseJsonDocument(const QJsonDocument &document) {
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
        dc.serverType = o.value("type").toString();
        dc.serverIP = o.value("ip").toString();
        dc.serverPort = o.value("port").toString();
        dc.userName = o.value("username").toString();
        dc.userPassword = o.value("password").toString();
        list.append(dc);
    }
    return list;
}

void MainWindow::onTabMouseEnter(int id) {
    this->tabVec[id]->setTextColor(QColor(255, 255, 255));
    this->tabVec[id]->setBackgroundColor(QBrush(QColor(0, 255, 0)));
    this->tabVec[id]->update();
}

void MainWindow::onTabMouseLeave(int id) {
    if (this->tab != id) {
        this->tabVec[id]->setTextColor(QColor(0, 0, 0));
        this->tabVec[id]->setBackgroundColor(QBrush(QColor(255, 255, 255)));
        this->tabVec[id]->update();
    }
}

void MainWindow::onConnectTabClicked(int id) {
    Q_UNUSED(id);
    this->connDialog->exec();
    this->connListWidget->clear();
    this->refreshList();
}

void MainWindow::resizeEvent(QResizeEvent *) {
    this->connListWidget->setGeometry(0, 100, this->width() * 0.3, this->height() - 100);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        QMessageBox::StandardButton result =
            QMessageBox::question(this, "确定吗？", "确定要删除 " + this->connListWidget->currentItem()->text() + " ？");
        if (result == QMessageBox::Yes) {
            this->configureList.removeAt(this->connListWidget->currentRow());
            QJsonObject o;
            QJsonArray all;
            for (const DatabaseConfigure &dc : this->configureList) {
                QJsonObject item_o;
                item_o["type"] = dc.serverType;
                item_o["ip"] = dc.serverIP;
                item_o["port"] = dc.serverPort;
                item_o["username"] = dc.userName;
                item_o["password"] = dc.userPassword;
                all.append(item_o);
            }
            o["all"] = all;
            QJsonDocument doc(o);
            this->writeJsonToFile(QApplication::applicationDirPath() + "/connect.txt", doc);
            this->connListWidget->clear();
            this->refreshList();
        }
    }
}

void MainWindow::writeJsonToFile(const QString &fileName, const QJsonDocument &document) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
}
