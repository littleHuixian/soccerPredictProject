#ifndef TRENDDIALOG_H
#define TRENDDIALOG_H

#include <QDialog>

namespace Ui {
class trendDialog;
}

class trendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit trendDialog(QWidget *parent = nullptr);
    ~trendDialog();

private:
    Ui::trendDialog *ui;
};

#endif // TRENDDIALOG_H
