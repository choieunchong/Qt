#include "chatserverform.h"
#include "ui_chatserverform.h"
#include "logthread.h"


#include <QPushButton>
#include <QBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDebug>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

ChatServerForm::ChatServerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatserverForm), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);
    QList<int> sizes;
    sizes << 120 << 500;
    ui->splitter->setSizes(sizes);

    chatServer = new QTcpServer(this);
    connect(chatServer, SIGNAL(newConnection( )), SLOT(clientConnect( )));
    if (!chatServer->listen(QHostAddress::Any, PORT_NUMBER)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(chatServer->errorString( )));
        close( );
        return;
    }

    fileServer = new QTcpServer(this);
    connect(fileServer, SIGNAL(newConnection()),SLOT(acceptConnection()));
    if(!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)){
        QMessageBox::critical(this, tr("chatting Server"),
                              tr("Unable to start the server: %1.").arg(fileServer->errorString()));
        close();
        return;
    }

    qDebug("Start listening ...");

    QAction* inviteAction = new QAction(tr("&Invite"));
    inviteAction->setObjectName("Invite");
    connect(inviteAction, SIGNAL(triggered()), SLOT(inviteClient()));

    QAction* removeAction = new QAction(tr("&Kick out"));
    connect(removeAction, SIGNAL(triggered()), SLOT(kickOut()));


    QPushButton* connectbutton = new QPushButton(tr("&connectbutton"));
    connectbutton->setObjectName("connectbutton");
    connect(connectbutton, SIGNAL(clicked()), SLOT(inviteClient()));


    QPushButton* exitbuton = new QPushButton(tr("&exit"));
    connect(exitbuton, SIGNAL(clicked()), SLOT(kickOut()));

    menu = new QMenu;
    menu->addAction(inviteAction);
    menu->addAction(removeAction);

    ui->clientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    logThread = new LogThread(this);
    logThread->start();

    connect(ui->savebutton, SIGNAL(clicked()), logThread, SLOT(saveData()));

    qDebug() << tr("The server is running on port %1.").arg(chatServer->serverPort( ));
}

ChatServerForm::~ChatServerForm()
{
    delete ui;
    logThread->terminate();
    chatServer->close();
    fileServer->close( );
}


void ChatServerForm::acceptConnection()
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

void ChatServerForm::clientConnect( )
{
    QTcpSocket *clientConnection = chatServer->nextPendingConnection( );
    connect(clientConnection, SIGNAL(readyRead( )), SLOT(receiveData( )));
    connect(clientConnection, SIGNAL(disconnected( )), SLOT(removeClient()));
    qDebug("new connection is established...");
}

void ChatServerForm::receiveData( )
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    Chat_Status type;       // ????????? ??????
    char data[1020];        // ???????????? ?????????/?????????
    memset(data, 0, 1020);

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in.device()->seek(0);
    in >> type;
    in.readRawData(data, 1020);

    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();
    QString name = QString::fromStdString(data);

    qDebug() << ip << " : " << type;

    switch(type) {
    case Chat_Login:
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
            if(item->text(1) != "O") {
                item->setIcon(0, QIcon(":/image/redpoint.png"));     // QList<QTcpSocket*> clientList;
            }
            clientSocketHash[name] = clientConnection;
        }
        break;
    case Chat_In:
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1)) {
            if(item->text(1) != "O") {
                item->setIcon(0, QIcon(":/image/greenpoint.png"));
            }
            clientNameHash[port] = name;
            if(clientSocketHash.contains(name))
                clientSocketHash[name] = clientConnection;
        }
        break;
    case Chat_Talk: {
        foreach(QTcpSocket *sock, clientSocketHash.values()) {
            qDebug() << sock->peerPort();
            if(clientNameHash.contains(sock->peerPort()) && port != sock->peerPort()) {
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;
                sendArray.append("<font color=lightsteelblue>");
                sendArray.append(clientNameHash[port].toStdString().data());
                sendArray.append("</font> : ");
                sendArray.append(name.toStdString().data());
                sock->write(sendArray);
                qDebug() << sock->peerPort();
            }
        }

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, QString::number(clientIDHash[clientNameHash[port]]));
        item->setText(3, clientNameHash[port]);
        item->setText(4, QString(data));
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, QString(data));

        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        logThread->appendData(item);
    }
        break;
    case Chat_Out:
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            if(item->text(0) != "-") {
                item->setIcon(0, QIcon(":/image/redpoint.png"));
            }
            clientNameHash.remove(port);
        }
        break;
    case Chat_LogOut:
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            if(item->text(0) != "X") {
                item->setIcon(0, QIcon(":/image/redpoint.png"));
            }
            clientSocketHash.remove(name);
        }
        break;
    }
}

void ChatServerForm::removeClient()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    if(clientConnection != nullptr) {
        QString name = clientNameHash[clientConnection->peerPort()];
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
            item->setIcon(0, QIcon(":/image/redpoint.png"));
        }
        clientSocketHash.remove(name);
        clientConnection->deleteLater();
    }
}

void ChatServerForm::addClient(int id, QString name)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->clientTreeWidget);
    item->setIcon(0, QIcon(":/image/redpoint.png"));
    item->setText(1, name);
    ui->clientTreeWidget->addTopLevelItem(item);
    clientIDHash[name] = id;
    ui->clientTreeWidget->resizeColumnToContents(0);
}


/* ??????????????? ???????????? */
void ChatServerForm::kickOut()
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_KickOut;
    out.writeRawData("", 1020);

    QString name = ui->clientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientSocketHash[name];
    sock->write(sendArray);

    ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/image/redpoint.png"));
}


/* ??????????????? ???????????? */
void ChatServerForm::inviteClient()
{

    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Invite;
    out.writeRawData("", 1020);

    /* ????????? ?????? ????????? ???????????? ????????? ????????? ????????? ?????? ????????????. */
    QString name = ui->clientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientSocketHash[name];
    sock->write(sendArray);

    ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/image/bluepoint.png"));
}


void ChatServerForm::on_exitbutton_clicked()
{

    if(ui->clientTreeWidget->currentItem() !=nullptr){
        QString name = ui->clientTreeWidget->currentItem()->text(1);
        QTcpSocket* sock = clientSocketHash[name];
        if(sock != nullptr){

            QByteArray sendArray;
            QDataStream out(&sendArray, QIODevice::WriteOnly);
            out << Chat_KickOut;
            out.writeRawData("", 1020);

            sock->write(sendArray);
            ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/image/redpoint.png"));
        }
    }
    else{
        return;
    }
}




void ChatServerForm::on_connectbutton_clicked()
{
   if(ui->clientTreeWidget->currentItem() !=nullptr){

        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out << Chat_Invite;
        out.writeRawData("", 1020);

        /* ????????? ?????? ????????? ???????????? ????????? ????????? ????????? ?????? ????????????. */
        QString name = ui->clientTreeWidget->currentItem()->text(1);
        QTcpSocket* sock = clientSocketHash[name];
        if(sock != nullptr){
        sock->write(sendArray);

        ui->clientTreeWidget->currentItem()->setIcon(0, QIcon(":/image/bluepoint.png"));
    }
   }
    else{
        return;
    }
}

void ChatServerForm::on_connectbutton_customContextMenuRequested(const QPoint &pos)
{
    if(ui->clientTreeWidget->currentItem())
    {
        foreach(QAction *action, menu->actions()) {
            if(action->objectName() == "connectbutton"){
                action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) != "O");
                action->setIconText(":/image/greenpoint.png");
            }

            else
            {
                action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) == "O");
                action->setIconText(":/image/greenpoint.png");
            }

        }
        QPoint globalPos = ui->clientTreeWidget->mapToGlobal(pos);
        menu->exec(globalPos);
    }
    else{
        return;
    }
}

/* ?????? ?????? */
void ChatServerForm::readClient()
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));
    QString filename, name;

    if (byteReceived == 0) {        // ?????? ?????? ?????? : ????????? ?????? ????????? ???????????? QFile ?????? ??????
        progressDialog->reset();
        progressDialog->show();

        QString ip = receivedSocket->peerAddress().toString();
        quint16 port = receivedSocket->peerPort();
        qDebug() << ip << " : " << port;

        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name;
        progressDialog->setMaximum(totalSize);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->messageTreeWidget);
        item->setText(0, ip);
        item->setText(1, QString::number(port));
        item->setText(2, QString::number(clientIDHash[name]));
        item->setText(3, name);
        item->setText(4, filename);
        item->setText(5, QDateTime::currentDateTime().toString());
        item->setToolTip(4, filename);

        /* ???????????? ????????? QTreeWidget??? ????????? ????????? ?????? */
        for(int i = 0; i < ui->messageTreeWidget->columnCount(); i++)
            ui->messageTreeWidget->resizeColumnToContents(i);

        ui->messageTreeWidget->addTopLevelItem(item);
        logThread->appendData(item);

        QFileInfo info(filename);
        QString currentFileName = info.fileName();
        file = new QFile(currentFileName);
        file->open(QFile::WriteOnly);
    } else {                    // ?????? ???????????? ????????? ??????
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }

    progressDialog->setValue(byteReceived);

    if (byteReceived == totalSize) {        /* ????????? ??? ????????? QFile ????????? ?????? ?????? */
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();

        file->close();
        delete file;
    }
}


void ChatServerForm::on_clientTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->clientTreeWidget->currentItem()){
        foreach(QAction *action, menu->actions()) {
            if(action->objectName() == "Invite")        // ??????
                action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) != "O");
            else                                        // ??????
                action->setEnabled(ui->clientTreeWidget->currentItem()->text(0) == "O");
        }
        QPoint globalPos = ui->clientTreeWidget->mapToGlobal(pos);
        menu->exec(globalPos);
    }else {
        return;
    }


}

