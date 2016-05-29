#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), player(NULL), server(NULL), client(NULL), lobbyWidget(NULL),

    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frameJoin->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete lobbyWidget;
    //delete gameScreenWidget;
    /*delete player;
    delete server;
    delete client;*/
    delete ui;
}

void MainWindow::on_btnWyjscie_clicked()
{
    if(ui->btnWyjscie->text()=="Powrót")
    {
        ui->btnDolacz->setVisible(true);
        ui->btnStworz->setVisible(true);
        ui->txtNick->setVisible(false);
        ui->txtIP->setVisible(false);
        ui->gphNick->setVisible(false);
        ui->gphIP->setVisible(false);
        ui->btnWyjscie->setText("Wyjście");
    }
    else
    {
      QApplication::exit();
    }
}

void MainWindow::on_btnDolacz_clicked()
{
    ui->btnDolacz->setVisible(false);
    ui->btnStworz->setVisible(false);
    ui->btnWyjscie->setVisible(false);
    ui->frameJoin->setVisible(true);
    ui->btnStworzGre->setVisible(false);

}

void MainWindow::on_btnStworz_clicked()
{
    ui->btnDolacz->setVisible(false);
    ui->btnStworz->setVisible(false);
    ui->btnWyjscie->setVisible(false);
       ui->txtNick->setVisible(true);
    ui->gphNick->setVisible(true);
    ui->frameJoin->setVisible(true);
    ui->txtIP->setVisible(false);
    ui->txtport->setVisible(false);
    ui->gphport->setVisible(false);
    ui->gphIP->setVisible(false);
    ui->btnStworzGre->setVisible(true);
}

void MainWindow::hostGame(QString playerName, int port)
{
    //if (player != NULL) delete player;
    //if (server != NULL) delete server;
    player = new Player(this, playerName);
    server = new Server(this, player);
    lobbyWidget = LobbyWidget::createServerLobby(this, server, port);
    lobbyWidget->show();
    lobbyWidget->startServer();
}

void MainWindow::joinGame(QString playerName, QString ip, int port)
{
    //if (player != NULL) delete player;
    //if (server != NULL) delete server;
    player = new Player(this, playerName);
    client = new Client(this, player);
    lobbyWidget = LobbyWidget::createClientLobby(this, client, ip, port);
    lobbyWidget->show();
    lobbyWidget->startClient();
}

void MainWindow::on_pushButton_2_clicked()
{
    hostGame("host", port);
}

void MainWindow::on_pushButton_clicked()
{
    joinGame("client", "127.0.0.1", port);
}

void MainWindow::on_btnPowrot_clicked()
{
    ui->btnDolacz->setVisible(true);
    ui->btnStworz->setVisible(true);
    ui->frameJoin->setVisible(false);
     ui->btnWyjscie->setVisible(true);
      ui->txtIP->setVisible(true);
     ui->txtport->setVisible(true);
     ui->gphport->setVisible(true);
     ui->gphIP->setVisible(true);
     ui->txtNick->setVisible(true);
     ui->gphNick->setVisible(true);
     ui->btnStworzGre->setVisible(false);
}

void MainWindow::on_btnDolaczdoGry_clicked()
{
   joinGame("clientPlayer", "127.0.0.1", port);
}

void MainWindow::on_btnStworzGre_clicked()
{
    hostGame("hostPlayer", port);
}
