#include "adddialog.h"
#include "ui_adddialog.h"

#include <QDebug>

extern int currentLeague, currentRound;
extern QStringList premierTeam, laligaTeam, serieaTeam, bundesligaTeam, ligueTeam;

AddDialog::AddDialog(QWidget *parent) :  QDialog(parent),  ui(new Ui::AddDialog)
{
    ui->setupUi(this);

    ui->lineEditDate->setEnabled(false);

    QDate selectDate = ui->calendarWidget->selectedDate();
    QString dateString = selectDate.toString("yyyy-MM-dd");
    ui->lineEditDate->setText( dateString );

    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &AddDialog::onCalendarClicked);

//    connect(ui->cbHome, &ComboBox::currentIndexChanged, this, &AddDialog::onIndexChanged);
//    connect(ui->cbHome, SIGNAL(currenIndexChanged(int)), this, SLOT(onIndexChanged(int)));
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::getSelectIndex(int a, int b)
{
    //qDebug()<<"getSelectIndex a, b"<<a<<b;

    if( a == 0 ){
        ui->cbHome->clear();
        ui->cbAway->clear();
        ui->cbHome->addItems(premierTeam);
        ui->cbAway->addItems(premierTeam);

    } else if ( a == 1 ) {
        ui->cbHome->clear();
        ui->cbAway->clear();
        ui->cbHome->addItems(laligaTeam);
        ui->cbAway->addItems(laligaTeam);

    } else if ( a == 2 ) {
        ui->cbHome->clear();
        ui->cbAway->clear();
        ui->cbHome->addItems(serieaTeam);
        ui->cbAway->addItems(serieaTeam);

    } else if ( a == 3 ) {
        ui->cbHome->clear();
        ui->cbAway->clear();
        ui->cbHome->addItems(bundesligaTeam);
        ui->cbAway->addItems(bundesligaTeam);

    } else {
        ui->cbHome->clear();
        ui->cbAway->clear();
        ui->cbHome->addItems(ligueTeam);
        ui->cbAway->addItems(ligueTeam);

    }
}

//确定按钮
void AddDialog::on_btnCertain_clicked()
{
    QString dateStr = ui->lineEditDate->text();

    QTime selectTime = ui->timeEdit->time();
    QString timeStr = selectTime.toString("HH:mm");

    QString sendDate = QString( "%1 %2" ).arg( dateStr ).arg( timeStr );

    QString homeTeam = ui->cbHome->currentText();
    QString awayTeam = ui->cbAway->currentText();
    QStringList sendStr;
    sendStr <<sendDate << homeTeam << awayTeam;

    emit sendAddInfo( sendStr );
}

void AddDialog::onCalendarClicked(const QDate &date)
{
    ui->lineEditDate->setText( date.toString("yyyy-MM-dd") );
}

void AddDialog::onIndexChanged(int index)
{
//    if (index == 0){
//        ui->cbAway->
//    }
}

