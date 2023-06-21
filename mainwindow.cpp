#include "mainwindow.h"
#include "ui_mainwindow.h"


QSqlDatabase database;
QSqlQueryModel *tableModel;
QStringList pointTableLists;
QString totalPointTables;
int currentLeague, currentRound;
QStringList premierTeam, laligaTeam, serieaTeam, bundesligaTeam, ligueTeam;


MainWindow::MainWindow(QWidget *parent)  : QMainWindow(parent)  , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(1400, 1000);
    this->setWindowTitle(tr("足球联赛预测-单机版 Version 1.1.0"));
    this->setWindowIcon( QIcon(":/icons/icon/mainIcon.png") );

    infoLabel = new QLabel;
    infoLabel->setMinimumWidth(200);
    ui->statusbar->addWidget(infoLabel);

    tableModel = new QSqlQueryModel();
    currentLeague = ui->cbGames->currentIndex();
    currentRound = ui->cbRounds->currentIndex();

    loadConfig();

    creatDatabase();

    initMain();
//    initTable();
    //加载球队对阵表
    initTableview();
    // 加载球队积分表
    loadLeagueTable();

    addWin = new AddDialog();
    addWin->setWindowTitle( tr("新增对局") );
    addWin->setWindowIcon( QIcon(":/icons/icon/addIcon.png") );


    connect(addWin, &AddDialog::sendAddInfo, this, &MainWindow::receiverAddInfo);

    detailDialog = new scoreDetailDialog();
    detailDialog->setWindowTitle( tr("对阵详情") );
    detailDialog->setWindowIcon( QIcon(":/icons/icon/detailIcon.png") );


    connect(ui->againstTable, &QTableView::doubleClicked, detailDialog, &scoreDetailDialog::getSelectData);
    connect(ui->againstTable, &QTableView::doubleClicked, this, &MainWindow::showDetailDialog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadConfig()
{
    configFilePath = qApp->applicationDirPath()+"/localconfig.ini";
    if(!QFile::exists(configFilePath)){
        QSettings configIniWrite(configFilePath, QSettings::IniFormat);

        configIniWrite.beginGroup("CONFIGURE");
        //0对应A4纸张，1对应A5纸张
        configIniWrite.setValue("INSERTFLAG", false);
        configIniWrite.endGroup();

    }

    QSettings *configFileRead = new QSettings(configFilePath, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    insertFlag = configFileRead->value("CONFIGURE/INSERTFLAG").toBool();
}

void MainWindow::creatDatabase()
{
    // 创建数据库
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("LEAGUEDATABASE.db");

    if( database.open() ) {
        leagueAgainstList<<"premierAgainstTable"<<"laligaAgainstTable"<<"serieaAgainstTable"<<"bundesligaAgainstTable"<<"ligueAgainstTable";
        // 建表 每个联赛的对阵表
        QSqlQuery query;
        for (int i = 0; i < leagueAgainstList.length() ; i++) {

            QString excStr = QString("CREATE TABLE IF NOT EXISTS %1 (round INTEGER, gameTime TEXT, hTeam TEXT, aTeam TEXT, halfScore TEXT,"
                                     " allScore TEXT, halfAll TEXT, predictAll TEXT, predictHalfAll TEXT, predictStatus TEXT, comment TEXT)").arg( leagueAgainstList.at(i) );

            bool success = query.exec( excStr );
            if( success ) {
                qDebug()<<"对阵数据保存完成";
                infoLabel->setText( tr("对阵数据保存完成") );
            } else {
                QMessageBox::information(this, tr("提示"), tr("对阵数据保存失败！"), QMessageBox::Ok);
                infoLabel->setText( tr("对阵数据保存失败") );
//                return;
            }
        }

        teamData<<"premierData"<<"laligaData"<<"serieaData"<<"bundesligaData"<<"ligueData";
        // 建表-积分表 英超-西甲-意甲-德甲-法甲
        for (int i = 0; i < teamData.length(); i++) {
            query.exec( QString("CREATE TABLE %1 ("
                       "round INTEGER, "
                       "rank INTEGER, "
                       "team TEXT, "
                       "wins INTEGER, "
                       "draws INTEGER, "
                       "losses INTEGER, "
                       "goals INTEGER, "
                       "againsts INTEGER, "
                       "points INTEGER, "
                       "hWins INTEGER, "
                       "hDraws INTEGER, "
                       "hLosses INTEGER, "
                       "hGoals INTEGER, "
                       "hAgainsts INTEGER, "
                       "hPoints INTEGER, "
                       "aWins INTEGER, "
                       "aDraws INTEGER, "
                       "aLosses INTEGER, "
                       "aGoals INTEGER, "
                       "aAgainsts INTEGER, "
                       "aPoints INTEGER, "
                       "last TEXT)").arg( teamData.at(i) ));
        }

        premierTeam<<tr("曼城")<<tr("阿森纳")<<tr("曼联")<<tr("纽卡斯尔联")<<tr("利物浦")<<tr("布莱顿")<<tr("维拉")<<tr("热刺")<<tr("布伦特福德")
                    <<tr("富勒姆")<<tr("水晶宫")<<tr("切尔西")<<tr("狼队")<<tr("西汉姆联")<<tr("伯恩茅斯")<<tr("诺丁汉森林")<<tr("埃弗顿")
                      <<tr("伯恩利")<<tr("谢菲尔德联")<<tr("卢顿");

        laligaTeam<< tr("巴萨")<<tr("皇马")<<tr("马竞")<<tr("皇家社会")<<tr("比利亚雷尔")<<tr("皇家贝蒂斯")<<tr("奥萨苏纳")<<tr("毕尔巴鄂")<<tr("马略卡")
                     <<tr("赫罗纳")<<tr("巴列卡诺")<<tr("塞维利亚")<<tr("塞尔塔")<<tr("加的斯")<<tr("赫塔费")<<tr("巴伦西亚")<<tr("阿尔梅里亚")
                       <<tr("格拉纳达")<<tr("拉斯帕尔")<<tr("莱万特");

        serieaTeam<<tr("那不勒斯")<<tr("拉齐奥")<<tr("国际米兰")<<tr("AC米兰")<<tr("亚特兰大")<<tr("罗马")<<tr("尤文图斯")<<tr("佛罗伦萨")<<tr("博洛尼亚")
                 <<tr("都灵")<<tr("蒙扎")<<tr("乌迪内斯")<<tr("萨索洛")<<tr("恩波利")<<tr("萨勒尼塔")<<tr("莱切")<<tr("斯佩齐亚")<<tr("维罗纳")
                <<tr("热那亚")<<tr("佛洛西");

        bundesligaTeam<<tr("拜仁")<<tr("多特蒙德")<<tr("RB莱比锡")<<tr("柏林联合")<<tr("弗莱堡")<<tr("勒沃库森")<<tr("法兰克福")
                     <<tr("沃夫斯堡")<<tr("美因茨")<<tr("门兴")<<tr("科隆")<<tr("霍芬海姆")<<tr("不莱梅")<<tr("波鸿")<<tr("奥格斯堡")
                    <<tr("斯图加特")<<tr("海登海姆")<<tr("达姆斯塔特");

        ligueTeam<<tr("巴黎")<<tr("朗斯")<<tr("马赛")<<tr("雷恩")<<tr("里尔")<<tr("摩纳哥")<<tr("里昂")<<tr("克莱蒙")<<tr("尼斯")
                <<tr("洛里昂")<<tr("兰斯")<<tr("蒙彼利埃")<<tr("图卢兹")<<tr("布雷斯特")<<tr("斯特拉斯")<<tr("南特")<<tr("欧赛尔")
                  <<tr("阿雅克肖")<<tr("特鲁瓦")<<tr("昂热");

        //建表 积分表
        pointTableLists<<"allinTable"<<"alllossTable"<<"homeinTable"<<"homelossTable"<<"awayinTable"<<"awaylossTable";
        //QSqlQuery query;
        for (int i = 0; i < pointTableLists.length(); i++ ) {
            query.exec( QString("CREATE TABLE IF NOT EXISTS %1 ("
                                "round INTEGER, "
                                "team TEXT, "
                                "zero INTEGER, "
                                "one INTEGER, "
                                "two INTEGER, "
                                "three INTEGER, "
                                "four INTEGER, "
                                "five INTEGER, "
                                "six INTEGER, "
                                "seven INTEGER, "
                                "eight INTEGER, "
                                "nine INTEGER, "
                                "ten INTEGER)").arg( pointTableLists.at(i) ));
        }

        //建统计表
        totalPointTables = "allPointTable";
        query.exec( QString("CREATE TABLE IF NOT EXISTS %1 ("
                            "round INTEGER, "
                            "team TEXT, "
                            "allWin INTEGER, "
                            "allDraw INTEGER, "
                            "allLoss INTEGER, "
                            "allGoal INTEGER, "
                            "allLosses INTEGER, "
                            "allPoint INTEGER, "
                            "homeWin INTEGER, "
                            "homeDraw INTEGER, "
                            "homeLoss INTEGER, "
                            "homeGoal INTEGER, "
                            "homeLosses INTEGER,"
                            "homePoint INTEGER,"
                            "awayWin INTEGER, "
                            "awayDraw INTEGER, "
                            "awayLoss INTEGER, "
                            "awayGoal INTEGER, "
                            "awayLosses INTEGER,"
                            "awayPoint INTEGER)").arg( totalPointTables ));

        if (!insertFlag) {
            insertTable("premierData", premierTeam);
            insertTable("laligaData", laligaTeam);
            insertTable("serieaData", serieaTeam);
            insertTable("bundesligaData", bundesligaTeam);
            insertTable("ligueData", ligueTeam);

            for (int k = 0; k < pointTableLists.length(); k++) {
                creatPointTable( pointTableLists.at(k), premierTeam );
                creatPointTable( pointTableLists.at(k), laligaTeam );
                creatPointTable( pointTableLists.at(k), serieaTeam );
                creatPointTable( pointTableLists.at(k), bundesligaTeam );
                creatPointTable( pointTableLists.at(k), ligueTeam );

            }
            insertTotalTable( premierTeam );
            insertTotalTable( laligaTeam );
            insertTotalTable( serieaTeam );
            insertTotalTable( bundesligaTeam );
            insertTotalTable( ligueTeam );

            //insertFlag = true;
            QSettings configIniWrite(configFilePath, QSettings::IniFormat);

            configIniWrite.beginGroup("CONFIGURE");
            //0对应A4纸张，1对应A5纸张
            configIniWrite.setValue("INSERTFLAG", true);
            configIniWrite.endGroup();

        }

        infoLabel->setText( tr("数据加载完成") );
    } else {
        infoLabel->setText( tr("数据加载失败") );
    }

}


void MainWindow::insertTable( QString leagueTable, QStringList teamList )
{
    QSqlQuery query;

    for (int j = 0; j < ( teamList.length() - 1 ) * 2; j++) {
        for (int i = 0; i < teamList.length(); i++) {
            QString execString = QString( "INSERT INTO %1 (round, rank, team, wins, draws, losses, goals, againsts, points,"
                                          " hWins, hDraws, hLosses, hGoals, hAgainsts, hPoints, aWins, aDraws, aLosses, aGoals, aAgainsts, aPoints, last) "
                                          "VALUES (:round, :rank, :team, :wins, :draws, :losses, :goals, :againsts, :points,"
                                          " :hWins, :hDraws, :hLosses, :hGoals, :hAgainsts, :hPoints, :aWins, :aDraws, :aLosses, :aGoals, :aAgainsts, :aPoints, :last)" )
                    .arg( leagueTable );

            // 准备插入语句
            query.prepare( execString );

            // 绑定插入数据的值
            query.bindValue(":round", j + 1);
            query.bindValue(":rank", i + 1);
            query.bindValue(":team", teamList.at(i) );
            query.bindValue(":wins", 0);
            query.bindValue(":draws", 0);
            query.bindValue(":losses", 0);
            query.bindValue(":goals", 0);
            query.bindValue(":againsts", 0);
            query.bindValue(":points", 0);
            query.bindValue(":hWins", 0);
            query.bindValue(":hDraws", 0);
            query.bindValue(":hLosses", 0);
            query.bindValue(":hGoals", 0);
            query.bindValue(":hAgainsts", 0);
            query.bindValue(":hPoints", 0);
            query.bindValue(":aWins", 0);
            query.bindValue(":aDraws", 0);
            query.bindValue(":aLosses", 0);
            query.bindValue(":aGoals", 0);
            query.bindValue(":aAgainsts", 0);
            query.bindValue(":aPoints", 0);
            query.bindValue(":last", "-----");

            // 执行插入操作
            if (!query.exec()) {
                // 插入失败处理
                infoLabel->setText( "insert data failure!" );
                qDebug()<<"insert data failure!";
                return;
            }

        }
    }
    qDebug()<<QString("insert %1 success!").arg(leagueTable);
}

void MainWindow::creatPointTable(QString a, QStringList teams)
{
    QSqlQuery query;

    for (int m = 0; m < (teams.length() - 1 ) * 2; m++ ) {
        for (int n = 0; n < teams.length(); n++) {
            //插入数据
            QString execString = QString( "INSERT INTO %1 (round, team, zero, one, two, three, four, five, six, seven, eight, nine, ten) "
                                          "VALUES (:round, :team, :zero, :one, :two, :three, :four, :five, :six, :seven, :eight, :nine, :ten)" ).arg( a );

            // 准备插入语句
            query.prepare( execString );

            // 绑定插入数据的值
            query.bindValue(":round",  m+1);
            query.bindValue(":team", teams.at(n) );
            query.bindValue(":zero", 0);
            query.bindValue(":one", 0);
            query.bindValue(":two", 0);
            query.bindValue(":three", 0);
            query.bindValue(":four", 0);
            query.bindValue(":five", 0);
            query.bindValue(":six", 0);
            query.bindValue(":seven", 0);
            query.bindValue(":eight", 0);
            query.bindValue(":nine", 0);
            query.bindValue(":ten", 0);

            // 执行插入操作
            if ( !query.exec() ) {
                // 插入失败处理
                infoLabel->setText( "insert data failure!" );
                qDebug()<<"insert points data failure!";
                return;
            }
        }
    }
}

void MainWindow::insertTotalTable( QStringList lTeam )
{
    QSqlQuery query;

    for (int m = 0; m < (lTeam.length() - 1 ) * 2; m++ ) {
        for (int n = 0; n < lTeam.length(); n++) {
            //插入数据
            QString execString = QString( "INSERT INTO allPointTable (round, team, allWin, allDraw, allLoss, allGoal, allLosses, allPoint,"
                                          " homeWin, homeDraw, homeLoss, homeGoal, homeLosses, homePoint, awayWin, awayDraw, awayLoss, awayGoal, awayLosses, awayPoint) "
                                          "VALUES (:round, :team, :allWin, :allDraw, :allLoss, :allGoal, :allLosses, :allPoint, :homeWin, :homeDraw, :homeLoss,"
                                          " :homeGoal, :homeLosses, :homePoint, :awayWin, :awayDraw, :awayLoss, :awayGoal, :awayLosses, :awayPoint)" );

            // 准备插入语句
            query.prepare( execString );

            // 绑定插入数据的值
            query.bindValue(":round",  m+1);
            query.bindValue(":team", lTeam.at(n) );
            query.bindValue(":allWin", 0);
            query.bindValue(":allDraw", 0);
            query.bindValue(":allLoss", 0);
            query.bindValue(":allGoal", 0);
            query.bindValue(":allLosses", 0);
            query.bindValue(":allPoint", 0);
            query.bindValue(":homeWin", 0);
            query.bindValue(":homeDraw", 0);
            query.bindValue(":homeLoss", 0);
            query.bindValue(":homeGoal", 0);
            query.bindValue(":homeLosses", 0);
            query.bindValue(":homePoint", 0);
            query.bindValue(":awayWin", 0);
            query.bindValue(":awayDraw", 0);
            query.bindValue(":awayLoss", 0);
            query.bindValue(":awayGoal", 0);
            query.bindValue(":awayLosses", 0);
            query.bindValue(":awayPoint", 0);

            // 执行插入操作
            if ( !query.exec() ) {
                // 插入失败处理
                infoLabel->setText( "insert toatal point table failure!" );
                qDebug()<<"insert toatal point table failure!";
                return;
            }
        }
    }
}

void MainWindow::initMain()
{
    QStringList soccerGames;
    soccerGames<<tr("英超联赛")<<tr("西甲联赛")<<tr("意甲联赛")<<tr("德甲联赛")<<tr("法甲联赛");
    ui->cbGames->clear();
    ui->cbGames->addItems(soccerGames);

    ui->cbRounds->clear();
    QString gameRound;
    for (int i = 1; i < 39; i++) {
        ui->cbRounds->addItem( QString("%1").arg(i) );

    }

}

void MainWindow::initTable()
{
    //第一个表格
//    ui->groupBox->setFixedHeight(400);
//    ui->tableWidget1->setFixedHeight(360);
//    ui->tableWidget1->setRowCount(11);
//    ui->tableWidget1->setColumnCount(9);

//    ui->tableWidget1->verticalHeader()->setVisible(false);
//    ui->tableWidget1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //使列完全填充并平分
//    ui->tableWidget1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget1->setAlternatingRowColors(true);     //设置隔行变颜色

//    ui->tableWidget1->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->tableWidget1->setSelectionBehavior(QAbstractItemView::SelectRows);     //整行选择

//    QStringList headLists1;
//    headLists1<<tr("比赛时间")<<tr("主队")<<tr("客队")<<tr("半场比分")<<tr("全场比分")<<tr("半全场")<<tr("预测全场")<<tr("预测半全")<<tr("预测状态");
//    ui->tableWidget1->setHorizontalHeaderLabels( headLists1 );

//    QHeaderView *headerView1 = ui->tableWidget1->horizontalHeader();
//    headerView1->setStyleSheet("QHeaderView::section {background-color: rgb(85, 170, 255)}");

    //第二个表格
    //--------------------------QTableWidget形式开始----------------------------------
    /*
    ui->tableWidget2->setRowCount(21);
    ui->tableWidget2->setColumnCount(22);
    ui->tableWidget2->verticalHeader()->setVisible(false);

    QStringList headerList2;
    headerList2<<tr("轮次")<<tr("排 名")<<tr("球 队")<<tr("胜")<<tr("平")<<tr("负")<<tr("进球数")<<tr("失球数")<<tr("积分")
              <<tr("主胜")<<tr("主平")<<tr("主负")<<tr("主进球")<<tr("主失球")<<tr("主积分")
             <<tr("客胜")<<tr("客平")<<tr("客负")<<tr("客进球")<<tr("客失球")<<tr("客积分")<<tr("近5场");
    ui->tableWidget2->setHorizontalHeaderLabels( headerList2 );

    ui->tableWidget2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //使列完全填充并平分
    QHeaderView *headerView2 = ui->tableWidget2->horizontalHeader();
    headerView2->setStyleSheet("QHeaderView::section {background-color: rgb(85, 170, 255)}");
    ui->tableWidget2->setSelectionBehavior(QAbstractItemView::SelectRows);     //整行选择

    //表格前四行为欧冠区 rgb(0, 255, 127)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < ui->tableWidget2->columnCount(); j++ ) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData( Qt::BackgroundColorRole, QColor(0, 255, 127) );
            ui->tableWidget2->setItem(i, j, item);
        }
    }

    //欧联区
    for (int j = 0; j < ui->tableWidget2->columnCount(); j++ ) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData( Qt::BackgroundColorRole, QColor(170, 255, 255) );
        ui->tableWidget2->setItem(4, j, item);
    }

    //降级区
    for (int i = 17; i < ui->tableWidget2->rowCount(); i++) {
        for (int j = 0; j < ui->tableWidget2->columnCount(); j++ ) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData( Qt::BackgroundColorRole, QColor(172, 172, 172) );
            ui->tableWidget2->setItem(i, j, item);
        }
    }
*/
    //--------------------------QTableWidget形式结束----------------------------------
}
// 初始化对阵表
void MainWindow::initTableview()
{
    ui->groupBox->setFixedHeight(400);
    ui->againstTable->setFixedHeight(360);

//    QSqlQueryModel *tableModel = new QSqlQueryModel();
    tableModel->setQuery( QString("SELECT round, gameTime,"
                                  "hTeam, aTeam, halfScore, allScore, halfAll, predictAll, predictHalfAll,"
                                  "predictStatus FROM %1 WHERE round=='%2'")
                          .arg( leagueAgainstList.at(ui->cbGames->currentIndex()) ).arg( ui->cbRounds->currentIndex()+1 ) );

    if ( tableModel->lastError().isValid() ) {
        QMessageBox::information(this, tr("提 示"), tr("对阵信息不完整，新增失败！"), QMessageBox::Ok);
        return;
    }

    QStringList headLists;
    headLists<<tr("轮次")<<tr("比赛时间")<<tr("主队")<<tr("客队")<<tr("半场比分")<<tr("全场比分")<<tr("半全场")<<tr("预测全场")<<tr("预测半全")<<tr("预测状态");
    for (int i = 0; i < headLists.length( ); i++) {
        tableModel->setHeaderData( i, Qt::Horizontal, headLists.at(i) );
    }

    ui->againstTable->setModel( tableModel );
    ui->againstTable->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->againstTable->horizontalHeader()->setDefaultAlignment( Qt::AlignCenter );
    ui->againstTable->resizeColumnToContents(1);
    ui->againstTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui->againstTable->horizontalHeader()->setStyleSheet( "QHeaderView::section {background-color: rgb(85, 170, 255)}" );

    ui->againstTable->verticalHeader()->setVisible(false);
}
//加载联赛积分表
void MainWindow::loadLeagueTable()
{
    QSqlQueryModel *qryModel = new QSqlQueryModel();
    qryModel->setQuery( QString("SELECT * FROM %1 WHERE round=='%2'").arg( teamData.at(ui->cbGames->currentIndex()) ).arg( ui->cbRounds->currentIndex()+1 ) );
    if ( qryModel->lastError().isValid() ) {
        QMessageBox::information(this, tr("提 示"), tr("对阵信息不完整，新增失败！"), QMessageBox::Ok);
        return;
    }

    modelToTable( qryModel  );

}

void MainWindow::modelToTable(QSqlQueryModel *queryModel)
{
    ui->tableView->setModel( queryModel );

    QStringList headList;
    headList<<tr("轮次")<<tr("排 名")<<tr("球 队")<<tr("胜")<<tr("平")<<tr("负")<<tr("进球数")<<tr("失球数")<<tr("积分")
           <<tr("主胜")<<tr("主平")<<tr("主负")<<tr("主进球")<<tr("主失球")<<tr("主积分")
          <<tr("客胜")<<tr("客平")<<tr("客负")<<tr("客进球")<<tr("客失球")<<tr("客积分")<<tr("近5场");

    for (int i = 0; i <headList.length( ); i++) {
        queryModel->setHeaderData( i, Qt::Horizontal, headList.at(i) );
    }
    //排序
//    qryModel->sort()

//    ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->tableView->horizontalHeader()->setDefaultAlignment( Qt::AlignCenter );

    ui->tableView->verticalHeader()->setVisible(false);

//    ui->tableView->setColumnWidth(2, 300);
    ui->tableView->resizeColumnToContents(2);

    ui->tableView->setSelectionBehavior( QAbstractItemView::SelectRows );
    //表格内容不可编辑
    ui->tableView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    ui->tableView->horizontalHeader()->setStyleSheet( "QHeaderView::section {background-color: rgb(85, 170, 255)}" );
}

//获取新增比赛信息
void MainWindow::receiverAddInfo(QStringList data)
{
    if( !data.at(1).isEmpty() && !data.at(2).isEmpty() ) {
        //主队和客队都不为空，则新增
        if(data.at(1) == data.at(2)) {
            QMessageBox::information(this, tr("提 示"), tr("对阵球队重复，新增失败！"), QMessageBox::Ok);
            return;
        }

        //查询新增球队在当前轮次是否已存在
        QSqlQuery query;
        QString searchStr = QString( "SELECT hTeam, aTeam FROM %1 WHERE round=='%2' " )
                .arg( leagueAgainstList.at( ui->cbGames->currentIndex() ) )
                .arg( ui->cbRounds->currentIndex() + 1 );
        query.exec( searchStr );

        while ( query.next() ) {
            QString homeTeam = query.value(0).toString();
            QString awayTeam = query.value(1).toString();

            if (homeTeam == data.at(1) || awayTeam == data.at(2)){
                QMessageBox::information(this, tr("提 示"), tr("对阵球队已存在，新增失败！"), QMessageBox::Ok);
                return;
            }

        }

        QString execStr = QString("INSERT INTO %1 (round, gameTime, hTeam, aTeam) "
                                  "VALUES (:round, :gameTime, :hTeam, :aTeam)").arg(leagueAgainstList.at( ui->cbGames->currentIndex() ));
        query.prepare( execStr );

        // 绑定插入数据的值
        query.bindValue(":round",  ui->cbRounds->currentIndex() + 1);
        query.bindValue(":gameTime", data.at(0));
        query.bindValue(":hTeam", data.at(1));
        query.bindValue(":aTeam", data.at(2));

        bool success = query.exec();
        // 执行插入操作
        if (!success) {
            // 插入失败处理
            infoLabel->setText( "insert data failure!" );
            qDebug()<<"insert data failure!";
            return;
        }
        initTableview();

    } else {
        QMessageBox::information(this, tr("提 示"), tr("对阵信息不完整，新增失败！"), QMessageBox::Ok);

    }

}

void MainWindow::on_btnAdd_clicked()
{
    addWin->getSelectIndex(ui->cbGames->currentIndex(), ui->cbRounds->currentIndex());
    addWin->exec();

}

void MainWindow::showDetailDialog()
{

    detailDialog->exec();

}


void MainWindow::on_cbGames_currentIndexChanged(int index)
{
    currentLeague = index;
    QSqlQueryModel *qryModel = new QSqlQueryModel();
    //更新排名数据库
    qryModel->setQuery( QString("SELECT * FROM %1 WHERE round=='%2'").arg( teamData.at(index) ).arg( ui->cbRounds->currentIndex()+1 ) );
    if ( qryModel->lastError().isValid() ) {
        QMessageBox::information(this, tr("提 示"), tr("对阵信息不完整，新增失败！"), QMessageBox::Ok);
        return;
    }

    modelToTable( qryModel );
    initTableview();
}


void MainWindow::on_cbRounds_currentIndexChanged(int index)
{
    currentRound = index;
    QSqlQueryModel *qryModel = new QSqlQueryModel();
    //更新排名数据库
    qryModel->setQuery( QString("SELECT * FROM %1 WHERE round=='%2'").arg( teamData.at( ui->cbGames->currentIndex() ) ).arg( index +1 ) );
    if ( qryModel->lastError().isValid() ) {
        QMessageBox::information(this, tr("提 示"), tr("对阵信息不完整，新增失败！"), QMessageBox::Ok);
        return;
    }

    modelToTable( qryModel );
    initTableview();

}

