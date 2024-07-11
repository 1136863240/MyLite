#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QListWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStringList>
#include <QKeyEvent>
#include <QList>
#include "hobutton.h"
#include "connectdialog.h"

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

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onTabMouseEnter(int id);
    void onTabMouseLeave(int id);
    void onConnectTabClicked(int id);

private:
    QJsonDocument parseJson(const QByteArray &jsonData);
    QList<DatabaseConfigure> parseJsonDocument(const QJsonDocument &document);
    void writeJsonToFile(const QString &fileName, const QJsonDocument &document);
    bool refreshList();
    Ui::MainWindow *ui;
    int tab;
    QVector<HoButton*> tabVec;
    QList<DatabaseConfigure> configureList;
    QListWidget *connListWidget;
    ConnectDialog *connDialog;
};
#endif // MAINWINDOW_H
