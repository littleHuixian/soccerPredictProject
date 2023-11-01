#ifndef SCOREDETAILDIALOG_H
#define SCOREDETAILDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>

namespace Ui {
class scoreDetailDialog;
}

class scoreDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit scoreDetailDialog(QWidget *parent = nullptr);
    ~scoreDetailDialog();

    void getSelectData(const QModelIndex &index);
signals:
    void windowClosed();

protected:
    void initTable();
    void loadDatabase(QTableWidget *tWidget, QString a);
    void updateHomeGoal(QString cIndex, int cGoal, QString teamA, QString teamB);
    void updateAwayGoal(QString cIndex, int cGoal, QString teamA, QString teamB);
    QString updateLastStatus(QString team, QString resultType);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnUpdate_clicked();

private:
    Ui::scoreDetailDialog *ui;

    //QSqlQueryModel *getSqlModel;
    QString currentState;
};

#endif // SCOREDETAILDIALOG_H
