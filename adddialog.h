#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

    void getSelectIndex(int a, int b);

signals:
    void sendAddInfo(QStringList sendStr);

private slots:
    void on_btnCertain_clicked();

    void onCalendarClicked(const QDate& date);
    void onIndexChanged(int index);

private:
    Ui::AddDialog *ui;
};

#endif // ADDDIALOG_H
