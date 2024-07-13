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
    this->tabVec[0]->setText("连接");
    this->tabVec[0]->setTextFont(QFont("", 25));
    this->tabVec[0]->setTextColor(QColor(255, 255, 255));
    this->tabVec[0]->setBackgroundColor(QBrush(QColor(0, 255, 0)));
    this->tabVec[0]->setTextColor(QColor(0, 0, 0));
    this->tabVec[0]->setBackgroundColor(QBrush(QColor(255, 255, 255)));
    this->tabVec[0]->setPosSize(0, 0, 100, 100);
    this->tabVec[0]->setRadius(20);
    this->tabVec[0]->show();

    this->connect(this->tabVec[0], &HoButton::onClick, this, &MainWindow::onConnectTabClicked);

    this->tabVec.append(new HoButton(this));
    this->tabVec[1]->setId(1);
    this->tabVec[1]->setText("表");
    this->tabVec[1]->setTextFont(QFont("", 25));
    this->tabVec[1]->setTextColor(QColor(255, 255, 255));
    this->tabVec[1]->setBackgroundColor(QBrush(QColor(0, 255, 0)));
    this->tabVec[1]->setPosSize(100, 0, 100, 100);
    this->tabVec[1]->setRadius(20);
    this->tabVec[1]->show();

    this->tabVec.append(new HoButton(this));
    this->tabVec[2]->setId(2);
    this->tabVec[2]->setText("SQL");
    this->tabVec[2]->setTextFont(QFont("", 25));
    this->tabVec[2]->setTextColor(QColor(0, 0, 0));
    this->tabVec[2]->setBackgroundColor(QBrush(QColor(255, 255, 255)));
    this->tabVec[2]->setPosSize(200, 0, 100, 100);
    this->tabVec[2]->setRadius(20);
    this->tabVec[2]->show();

    for (HoButton *btn : this->tabVec) {
        this->connect(btn, &HoButton::onMouseEnter, this, &MainWindow::onTabMouseEnter);
        this->connect(btn, &HoButton::onMouseLeave, this, &MainWindow::onTabMouseLeave);
    }

    this->connListWidget = new QListWidget(this);
    QFile _file(QApplication::applicationDirPath() + "/connect.txt");
    if (_file.open(QIODevice::Append)) {
        _file.close();
    }
    if (!this->refreshList()) {
        exit(0);
    }
    this->connListWidget->setGeometry(0, 100, this->width() * 0.3, this->height() - 100);
    this->connListWidget->show();
    this->connect(this->connListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onConnectListDoubleClicked);

    this->tableListWidget = new QListWidget(this);
    this->tableListWidget->setGeometry(this->width() * 0.3, 100, this->width() * 0.2, this->height() - 100);
    this->tableListWidget->show();
    this->connect(this->tableListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onTableListDoubleClicked);

    this->connDialog = new ConnectDialog(this);

    this->tableWidget = new QTableWidget(this);
    this->tableWidget->setGeometry(this->width() * 0.5, 100, this->width() * 0.5, this->height() - 100);
    this->tableWidget->show();
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
                dc.databaseName + "/" +
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
        return QJsonDocument();
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
        dc.databaseName = o.value("database").toString();
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

void MainWindow::onConnectListDoubleClicked(QListWidgetItem *item) {
    Q_UNUSED(item);
    int row = this->connListWidget->currentRow();
    DatabaseConfigure dc = this->configureList.at(row);
    if (dc.serverIP != this->currentDatabase.serverIP ||
        dc.serverPort != this->currentDatabase.serverPort ||
        dc.serverType != this->currentDatabase.serverType ||
        dc.databaseName != this->currentDatabase.databaseName ||
        dc.userName != this->currentDatabase.userName) {
        this->currentDatabase = dc;
        QSqlDatabase::removeDatabase("current");
        if (dc.serverType == "MySQL") {
            this->db = QSqlDatabase::addDatabase("QMYSQL", "current");
            this->db.setHostName(dc.serverIP);
            this->db.setPort(dc.serverPort.toInt());
            this->db.setDatabaseName(dc.databaseName);
        } else if (dc.serverType == "SQLite3") {
            this->db = QSqlDatabase::addDatabase("QSQLITE", "current");
            this->db.setDatabaseName(dc.databaseName);
            this->tableListWidget->clear();
            this->tableListWidget->addItem(dc.databaseName);
        } else {
            QMessageBox::warning(this, "系统警告", "暂不支持该数据库驱动");
            return;
        }
        if (!dc.userName.isEmpty()) {
            if (!this->db.open(dc.userName, dc.userPassword)) {
                QMessageBox::critical(this, "连接失败", this->db.lastError().text());
                return;
            }
        } else {
            if (!this->db.open()) {
                QMessageBox::critical(this, "连接失败", this->db.lastError().text());
                return;
            }
        }
        if (dc.serverType == "MySQL") {
            this->query = QSqlQuery(this->db);
            if (this->query.exec("SHOW TABLES;")) {
                this->tableListWidget->clear();
                while (this->query.next()) {
                    this->tableListWidget->addItem(this->query.value(0).toString());
                }
            }
        } else if (dc.serverType == "SQLite3") {
            this->query = QSqlQuery(this->db);
            if (this->query.exec("SELECT `name` FROM `sqlite_master` WHERE `type` = 'table'")) {
                this->tableListWidget->clear();
                while (this->query.next()) {
                    this->tableListWidget->addItem(this->query.value(0).toString());
                }
            }
        }
        QSqlDatabase::removeDatabase("current");
    }
}

void MainWindow::onTableListDoubleClicked(QListWidgetItem *item) {
    Q_UNUSED(item);
    QSqlDatabase::removeDatabase("current");
    if (this->currentDatabase.serverType == "MySQL") {
        this->db = QSqlDatabase::addDatabase("QMYSQL", "current");
    } else if (this->currentDatabase.serverType == "SQLite3") {
        this->db = QSqlDatabase::addDatabase("QSQLITE", "current");
    } else {
        QMessageBox::warning(this, "系统警告", "暂不支持该数据库驱动");
        return;
    }
    this->db.setHostName(this->currentDatabase.serverIP);
    this->db.setPort(this->currentDatabase.serverPort.toInt());
    this->db.setDatabaseName(this->currentDatabase.databaseName);
    if (!this->currentDatabase.userName.isEmpty()) {
        if (!this->db.open(this->currentDatabase.userName, this->currentDatabase.userPassword)) {
            QMessageBox::critical(this, "连接失败", this->db.lastError().text());
            return;
        }
    } else {
        if (!this->db.open()) {
            QMessageBox::critical(this, "连接失败", this->db.lastError().text());
            return;
        }
    }
    this->query = QSqlQuery(this->db);
    QString getColumnSql = "";
    if (this->currentDatabase.serverType == "MySQL") {
        getColumnSql = QString("SELECT `COLUMN_NAME` FROM `INFORMATION_SCHEMA`.`COLUMNS`") +
                       "WHERE `TABLE_NAME` = '" + item->text() + "' AND `TABLE_SCHEMA` = '" +
                       this->currentDatabase.databaseName + "';";
    } else if (this->currentDatabase.serverType == "SQLite3") {
        getColumnSql = "SELECT `name` FROM pragma_table_info('" + item->text() + "');";
    }
    if (this->query.exec(getColumnSql)) {
        this->tableWidget->clear();
        this->tableWidget->setColumnCount(0);
        int column = 0;
        QStringList columnList;
        while (this->query.next()) {
            QString columnName = this->query.value(0).toString();
            columnList << columnName;
            this->tableWidget->insertColumn(column);
            QTableWidgetItem *titem = new QTableWidgetItem(columnName);
            this->tableWidget->setHorizontalHeaderItem(column, titem);
            ++column;
        }
        if (columnList.length() > 0) {
            QString sql = "SELECT ";
            bool isFirst = true;
            for (const QString &columnName : columnList) {
                if (isFirst) {
                    sql += "`" + columnName + "`";
                    isFirst = false;
                } else {
                    sql += ", `" + columnName + "`";
                }
            }
            sql += " FROM `" + item->text() + "` LIMIT 20;";
            if (this->query.exec(sql)) {
                int row = 0;
                this->tableWidget->setRowCount(0);
                while (this->query.next()) {
                    int col = 0;
                    this->tableWidget->insertRow(row);
                    for (const QString &colName : columnList) {
                        this->tableWidget->setItem(row, col, new QTableWidgetItem(this->query.value(colName).toString()));
                        ++col;
                    }
                }
            } else {
                qCritical() << this->db.lastError() << sql;
                QMessageBox::critical(this, "执行失败", sql);
                return;
            }
        }
    } else {
        qCritical() << this->db.lastError() << getColumnSql;
        QMessageBox::critical(this, "执行失败", getColumnSql);
        return;
    }
}

void MainWindow::resizeEvent(QResizeEvent *) {
    this->connListWidget->setGeometry(0, 100, this->width() * 0.3, this->height() - 100);
    this->tableListWidget->setGeometry(this->width() * 0.3, 100, this->width() * 0.2, this->height() - 100);
    this->tableWidget->setGeometry(this->width() * 0.5, 100, this->width() * 0.5, this->height() - 100);
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
                item_o["database"] = dc.databaseName;
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
