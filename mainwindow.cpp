#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanagerform.h"
#include "productmanagerform.h"
#include "ordermanagerform.h"
#include "chatserverform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //    ClientManagerForm *clientForm1 = new ClientManagerForm(0);
    //    clientForm1->show();

    orderForm = new OrderManagerForm(this);
    orderForm->loadData();
    orderForm->setWindowTitle(tr("Order Info"));

    connect(orderForm, SIGNAL(destroyed()),
            orderForm, SLOT(deleteLater()));

    clientForm = new ClientManagerForm(this);
    connect(clientForm, SIGNAL(destroyed()),
            clientForm, SLOT(deleteLater()));
    connect(clientForm, SIGNAL(clientAdded(ClientItem*)),
            orderForm, SLOT(addClient(ClientItem*)));


    clientForm->setWindowTitle(tr("Client Info"));
    //    ui->tabWidget->addTab(clientForm, "&Client Info");

    productForm = new ProductManagerForm(this);
    connect(productForm, SIGNAL(destroyed()),
            productForm, SLOT(deleteLater()));
    connect(productForm, SIGNAL(productAdded(ProductItem*)),
            orderForm, SLOT(addProduct(ProductItem*)));

    chatserverForm = new ChatServerForm(this);
    productForm->loadData();
    productForm->setWindowTitle(tr("Product Info"));

    orderForm->client = clientForm;
    orderForm->product = productForm;


    connect(chatserverForm, SIGNAL(destroyed()),
            chatserverForm, SLOT(deleteLater()));

    connect(clientForm, SIGNAL(tcpClient(int, QString)),
            chatserverForm, SLOT(addClient(int, QString)));

    clientForm->loadData();

    chatserverForm->setWindowTitle(tr("chat Info"));
    QMdiSubWindow *cw = ui->mdiArea->addSubWindow(clientForm);
    ui->mdiArea->addSubWindow(productForm);
    ui->mdiArea->addSubWindow(orderForm);
    ui->mdiArea->addSubWindow(chatserverForm);
    ui->mdiArea->setActiveSubWindow(cw);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionClient_triggered()
{
    if(clientForm != nullptr) {
        clientForm->setFocus();
    }
}

void MainWindow::on_actionProduct_triggered()
{
    if(productForm != nullptr) {
        productForm->setFocus();
    }
}


void MainWindow::on_actionOrder_triggered()
{
    if(orderForm != nullptr) {
        orderForm->setFocus();
    }
}


void MainWindow::on_actionServer_triggered()
{
    if(chatserverForm != nullptr) {
        chatserverForm->setFocus();
    }
}

