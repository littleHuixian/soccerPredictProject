#include "trenddialog.h"
#include "ui_trenddialog.h"

trendDialog::trendDialog(QWidget *parent) :  QDialog(parent),  ui(new Ui::trendDialog)
{
    ui->setupUi(this);


}

trendDialog::~trendDialog()
{
    delete ui;
}
