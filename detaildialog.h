#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>

namespace Ui {
class detailDialog;
}

class detailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit detailDialog(QWidget *parent = nullptr);
    ~detailDialog();

    void getSelectData(const QModelIndex &index);

private:
    Ui::detailDialog *ui;
};

#endif // DETAILDIALOG_H
