#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    tcpConnect();
    //botão start
    connect(ui->pushButton_start, &QPushButton::clicked, this, &MainWindow::putData);

    //botão stop
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &MainWindow::timerStop);

    //tcp conectado:
    connect(ui->pushButton_Connect, &QPushButton::clicked, this, &MainWindow::tcpConnect);

    //tcp desconectado:
    connect(ui->pushButton_Disconnect, &QPushButton::clicked, this, &MainWindow::tcpDisconnect);

    //H_SLIDER MINIMO
    connect(ui->horizontalSlider_min, &QSlider::valueChanged, ui->lcdNumber_min, static_cast<void (QLCDNumber::*)(int)>(&QLCDNumber::display));

    //H_SLIDER MAXIMO
    connect(ui->horizontalSlider_max, &QSlider::valueChanged, ui->lcdNumber_max, static_cast<void (QLCDNumber::*)(int)>(&QLCDNumber::display));

    //H_SLIDER INTERVALO
    connect(ui->horizontalSlider_intervalo, &QSlider::valueChanged, ui->label_intervaloInt, static_cast<void (QLabel::*)(int)>(&QLabel::setNum));

    //Criando variável do tempo min
    connect(ui->horizontalSlider_min, &QSlider::valueChanged, this, &MainWindow::valorMin);

    //Criando variável do tempo max
    connect(ui->horizontalSlider_max, &QSlider::valueChanged, this, &MainWindow::valorMax);

    //Criando variável do intervalo de tempo
    connect(ui->horizontalSlider_intervalo, &QSlider::valueChanged, this, &MainWindow::valorInterv);

    Timer = new QTimer(this);
    //intervalo start
    connect(Timer, &QTimer::timeout, this, &MainWindow::timerEvent);
    Timer->setInterval(seg);
}

void MainWindow::tcpConnect()
{
    socket->connectToHost(ui->lineEdit_ip->text(), 1234);
    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected";
        statusBar()->showMessage("Connected");
    } else {
        qDebug() << "Disconnected";
        statusBar()->showMessage("Disconnected");
    }
}

void MainWindow::tcpDisconnect()
{
    socket->disconnectFromHost();
    statusBar()->showMessage("Disconnected");
}

void MainWindow::valorMin(int vMin)
{
    min = vMin;
}

void MainWindow::valorMax(int vMax)
{
    max = vMax;
}

void MainWindow::valorInterv(int inter)
{
    seg = inter * 500;
    Timer->setInterval(seg);
}

void MainWindow::putData()
{
    Timer->start();
    QDateTime datetime;
    QString str;
    qint64 msecdate;
    if (socket->state() == QAbstractSocket::ConnectedState) {
        msecdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
        str = "set " + QString::number(msecdate) + " " +
              QString::number(rand() % (max - min) + min) + "\n";

        ui->textBrowser_exibidor->append(str);
        qDebug() << &Timer << " ";
        qDebug() << str;
        qDebug() << socket->write(str.toStdString().c_str())
                 << " bytes written";
        if (socket->waitForBytesWritten(3000)) {
            qDebug() << "wrote";
        }
    }
}

MainWindow::~MainWindow()
{
    delete socket;
    delete ui;
}

void MainWindow::timerEvent()
{
    putData();
}

void MainWindow::timerStop()
{
    Timer->stop();
}
