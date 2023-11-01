#include "scoredetaildialog.h"
#include "ui_scoredetaildialog.h"

extern QSqlDatabase database;
extern QSqlQueryModel *tableModel;
extern QStringList pointTableLists;
extern QStringList teamData;
extern int currentLeague, currentRound;
extern QStringList leagueAgainstList;
extern QStringList premierTeam, laligaTeam, serieaTeam, bundesligaTeam, ligueTeam;

#include <QDebug>

scoreDetailDialog::scoreDetailDialog(QWidget *parent) : QDialog(parent), ui(new Ui::scoreDetailDialog)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());
    ui->lineEditTime->setEnabled(false);
    ui->homeGoal->clear();
    ui->awayGoal->clear();
    ui->homeHalfGoal->clear();
    ui->awayHalfGoal->clear();

    initTable();
}

scoreDetailDialog::~scoreDetailDialog()
{
    delete ui;
}

void scoreDetailDialog::getSelectData(const QModelIndex &index)
{
    ui->comboBox1->setCurrentIndex(0);
    ui->comboBox2->setCurrentIndex(0);
    ui->homeGoal->clear();
    ui->awayGoal->clear();
    ui->homeHalfGoal->clear();
    ui->awayHalfGoal->clear();

    int selectRow = index.row();

    QModelIndex timeIndex = tableModel->index(selectRow, 1);
    QModelIndex homeIndex = tableModel->index(selectRow, 2);
    QModelIndex awayIndex = tableModel->index(selectRow, 3);
    QModelIndex statusIndex = tableModel->index( selectRow, 9 );

    QModelIndex allIndex = tableModel->index( selectRow, 7 );
    QModelIndex halfAllIndex = tableModel->index( selectRow, 8 );
    QString predictAll = tableModel->data( allIndex, 0 ).toString();
    QString predictHalfAll = tableModel->data( halfAllIndex, 0 ).toString();

    if( predictAll == tr("未预测") ) {
        ui->comboBox1->setCurrentIndex( 0 );
    } else if ( predictAll == tr("主胜") ) {
        ui->comboBox1->setCurrentIndex( 1 );
    } else if (predictAll == tr("平局")) {
        ui->comboBox1->setCurrentIndex( 2 );
    } else {
        ui->comboBox1->setCurrentIndex( 3 );
    }

    if( predictHalfAll == tr("未预测") ) {
        ui->comboBox2->setCurrentIndex( 0 );
    } else if (predictHalfAll == tr("胜胜")) {
        ui->comboBox2->setCurrentIndex( 1 );
    } else if (predictHalfAll == tr("胜平")) {
        ui->comboBox2->setCurrentIndex( 2 );
    } else if (predictHalfAll == tr("胜负")) {
        ui->comboBox2->setCurrentIndex( 3 );
    } else if (predictHalfAll == tr("平胜")) {
        ui->comboBox2->setCurrentIndex( 4 );
    } else if (predictHalfAll == tr("平平")) {
        ui->comboBox2->setCurrentIndex( 5 );
    } else if (predictHalfAll == tr("平负")) {
        ui->comboBox2->setCurrentIndex( 6 );
    } else if (predictHalfAll == tr("负胜")) {
        ui->comboBox2->setCurrentIndex( 7 );
    } else if (predictHalfAll == tr("负平")) {
        ui->comboBox2->setCurrentIndex( 8 );
    } else if (predictHalfAll == tr("负负")) {
        ui->comboBox2->setCurrentIndex( 9 );
    }
    //已预测状态，则更新下拉框 【未预测，已预测，已更新】
    currentState = tableModel->data( statusIndex, 0 ).toString();
     if (currentState == tr("已更新")) {
         ui->btnUpdate->setEnabled( false );
     } else {
         ui->btnUpdate->setEnabled( true );
     }

    QDateTime gameTime = tableModel->data( timeIndex, 0 ).toDateTime();
    //获取当前时间，判断是否已经完赛
    QDateTime currentTime = QDateTime::currentDateTime();
    //QString currentStr = currentTime.toString("yyyy-MM-dd hh:mm");
    if (gameTime > currentTime){
        //未开赛
        ui->lineEditTime->setText( gameTime.toString("yyyy-MM-dd hh:mm") );
    } else {
        //已开赛
        ui->lineEditTime->setText( tr("已完赛") );
    }

    QString homeTeam = tableModel->data(homeIndex, 0).toString();
    QString awayTeam = tableModel->data(awayIndex, 0).toString();
    ui->labelHome->setText( homeTeam );
    ui->labelAway->setText( awayTeam );

    loadDatabase( ui->tableWidget1, homeTeam );
    loadDatabase( ui->tableWidget2, awayTeam );

    QSqlQuery query;
    QString lastHome = QString( "SELECT lastStatus FROM %1 WHERE team='%2'"  )
            .arg(teamData.at(currentLeague)).arg( homeTeam );

    query.exec( lastHome );
    while ( query.next() ) {
        QString hStatus = query.value(0).toString();
        ui->lineEditHlast1->setText(hStatus);
    }

    QString lastAway = QString( "SELECT lastStatus FROM %1 WHERE team='%2'"  )
            .arg(teamData.at(currentLeague)).arg( awayTeam );

    query.exec( lastAway );
    while ( query.next() ) {
        QString aStatus = query.value(0).toString();
        ui->lineEditHlast2->setText(aStatus);
    }
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
        QString str = QString( "SELECT zero, one, two, three, four, five, six, seven, eight, nine, ten FROM %1 WHERE team='%2'"  )
                .arg( pointTableLists.at(i) ).arg( a );
        //qDebug()<<str;

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
    QString pointStr = QString( "SELECT wins, draws, losses, goals, againsts, points, "
                                "hWins, hDraws, hLosses, hGoals, hAgainsts, hPoints,"
                                "aWins, aDraws, aLosses, aGoals, aAgainsts, aPoints, lastStatus FROM %1 WHERE team='%2'"  )
            .arg(teamData.at(currentLeague)).arg( a );

    bool success = query.exec( pointStr );

    if(success) {
        //qDebug()<<"pointStr"<<pointStr;
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

}


void scoreDetailDialog::updateHomeGoal(QString cIndex, int cGoal, QString teamA, QString teamB)
{
    //pointTableLists<<"allGoalTable"<<"alllossTable"<<"homeGoalTable"<<"homelossTable"<<"awayGoalTable"<<"awaylossTable"
    QSqlQuery query;
    query.exec( QString("UPDATE allGoalTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamA) );
    query.exec( QString("UPDATE homeGoalTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamA) );
    query.exec( QString("UPDATE alllossTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamB) );
    query.exec( QString("UPDATE awaylossTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamB) );
}

void scoreDetailDialog::updateAwayGoal(QString cIndex, int cGoal, QString teamA, QString teamB)
{
    QSqlQuery query;
    query.exec( QString("UPDATE allGoalTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamA) );
    query.exec( QString("UPDATE awayGoalTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamA) );
    query.exec( QString("UPDATE alllossTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamB) );
    query.exec( QString("UPDATE homelossTable SET %1='%2' WHERE team='%3'").arg(cIndex).arg(cGoal).arg(teamB) );
}

QString scoreDetailDialog::updateLastStatus(QString team, QString resultType)
{
    //查询上轮
    QSqlQuery query;
    QString searchLastString = QString("SELECT lastStatus FROM %1 WHERE round='%2' AND team='%3'")
            .arg(teamData.at(currentLeague)).arg(currentRound).arg(team);
    query.exec( searchLastString );
    QString lastStatus;
    while (query.next()) {
        lastStatus = query.value(0).toString();
    }
    //qDebug()<<"lastStatus:"<<lastStatus<<lastStatus.length();
    QString result;
    result=resultType+lastStatus;
    //qDebug()<<"result"<<result;
    result.remove(5, 1);

    return result;
}

void scoreDetailDialog::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}


void scoreDetailDialog::on_btnUpdate_clicked()
{
    //先获取数据库的预测状态 【未预测，已预测，已更新】
    QString gameTime = ui->lineEditTime->text();
    QString homeTeam = ui->labelHome->text();
    QString awayTeam = ui->labelAway->text();
    QSqlQuery query;
    bool updateState = ui->homeGoal->text().isEmpty()||ui->awayGoal->text().isEmpty()||ui->homeHalfGoal->text().isEmpty()||ui->awayHalfGoal->text().isEmpty();
    //更新预测
    if ( updateState ){
        QString excStr = QString("UPDATE %1 SET predictAll='%2', predictHalfAll='%3',"
                                 "predictStatus='%4' WHERE round='%5' AND hTeam='%6' AND aTeam='%7' ")
                .arg( leagueAgainstList.at( currentLeague ) ).arg(ui->comboBox1->currentText())
                .arg( ui->comboBox2->currentText() ).arg(tr("已预测")).arg( (currentRound+1) ).arg( homeTeam ).arg( awayTeam );

        query.exec( excStr );
    }

    if (currentState != tr("已更新") && !updateState) {
        int homeHalfGoal = ui->homeHalfGoal->text().toInt();
        int awayHalfGoal = ui->awayHalfGoal->text().toInt();
        int homeGoalNum = ui->homeGoal->text().toInt();
        int awayGoalNum = ui->awayGoal->text().toInt();

        QString halfAllStr;
        if(homeHalfGoal > awayHalfGoal){
            if(homeGoalNum>awayGoalNum){
                halfAllStr = tr("胜胜");
            } else if (homeGoalNum==awayGoalNum) {
                halfAllStr = tr("胜平");
            } else {
                halfAllStr = tr("胜负");
            }
        } else if (homeHalfGoal == awayHalfGoal) {
            if(homeGoalNum>awayGoalNum){
                halfAllStr = tr("平胜");
            } else if (homeGoalNum==awayGoalNum) {
                halfAllStr = tr("平平");
            } else {
                halfAllStr = tr("平负");
            }
        } else if (homeHalfGoal<awayHalfGoal) {
            if(homeGoalNum>awayGoalNum){
                halfAllStr = tr("负胜");
            } else if (homeGoalNum==awayGoalNum) {
                halfAllStr = tr("负平");
            } else {
                halfAllStr = tr("负负");
            }
        }

        //对阵表更新
        QString excStr = QString("UPDATE %1 SET halfScore='%2-%3', allScore='%4-%5', halfAll='%6',"
                                 "predictStatus='%7' WHERE round='%8'  AND hTeam='%9' AND aTeam='%10' ")
                .arg( leagueAgainstList.at( currentLeague ) ).arg( ui->homeHalfGoal->text() ).arg( ui->awayHalfGoal->text() )
                .arg( ui->homeGoal->text() ).arg( ui->awayGoal->text() ).arg(halfAllStr).arg(tr("已更新")).arg( (currentRound+1) )
                .arg( homeTeam ).arg( awayTeam );

        query.exec( excStr );

        //积分表更新
        if(homeGoalNum>awayGoalNum){
            qDebug()<<"主 胜"<<homeGoalNum<<awayGoalNum;

            //=======主 胜========
            if(currentRound==0){
                QString updateStr = QString("UPDATE %1 SET wins='1', goals='%2', againsts='%3', points='3',"
                                            "hWins='1', hGoals='%4', hAgainsts='%5', hPoints='3', lastStatus='%6' WHERE team='%7'")
                        .arg(teamData.at(currentLeague)).arg(homeGoalNum).arg(awayGoalNum).arg(homeGoalNum)
                        .arg(awayGoalNum).arg(tr("W----")).arg(homeTeam);
                query.exec(updateStr);

                //=======客 负========
                QString updateAway = QString("UPDATE %1 SET losses='1', goals='%2', againsts='%3',"
                                            "aLosses='1', aGoals='%4', aAgainsts='%5', lastStatus='%6' WHERE team='%7'")
                        .arg(teamData.at(currentLeague)).arg(awayGoalNum).arg(homeGoalNum).arg(awayGoalNum)
                        .arg(homeGoalNum).arg(tr("L----")).arg(awayTeam);
                qDebug()<<updateAway;

                query.exec(updateAway);

            } else {
                QString getHomeResult = updateLastStatus(homeTeam, "W");
                QString getAwayResult = updateLastStatus(awayTeam, "L");

                //先查询上轮数据，再增加到本轮
//                QString pointStr = QString( "SELECT wins, draws, losses, goals, againsts, points, "
//                                            "hWins, hDraws, hLosses, hGoals, hAgainsts, hPoints,"
//                                            "aWins, aDraws, aLosses, aGoals, aAgainsts, aPoints FROM %1 WHERE round='%2' AND team='%3'"  )
//                        .arg(teamData.at(currentLeague)).arg( (currentRound+1) ).arg( a );
//                bool success = query.exec( pointStr );

                QString searchHomeString = QString("SELECT round, wins, goals, againsts, points, hWins, hGoals, hAgainsts, hPoints "
                                                   "FROM %1 WHERE team='%3'")
                        .arg(teamData.at(currentLeague)).arg(homeTeam);
                query.exec( searchHomeString );

                int hR, w, g, a, p, hw, hg, ha, hp;
                while ( query.next() ) {
                    hR = query.value(0).toInt();
                    w = query.value(1).toInt();
                    g = query.value(2).toInt();
                    a = query.value(3).toInt();
                    p = query.value(4).toInt();
                    hw = query.value(5).toInt();
                    hg = query.value(6).toInt();
                    ha = query.value(7).toInt();
                    hp = query.value(8).toInt();

                }

                QString updateHomeWin = QString("UPDATE %1 SET round='%2', wins='%3', goals='%4', againsts='%5', points='%6',"
                                                "hWins='%7', hGoals='%8', hAgainsts='%9', hPoints='%10', lastStatus='%11' WHERE team='%12'")
                        .arg(teamData.at(currentLeague)).arg((hR+1)).arg((w+1)).arg((g+homeGoalNum)).arg((a+awayGoalNum)).arg((p+3)).arg((hw+1)).arg((hg+homeGoalNum))
                        .arg((ha+awayGoalNum)).arg((hp+3)).arg(getHomeResult).arg(homeTeam);
                query.exec(updateHomeWin);

                //=======客 负========
                QString searchAwayString = QString("SELECT round, losses, goals, againsts, aLosses, aGoals, aAgainsts FROM %1 WHERE team='%3'")
                        .arg(teamData.at(currentLeague)).arg(awayTeam);
                query.exec( searchAwayString );

                int aR, al, ag, aa, aLoss, aGoal, aAga;
                while ( query.next() ) {
                    aR = query.value(0).toInt();
                    al = query.value(1).toInt();
                    ag = query.value(2).toInt();
                    aa = query.value(3).toInt();
                    aLoss = query.value(4).toInt();
                    aGoal = query.value(5).toInt();
                    aAga = query.value(6).toInt();

                }

                QString updateAwayStr = QString("UPDATE %1 SET round='%2', losses='%3', goals='%4', againsts='%5',"
                                            "aLosses='%6', aGoals='%7', aAgainsts='%8', lastStatus='%9' WHERE team='%10'")
                        .arg(teamData.at(currentLeague)).arg( (aR+1) ).arg((al+1)).arg((ag+awayGoalNum)).arg((aa+homeGoalNum)).arg((aLoss+1)).arg((aGoal+awayGoalNum))
                        .arg((aAga+homeGoalNum)).arg(getAwayResult).arg(awayTeam);
                qDebug()<<"kefu"<<updateAwayStr;
                query.exec(updateAwayStr);
            }

        } else if (homeGoalNum==awayGoalNum) {
            qDebug()<<"主 平"<<homeGoalNum<<awayGoalNum;
            //主 平局
            if(currentRound==0){
                QString updateStr = QString("UPDATE %1 SET draws='1', goals='%2', againsts='%3', points='1',"
                                            "hDraws='1', hGoals='%4', hAgainsts='%5', hPoints='1', lastStatus='%6' WHERE team='%7'")
                        .arg(teamData.at(currentLeague)).arg(homeGoalNum).arg(awayGoalNum).arg(homeGoalNum)
                        .arg(awayGoalNum).arg(tr("D----")).arg(homeTeam);
                query.exec(updateStr);

                //=======客 平========
                QString updateAway = QString("UPDATE %1 SET draws='1', goals='%2', againsts='%3', points='1',"
                                            "aDraws='1', aGoals='%4', aAgainsts='%5', aPoints='1', lastStatus='%6' WHERE team='%7'")
                        .arg(teamData.at(currentLeague)).arg(awayGoalNum).arg(homeGoalNum).arg(awayGoalNum)
                        .arg(homeGoalNum).arg(tr("D----")).arg(awayTeam);

                query.exec(updateAway);

            } else {
                QString getHomeResult = updateLastStatus(homeTeam, "D");
                QString getAwayResult = updateLastStatus(awayTeam, "D");
                //先查询上轮数据，再增加到本轮
                QString searchString = QString("SELECT round, draws, goals, againsts, points, hDraws, hGoals, hAgainsts, hPoints FROM %1 WHERE team='%2'")
                        .arg(teamData.at(currentLeague)).arg(homeTeam);
                query.exec( searchString );

                int hR, d, g, a, p, hd, hg, ha, hp;
                while ( query.next() ) {
                    hR = query.value(0).toInt();
                    d = query.value(1).toInt();
                    g = query.value(2).toInt();
                    a = query.value(3).toInt();
                    p = query.value(4).toInt();
                    hd = query.value(5).toInt();
                    hg = query.value(6).toInt();
                    ha = query.value(7).toInt();
                    hp = query.value(8).toInt();
                }

                QString updateStr = QString("UPDATE %1 SET round='%2', draws='%3', goals='%4', againsts='%5', points='%6',"
                                            "hDraws='%7', hGoals='%8', hAgainsts='%9', hPoints='%10', lastStatus='%11' WHERE team='%12'")
                        .arg(teamData.at(currentLeague)).arg(hR+1).arg(d+1).arg(g+homeGoalNum).arg(a+awayGoalNum).arg(p+1).arg(hd+1).arg(hg+homeGoalNum)
                        .arg(ha+awayGoalNum).arg(hp+1).arg(getHomeResult).arg(homeTeam);
                query.exec(updateStr);

                //=======客 平========
                QString searchAwayString = QString("SELECT round, draws, goals, againsts, points, aDraws, aGoals, aAgainsts, aPoints FROM %1 WHERE team='%2'")
                        .arg(teamData.at(currentLeague)).arg(awayTeam);
                query.exec( searchAwayString );

                int aR, ad, ag, aa, ap, ads, ags, aAs, aps;
                while ( query.next() ) {
                    aR = query.value(0).toInt();
                    ad = query.value(1).toInt();
                    ag = query.value(2).toInt();
                    aa = query.value(3).toInt();
                    ap = query.value(4).toInt();
                    ads = query.value(5).toInt();
                    ags = query.value(6).toInt();
                    aAs = query.value(7).toInt();
                    aps = query.value(8).toInt();
                }

                QString updateAwayStr = QString("UPDATE %1 SET round='%2', draws='%3', goals='%4', againsts='%5', points='%6', "
                                            "aDraws='%7', aGoals='%8', aAgainsts='%9', aPoints='%10', lastStatus='%11' WHERE team='%12'")
                        .arg(teamData.at(currentLeague)).arg(aR+1).arg(ad+1).arg(ag+awayGoalNum).arg(aa+homeGoalNum).arg(ap+1).arg(ads+1).arg(ags+awayGoalNum)
                        .arg(aAs+homeGoalNum).arg(aps+1).arg(getAwayResult).arg(awayTeam);
                query.exec(updateAwayStr);
            }

        } else if (homeGoalNum<awayGoalNum) {
            qDebug()<<"主 负"<<homeGoalNum<<awayGoalNum;
            //主 负
            if(currentRound==0){
                QString updateStr = QString("UPDATE %1 SET losses='1', goals='%2', againsts='%3', "
                                            "hLosses='1', hGoals='%4', hAgainsts='%5', lastStatus='%6' WHERE team='%7'")
                        .arg(teamData.at(currentLeague)).arg(homeGoalNum).arg(awayGoalNum).arg(homeGoalNum)
                        .arg(awayGoalNum).arg(tr("L----")).arg(homeTeam);
                query.exec( updateStr );

                //=======客 胜========
                QString updateAway = QString("UPDATE %1 SET wins='1', goals='%2', againsts='%3', points='3', "
                                            "aWins='1', aGoals='%4', aAgainsts='%5', aPoints='3',lastStatus='%6' WHERE team='%7'")
                        .arg(teamData.at(currentLeague)).arg(awayGoalNum).arg(homeGoalNum).arg(awayGoalNum)
                        .arg(homeGoalNum).arg(tr("W----")).arg(awayTeam);

                query.exec(updateAway);

            } else {
                QString getHomeResult = updateLastStatus(homeTeam, "L");
                QString getAwayResult = updateLastStatus(awayTeam, "W");
                //先查询上轮数据，再增加到本轮
                QString searchString = QString("SELECT round, losses, goals, againsts, hLosses, hGoals, hAgainsts FROM %1 WHERE team='%2'")
                        .arg(teamData.at(currentLeague)).arg(homeTeam);
                query.exec( searchString );

                int hR, l, g, a, hl, hg, ha;
                while ( query.next() ) {
                    hR = query.value(0).toInt();
                    l = query.value(1).toInt();
                    g = query.value(2).toInt();
                    a = query.value(3).toInt();
                    hl = query.value(4).toInt();
                    hg = query.value(5).toInt();
                    ha = query.value(6).toInt();
                }

                QString updateStr = QString("UPDATE %1 SET round='%2', losses='%3', goals='%4', againsts='%5', "
                                            "hLosses='%6', hGoals='%7', hAgainsts='%8', lastStatus='%9' WHERE team='%10'")
                        .arg(teamData.at(currentLeague)).arg(hR+1).arg(l+1).arg(g+homeGoalNum).arg(a+awayGoalNum).arg(hl+1).arg(hg+homeGoalNum)
                        .arg(ha+awayGoalNum).arg(getHomeResult).arg(homeTeam);
                query.exec(updateStr);

                //=======客 胜========
                QString searchStr = QString("SELECT round, wins, goals, againsts, points, aWins, aGoals, aAgainsts, aPoints FROM %1 WHERE team='%2'")
                        .arg(teamData.at(currentLeague)).arg(homeTeam);
                query.exec( searchStr );

                int aR, aw, ag, aa, ap, aws, ags, aAs, aPs;
                while ( query.next() ) {
                    aR = query.value(0).toInt();
                    aw = query.value(1).toInt();
                    ag = query.value(2).toInt();
                    aa = query.value(3).toInt();
                    ap = query.value(4).toInt();
                    aws = query.value(5).toInt();
                    ags = query.value(6).toInt();
                    aAs = query.value(7).toInt();
                    aPs = query.value(8).toInt();
                }

                QString updateWinStr = QString("UPDATE %1 SET round='%2', wins='%3', goals='%4', againsts='%5', points='%6',"
                                            "aWins='%7', aGoals='%8', aAgainsts='%9', aPoints='%10', lastStatus='%11' WHERE team='%12'")
                        .arg(teamData.at(currentLeague)).arg(aR).arg(aw+1).arg(ag+awayGoalNum).arg(aa+homeGoalNum).arg(ap+3).arg(aws+1)
                        .arg(ags+awayGoalNum).arg(aAs+homeGoalNum).arg(aPs+3).arg(getAwayResult).arg(awayTeam);
                query.exec(updateWinStr);
            }
        }

        //进失球表更新
        //=======进失球========
        if(currentRound==0){
            //主队
            if(ui->homeGoal->text().toInt() == 0){
                updateHomeGoal("zero", 1, homeTeam, awayTeam);
//                query.exec( QString("UPDATE allGoalTable SET zero='1' WHERE round='1' AND team='%2'").arg(homeTeam) );
//                query.exec( QString("UPDATE homeGoalTable SET zero='1' WHERE round='1' AND team='%2'").arg(homeTeam) );
//                query.exec( QString("UPDATE alllossTable SET zero='1' WHERE round='1' AND team='%2'").arg(awayTeam) );
//                query.exec( QString("UPDATE awaylossTable SET zero='1' WHERE round='1' AND team='%2'").arg(awayTeam) );

            } else if (ui->homeGoal->text().toInt() == 1) {
                updateHomeGoal("one", 1, homeTeam, awayTeam);

            } else if (ui->homeGoal->text().toInt() == 2) {
                updateHomeGoal("two", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 3) {
                updateHomeGoal("three", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 4) {
                updateHomeGoal("four", 1,  homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 5) {
                updateHomeGoal("five", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 6) {
                updateHomeGoal("six", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 7) {
                updateHomeGoal("seven", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 8) {
                updateHomeGoal("eight", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 9) {
                updateHomeGoal("nine", 1, homeTeam, awayTeam);
            } else if (ui->homeGoal->text().toInt() == 10) {
                updateHomeGoal("ten", 1, homeTeam, awayTeam);
            }

            //客队
            if(ui->awayGoal->text().toInt() == 0){
                updateAwayGoal("zero", 1, awayTeam, homeTeam);

            } else if (ui->awayGoal->text().toInt() == 1) {
                updateAwayGoal("one", 1, awayTeam, homeTeam);
//                query.exec( QString("UPDATE allGoalTable SET one='1' WHERE round='1' AND team='%2'").arg(awayTeam) );
//                query.exec( QString("UPDATE awayGoalTable SET one='1' WHERE round='1' AND team='%2'").arg(awayTeam) );
//                query.exec( QString("UPDATE alllossTable SET one='1' WHERE round='1' AND team='%2'").arg(homeTeam) );
//                query.exec( QString("UPDATE homelossTable SET one='1' WHERE round='1' AND team='%2'").arg(homeTeam) );
            } else if (ui->awayGoal->text().toInt() == 2) {
                updateAwayGoal("two", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 3) {
                updateAwayGoal("three", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 4) {
                updateAwayGoal("four", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 5) {
                updateAwayGoal("five", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 6) {
                updateAwayGoal("six", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 7) {
                updateAwayGoal("seven", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 8) {
                updateAwayGoal("eight", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 9) {
                updateAwayGoal("nine", 1, awayTeam, homeTeam);
            } else if (ui->awayGoal->text().toInt() == 10) {
                updateAwayGoal("ten", 1, awayTeam, homeTeam);
            }

        } else {
            for (int i = 0; i < pointTableLists.length(); i++) {
                QString str1 = QString( "SELECT zero, one, two, three, four, five, six, seven, eight, nine, ten FROM %1 WHERE team='%2'"  )
                        .arg( pointTableLists.at(i) ).arg( homeTeam );

                query.exec( str1 );

                int zero1, one1, two1, three1, four1, five1, six1, seven1, eight1, nine1, ten1;
                while ( query.next() ) {
                    zero1 = query.value("zero").toInt();
                    one1 = query.value("one").toInt();
                    two1 = query.value("two").toInt();
                    three1 = query.value("three").toInt();
                    four1 = query.value("four").toInt();
                    five1 = query.value("five").toInt();
                    six1 = query.value("six").toInt();
                    seven1 = query.value("seven").toInt();
                    eight1 = query.value("eight").toInt();
                    nine1 = query.value("nine").toInt();
                    ten1 = query.value("ten").toInt();

                }
                //qDebug()<<"qqqqq"<<zero1<<one1<<two1<<three1<<four1<<five1<<six1<<seven1<<eight1<<nine1<<ten1;
                //主队
                if(ui->homeGoal->text().toInt() == 0){
                    updateHomeGoal("zero", (zero1+1), homeTeam, awayTeam);

                } else if (ui->homeGoal->text().toInt() == 1) {
                    updateHomeGoal("one", (one1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 2) {
                    updateHomeGoal("two", (two1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 3) {
                    updateHomeGoal("three", (three1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 4) {
                    updateHomeGoal("four", (four1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 5) {
                    updateHomeGoal("five", (five1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 6) {
                    updateHomeGoal("six", (six1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 7) {
                    updateHomeGoal("seven", (seven1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 8) {
                    updateHomeGoal("eight", (eight1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 9) {
                    updateHomeGoal("nine", (nine1+1), homeTeam, awayTeam);
                } else if (ui->homeGoal->text().toInt() == 10) {
                    updateHomeGoal("ten", (ten1+1), homeTeam, awayTeam);
                }

                //客队
                QString str2 = QString( "SELECT zero, one, two, three, four, five, six, seven, eight, nine, ten FROM %1 WHERE team='%2'"  )
                        .arg( pointTableLists.at(i) ).arg( awayTeam );

                query.exec( str2 );

                int zero2, one2, two2, three2, four2, five2, six2, seven2, eight2, nine2, ten2;
                while ( query.next() ) {
                    zero2 = query.value("zero").toInt();
                    one2 = query.value("one").toInt();
                    two2 = query.value("two").toInt();
                    three2 = query.value("three").toInt();
                    four2 = query.value("four").toInt();
                    five2 = query.value("five").toInt();
                    six2 = query.value("six").toInt();
                    seven2 = query.value("seven").toInt();
                    eight2 = query.value("eight").toInt();
                    nine2 = query.value("nine").toInt();
                    ten2 = query.value("ten").toInt();

                }
                if(ui->awayGoal->text().toInt() == 0){
                    updateAwayGoal("zero", (zero2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 1) {
                    updateAwayGoal("one", (one2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 2) {
                    updateAwayGoal("two", (two2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 3) {
                    updateAwayGoal("three", (three2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 4) {
                    updateAwayGoal("four", (four2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 5) {
                    updateAwayGoal("five", (five2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 6) {
                    updateAwayGoal("six", (six2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 7) {
                    updateAwayGoal("seven", (seven2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 8) {
                    updateAwayGoal("eight", (eight2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 9) {
                    updateAwayGoal("nine", (nine2+1), awayTeam, homeTeam);
                } else if (ui->awayGoal->text().toInt() == 10) {
                    updateAwayGoal("ten", (ten2+1), awayTeam, homeTeam);
                }
            }
        }

        loadDatabase( ui->tableWidget1, homeTeam );
        loadDatabase( ui->tableWidget2, awayTeam );
    }

}

