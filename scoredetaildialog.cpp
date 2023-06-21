#include "scoredetaildialog.h"
#include "ui_scoredetaildialog.h"

extern QSqlDatabase database;
extern QSqlQueryModel *tableModel;
extern QStringList pointTableLists;
extern int currentLeague, currentRound;
extern QStringList premierTeam, laligaTeam, serieaTeam, bundesligaTeam, ligueTeam;

#include <QDebug>

scoreDetailDialog::scoreDetailDialog(QWidget *parent) : QDialog(parent), ui(new Ui::scoreDetailDialog)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());

    initTable();

}

scoreDetailDialog::~scoreDetailDialog()
{
    delete ui;
}

void scoreDetailDialog::getSelectData(const QModelIndex &index)
{
    //qDebug()<<index;
    int selectRow = index.row();
//    QModelIndex indexPosition0 = sqlModel.index(selectRow, 0);
//    ui->lineEdit0->setText(sqlModel.data(indexPosition0, 0).toString());
    QModelIndex homeIndex = tableModel->index(selectRow, 2);
    QModelIndex awayIndex = tableModel->index(selectRow, 3);

    homeTeam = tableModel->data(homeIndex, 0).toString();
    awayTeam = tableModel->data(awayIndex, 0).toString();
    //qDebug()<<"主队："<<homeTeam<<"客队："<<awayTeam;
    ui->lineEditHome->setText(homeTeam);
    ui->lineEditAway->setText(awayTeam);

    loadDatabase( ui->tableWidget1, homeTeam );
    loadDatabase( ui->tableWidget2, awayTeam );

}

void scoreDetailDialog::initTable()
{
    //第一个表格
    ui->tableWidget1->setRowCount(9);
    ui->tableWidget1->setColumnCount(11);
//    ui->tableWidget->horizontalHeader()->setVisible(false);

    QHeaderView *horHeader1 = ui->tableWidget1->horizontalHeader();
    horHeader1->setStyleSheet("QHeaderView::section {background-color: rgb(0, 255, 127)}");

    QStringList horHeadLists1;
    horHeadLists1<<tr("0")<<tr("1")<<tr("2")<<tr("3")<<tr("4")<<tr("5")<<tr("6")<<tr("7")<<tr("8")<<tr("9")<<tr("10");
    ui->tableWidget1->setHorizontalHeaderLabels( horHeadLists1 );

    QHeaderView *verHeader1 = ui->tableWidget1->verticalHeader();
    verHeader1->setStyleSheet("QHeaderView::section {background-color: rgb(0, 255, 127)}");

    //主全：主队全场进球；客全：客队全场；主主：主队主场；主失：主队失球；
    QStringList verHeadLists1;
    verHeadLists1<<tr("主全进")<<tr("主全失")<<tr("主主进")<<tr("主主失")<<tr("主客进")<<tr("主客失")<<tr("全战绩")<<tr("主战绩")<<tr("主战绩");

    ui->tableWidget1->setVerticalHeaderLabels(verHeadLists1);

    ui->tableWidget1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //使列完全填充并平分
    ui->tableWidget1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget1->setEditTriggers( QAbstractItemView::NoEditTriggers );

    //第二个表格
    ui->tableWidget2->setRowCount(9);
    ui->tableWidget2->setColumnCount(11);
    //    ui->tableWidget->horizontalHeader()->setVisible(false);

    QHeaderView *horHeader2 = ui->tableWidget2->horizontalHeader();
    horHeader2->setStyleSheet("QHeaderView::section {background-color: rgb(0, 255, 127)}");

    QStringList horHeadLists2;
    horHeadLists2<<tr("0")<<tr("1")<<tr("2")<<tr("3")<<tr("4")<<tr("5")<<tr("6")<<tr("7")<<tr("8")<<tr("9")<<tr("10");
    ui->tableWidget2->setHorizontalHeaderLabels( horHeadLists1 );

    QHeaderView *verHeader2 = ui->tableWidget2->verticalHeader();
    verHeader2->setStyleSheet("QHeaderView::section {background-color: rgb(0, 255, 127)}");

    //主全：主队全场进球；客全：客队全场；主主：主队主场；主失：主队失球；
    QStringList verHeadLists2;
    verHeadLists2<<tr("客全进")<<tr("客全失")<<tr("客主进")<<tr("客主失")<<tr("客客进")<<tr("客客失")<<tr("全战绩")<<tr("客战绩")<<tr("客战绩");

    ui->tableWidget2->setVerticalHeaderLabels(verHeadLists2);

    ui->tableWidget2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 使列完全填充并平分
    ui->tableWidget2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget2->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void scoreDetailDialog::loadDatabase(QTableWidget *tWidget, QString a)
{
    //加载主客场数据
    QSqlQuery query;

    for (int i = 0; i < pointTableLists.length(); i++) {
        QString str = QString( "SELECT zero, one, two, three, four, five, six, seven, eight, nine, ten FROM %1 WHERE round='%2' AND team='%3'"  )
                .arg( pointTableLists.at(i) ).arg( currentRound+1 ).arg( a );

        query.exec( str );

        while ( query.next() ) {
            //int zero = query.value(0).toInt();
            int zero = query.value("zero").toInt();
            int one = query.value("one").toInt();
            int two = query.value("two").toInt();
            int three = query.value("three").toInt();
            int four = query.value("four").toInt();
            int five = query.value("five").toInt();
            int six = query.value("six").toInt();
            int seven = query.value("seven").toInt();
            int eight = query.value("eight").toInt();
            int nine = query.value("nine").toInt();
            int ten = query.value("ten").toInt();

            QTableWidgetItem *item0 = new QTableWidgetItem();
            item0->setText( QString("%1").arg( zero ));
            tWidget->setItem(i, 0, item0);

            QTableWidgetItem *item1 = new QTableWidgetItem();
            item1->setText( QString("%1").arg( one ));
            tWidget->setItem(i, 1, item1);

            QTableWidgetItem *item2 = new QTableWidgetItem();
            item2->setText( QString("%1").arg( two ));
            tWidget->setItem(i, 2, item2);

            QTableWidgetItem *item3 = new QTableWidgetItem();
            item3->setText( QString("%1").arg( three ));
            tWidget->setItem(i, 3, item3);

            QTableWidgetItem *item4 = new QTableWidgetItem();
            item4->setText( QString("%1").arg( four ));
            tWidget->setItem(i, 4, item4);

            QTableWidgetItem *item5 = new QTableWidgetItem();
            item5->setText( QString("%1").arg( five ));
            tWidget->setItem(i, 5, item5);

            QTableWidgetItem *item6 = new QTableWidgetItem();
            item6->setText( QString("%1").arg( six ));
            tWidget->setItem(i, 6, item6);

            QTableWidgetItem *item7 = new QTableWidgetItem();
            item7->setText( QString("%1").arg( seven ));
            tWidget->setItem(i, 7, item7);

            QTableWidgetItem *item8 = new QTableWidgetItem();
            item8->setText( QString("%1").arg( eight ));
            tWidget->setItem(i, 8, item8);

            QTableWidgetItem *item9 = new QTableWidgetItem();
            item9->setText( QString("%1").arg( nine ));
            tWidget->setItem(i, 9, item9);

            QTableWidgetItem *item10 = new QTableWidgetItem();
            item10->setText( QString("%1").arg( ten ));
            tWidget->setItem(i, 10, item10);
        }
    }

    //加载主客场数据
    QString pointStr = QString( "SELECT allWin, allDraw, allLoss, allGoal, allLosses, allPoint, "
                                "homeWin, homeDraw, homeLoss, homeGoal, homeLosses, homePoint,"
                                "awayWin, awayDraw, awayLoss, awayGoal, awayLosses, awayPoint FROM allPointTable WHERE round='%1' AND team='%2'"  )
            .arg( currentRound+1 ).arg( a );

    query.exec( pointStr );

    while ( query.next() ) {
        int allWin = query.value(0).toInt();
        int allDraw = query.value(1).toInt();
        int allLoss = query.value(2).toInt();
        int allGoal = query.value(3).toInt();
        int allLosses = query.value(4).toInt();
        int allPoint = query.value(5).toInt();
        int homeWin = query.value(6).toInt();
        int homeDraw = query.value(7).toInt();
        int homeLoss = query.value(8).toInt();
        int homeGoal =query.value(9).toInt();
        int homeLosses = query.value(10).toInt();
        int homePoint = query.value(11).toInt();
        int awayWin =query.value(12).toInt();
        int awayDraw = query.value(13).toInt();
        int awayLoss = query.value(14).toInt();
        int awayGoal = query.value(15).toInt();
        int awayLosses = query.value(16).toInt();
        int awayPoint = query.value(17).toInt();
        //---------------------全战绩--------------------------
        QTableWidgetItem *item0 = new QTableWidgetItem();
        item0->setText( QString("%1").arg( allWin ));
        tWidget->setItem(6, 5, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setText( QString("%1").arg( allDraw ));
        tWidget->setItem(6, 6, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setText( QString("%1").arg( allLoss ));
        tWidget->setItem(6, 7, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem();
        item3->setText( QString("%1").arg( allGoal ));
        tWidget->setItem(6, 8, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem();
        item4->setText( QString("%1").arg( allLosses ));
        tWidget->setItem(6, 9, item4);

        QTableWidgetItem *item5 = new QTableWidgetItem();
        item5->setText( QString("%1").arg( allPoint ));
        tWidget->setItem(6, 10, item5);
        //---------------------主战绩--------------------------
        QTableWidgetItem *item10 = new QTableWidgetItem();
        item10->setText( QString("%1").arg( homeWin ));
        tWidget->setItem(7, 5, item10);

        QTableWidgetItem *item11 = new QTableWidgetItem();
        item11->setText( QString("%1").arg( homeDraw ));
        tWidget->setItem(7, 6, item11);

        QTableWidgetItem *item12 = new QTableWidgetItem();
        item12->setText( QString("%1").arg( homeLoss ));
        tWidget->setItem(7, 7, item12);

        QTableWidgetItem *item13 = new QTableWidgetItem();
        item13->setText( QString("%1").arg( homeGoal ));
        tWidget->setItem(7, 8, item13);

        QTableWidgetItem *item14 = new QTableWidgetItem();
        item14->setText( QString("%1").arg( homeLosses ));
        tWidget->setItem(7, 9, item14);

        QTableWidgetItem *item15 = new QTableWidgetItem();
        item15->setText( QString("%1").arg( homePoint ));
        tWidget->setItem(7, 10, item15);
        //---------------------客战绩--------------------------
        QTableWidgetItem *item20 = new QTableWidgetItem();
        item20->setText( QString("%1").arg( awayWin ));
        tWidget->setItem(8, 5, item20);

        QTableWidgetItem *item21 = new QTableWidgetItem();
        item21->setText( QString("%1").arg( awayDraw ));
        tWidget->setItem(8, 6, item21);

        QTableWidgetItem *item22 = new QTableWidgetItem();
        item22->setText( QString("%1").arg( awayLoss ));
        tWidget->setItem(8, 7, item22);

        QTableWidgetItem *item23 = new QTableWidgetItem();
        item23->setText( QString("%1").arg( awayGoal ));
        tWidget->setItem(8, 8, item23);

        QTableWidgetItem *item24 = new QTableWidgetItem();
        item24->setText( QString("%1").arg( awayLosses ));
        tWidget->setItem(8, 9, item24);

        QTableWidgetItem *item25 = new QTableWidgetItem();
        item25->setText( QString("%1").arg( awayPoint ));
        tWidget->setItem(8, 10, item25);

    }

}


