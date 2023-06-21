#include "detaildialog.h"
#include "ui_detaildialog.h"


extern int currentLeague, currentRound;

detailDialog::detailDialog(QWidget *parent) : QDialog(parent), ui(new Ui::detailDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    ui->tableWidget->setRowCount(19);
    ui->tableWidget->setColumnCount(11);
//    ui->tableWidget->horizontalHeader()->setVisible(false);//

    QHeaderView *horHeader = ui->tableWidget->horizontalHeader();
    horHeader->setStyleSheet("QHeaderView::section {background-color: rgb(0, 255, 127)}");

    QStringList horHeadLists;
    horHeadLists<<tr("0")<<tr("1")<<tr("2")<<tr("3")<<tr("4")<<tr("5")<<tr("6")<<tr("7")<<tr("8")<<tr("9")<<tr("10");
    ui->tableWidget->setHorizontalHeaderLabels( horHeadLists );

    QHeaderView *verHeader = ui->tableWidget->verticalHeader();
    verHeader->setStyleSheet("QHeaderView::section {background-color: rgb(0, 255, 127)}");

    //主全：主队全场进球；客全：客队全场；主主：主队主场；主失：主队失球；
    QStringList verHeadLists;
    verHeadLists<<tr("主全进")<<tr("主全失")<<tr("客全进")<<tr("客全失")<<tr("主战绩")<<tr("客战绩")
               <<tr("主主进")<<tr("主主失")<<tr("客主进")<<tr("客主失")<<tr("战绩")<<tr("战绩")
              <<tr("主客进")<<tr("主客失")<<tr("客客进")<<tr("客客失")<<tr("战绩")<<tr("战绩")<<tr("预 测");
    ui->tableWidget->setVerticalHeaderLabels(verHeadLists);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //使列完全填充并平分
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        for (int j = 0; j < ui->tableWidget->columnCount(); j++ ) {
            if ( i < 6 ) {
                for (int j = 0; j < ui->tableWidget->columnCount(); j++ ) {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setData( Qt::BackgroundColorRole, QColor(255, 170, 0) );
                    ui->tableWidget->setItem(i, j, item);
                }

            } else if ( i < 12 ) {
                for (int j = 0; j < ui->tableWidget->columnCount(); j++ ) {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setData( Qt::BackgroundColorRole, QColor(170, 170, 255) );
                    ui->tableWidget->setItem(i, j, item);
                }

            } else if ( i < ui->tableWidget->rowCount() - 1 ){
                for (int j = 0; j < ui->tableWidget->columnCount(); j++ ) {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setData( Qt::BackgroundColorRole, QColor(85, 170, 255) );
                    ui->tableWidget->setItem(i, j, item);
                }

            } else {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setData( Qt::BackgroundColorRole, QColor(255, 85, 127) );
                ui->tableWidget->setItem(i, j, item);
            }

        }
    }

    //setSpan实现表格单元格的合并，起始行、起始列、行跨度、列跨度
//    for (int i = 1; i < ui->tableWidget->columnCount(); i++) {
//        QTableWidgetItem *item = new QTableWidgetItem(i);
//        ui->tableWidget->setItem(0, i, item);
//    }

}

detailDialog::~detailDialog()
{
    delete ui;
}

void detailDialog::getSelectData(const QModelIndex &index)
{
//    QTableWidgetItem *item1 = table1->item( index.row(), 1 );
//    QTableWidgetItem *item2 = table1->item( index.row(), 2 );

}

