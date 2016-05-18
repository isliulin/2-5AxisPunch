#include "simulateknob.h"
#include "ui_simulateknob.h"
#include <QProcess>
#include <QTcpSocket>
#include "icnwmprotocol.h"
#include "parser.h"
#include "icvirtualhost.h"

SimulateKnob::SimulateKnob(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulateKnob)
{
    ui->setupUi(this);
    connect(ui->manualButton,
            SIGNAL(clicked()),
            SIGNAL(manualButtonClicked()));
    connect(ui->stopButton,
            SIGNAL(clicked()),
            SIGNAL(stopButtonClicked()));
    connect(ui->autoButton,
            SIGNAL(clicked()),
            SIGNAL(autoButtonClicked()));
    this->setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
    server_.listen(QHostAddress::Any, 8888);
    connect(&server_, SIGNAL(newConnection()), SLOT(onClientServerUp()));
    connect(&connectedSocket_, SIGNAL(connected()), SLOT(OnConnectedClient()));
    connect(&connectedSocket_, SIGNAL(disconnected()), SLOT(OnDisconnectedClient()));
    connect(&connectedSocket_, SIGNAL(readyRead()), SLOT(OnReadyReadClient()));
}

SimulateKnob::~SimulateKnob()
{
    delete ui;
}

void SimulateKnob::on_scanButton_clicked()
{

}

void SimulateKnob::onClientServerUp()
{
    qDebug("Client IN");
    QTcpSocket* socket = server_.nextPendingConnection();
//    connect(socket, SIGNAL(disconnected()),SLOT(onClientServerDown()));
    QHostAddress clientAddr = socket->peerAddress();
    if(addrToItems_.contains(clientAddr.toString()))
        return;
    ui->onlineMachineList->addItem(clientAddr.toString());
    onlineClients_.insert(ui->onlineMachineList->item(ui->onlineMachineList->count() - 1), clientAddr);
    addrToItems_.insert(clientAddr.toString(), ui->onlineMachineList->item(ui->onlineMachineList->count() - 1));
    socket->close();
}

void SimulateKnob::onClientServerDown()
{

}

void SimulateKnob::on_onlineMachineList_itemDoubleClicked(QListWidgetItem *item)
{
    connectedSocket_.close();
    qDebug()<<"hostAddr:"<<onlineClients_.value(item);
    connectedSocket_.connectToHost(onlineClients_.value(item), 9999);
}

void SimulateKnob::OnConnectedClient()
{
    connectedSocket_.write(ICNWMProtocol::RequestInitPackage());
    ICVirtualHost::GlobalVirtualHost()->SetClientConnected(true);
    QListWidgetItem* item = addrToItems_.value(connectedSocket_.peerAddress().toString());
    item->setText(item->text() + "|" + tr("Connected"));
}

void SimulateKnob::OnDisconnectedClient()
{
    QHostAddress addr = connectedSocket_.peerAddress();
    QListWidgetItem* item = addrToItems_.value(addr.toString());
    onlineClients_.remove(item);
    addrToItems_.remove(addr.toString());
    ui->onlineMachineList->removeItemWidget(item);
}

void SimulateKnob::OnReadyReadClient()
{

    QString rd = connectedSocket_.readAll();
    while (!connectedSocket_.atEnd()) {
        rd += connectedSocket_.readAll();
    }
    connectedSocket_.waitForReadyRead(100);
    while (!connectedSocket_.atEnd()) {
        rd += connectedSocket_.readAll();
    }
    rd += connectedSocket_.readAll();
    QStringList datas = rd.split("}{");
    if(datas.isEmpty()) return;
    if(datas.size() > 1)
    {
        datas[0].append('}');
        for(int i = 1; i < datas.size() - 1; ++i)
        {
            datas[i].prepend('{');
            datas[i].append('}');
        }
        datas[datas.size() -1].prepend('{');
    }
    QJson::Parser parser;
    bool ok;
    for(int i = 0; i < datas.size(); ++i)
    {
        QVariant d = parser.parse(datas.at(i).toUtf8(), &ok);
        if(!ok)
        {
            qDebug()<<"Parser Error"<<datas.at(i);
            continue;
        }
        QVariantMap dOject = d.toMap();
        ICVirtualHost::GlobalVirtualHost()->RecvClientData(dOject);
    }
}
