#ifndef SCOREDETAILDIALOG_H
#define SCOREDETAILDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

namespace Ui {
class scoreDetailDialog;
}

class scoreDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit scoreDetailDialog(QWidget *parent = nullptr);
    ~scoreDetailDialog();


//signals:
    void getSelectData(const QModelIndex &index);

protected:
    void initTable();

    void loadDatabase(QTableWidget *tWidget, QString a);

private:
    Ui::scoreDetailDialog *ui;

    QSqlQueryModel *getSqlModel;
    QString homeTeam, awayTeam;
};

#endif // SCOREDETAILDIALOG_H
