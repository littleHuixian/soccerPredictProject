#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QSettings>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QStandardItemModel>

#include "detaildialog.h"
#include "adddialog.h"
#include "trenddialog.h"
#include "scoredetaildialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadConfig();

    void creatDatabase();
    void initMain();
    void initTable();
    void initAgainstTableview();
    void modelToTable( QSqlQueryModel *qryModel );
    void loadLeagueTable();

    void insertTable(QString leagueTable, QStringList teamList);
    void creatPointTable(QString a, QStringList teams);

    void receiverAddInfo(QStringList data);
    void updateTable();

//signals:
//    void sendAddGames(int selectIndex, QString team1, QString team2);

private slots:
    void on_btnAdd_clicked();
    void showDetailDialog();

    void on_cbGames_currentIndexChanged(int index);

    void on_cbRounds_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QLabel *infoLabel;

    QString configFilePath;
    bool insertFlag;

//    QSqlDatabase database;
    //QStringList leagueAgainstList;

//    QStringList pointTableLists;

    detailDialog *detailWin;
    scoreDetailDialog *detailDialog;
    AddDialog *addWin;



};
#endif // MAINWINDOW_H
