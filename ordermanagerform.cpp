#include "ordermanagerform.h"
#include "orderitem.h"
#include "ui_ordermanagerform.h"
#include "clientitem.h"
#include "productitem.h"
#include <QMenu>
#include <QFile>
#include <QString>
#include <QTreeWidgetItem>

OrderManagerForm::OrderManagerForm(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::OrderManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 540 << 400;
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));
    connect(ui->addPushButton, SIGNAL(pressed()), this, SLOT(on_addButton_Clicked()));
    connect(ui->CountLineEdit, SIGNAL(textChanged(QString)),this, SLOT(total(QString)));
}

void OrderManagerForm::addProduct(ProductItem* pl)
{
    tree = new QTreeWidgetItem;
    tree->setText(0, QString::number((pl->id())));
    tree->setText(1, pl->getProductName());
    tree->setText(2,pl->getProductType());
    tree->setText(3, QString::number(pl->getProductPrice()));
    tree->setText(4, QString::number((pl->getStock())));
    ui->ProductW->addTopLevelItem(tree);

}



void OrderManagerForm::addClient(ClientItem* cl)
{
    tree = new QTreeWidgetItem;
    tree->setText(0, QString::number((cl->id())));
    tree->setText(1, cl->getName());
    tree->setText(2, cl->getPhoneNumber());
    tree->setText(3, cl->getAddress());
    ui->ClientW->addTopLevelItem(tree);
}

void OrderManagerForm::total(QString num)
{
    int b= ui->PriceLineEdit->text().toInt() * ui->CountLineEdit->text().toInt();
    ui->TotalLineEdit->setText(QString::number(b));
}

void OrderManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}

void OrderManagerForm::on_addButton_Clicked()
{
    QString ClientName,ProductName;
    int Price, Count, Total;//= ;;
    int id = makeId();
    ClientName = ui->ClientNameLineEdit->text();
    ProductName = ui->ProductNameLineEdit->text();
    Price = ui->PriceLineEdit->text().toInt();
    Count = ui->CountLineEdit->text().toInt();
    int c = Price * Count;
    Total = c;

    if(ClientName.length()) {
        orderitem* o = new orderitem(id, ClientName, ProductName, Price, Count,Total);
        OrderList.insert(id, o);
        ui->treeWidget->addTopLevelItem(o);
        emit orderAdded(o);
    }
}

int OrderManagerForm::makeId()
{
    if(OrderList.size( ) == 0) {
        return 100;
    } else {
        auto id = OrderList.lastKey();
        return ++id;
    }
}

void OrderManagerForm::loadData()
{
    QFile file("orderlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            orderitem* c = new orderitem(id, row[1], row[2], row[3].toInt(), row[4].toInt(),row[3].toInt()*row[4].toInt());
            ui->treeWidget->addTopLevelItem(c);
            OrderList.insert(id, c);

            emit orderAdded(c);
        }
    }
    file.close( );
}

OrderManagerForm::~OrderManagerForm()
{
    delete ui;

    QFile file("orderlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : OrderList) {
        orderitem* c = v;
        out << c->id() << ", ";
        out << c->getName() << ", ";
        out << c->getProductName() << ", ";
        out << c->getPrice() << ", ";
        out << c->getCount() <<", ";
        out << c->getTotal() << ", \n";
    }
    file.close( );
}


void OrderManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));
    ui->ClientNameLineEdit->setText(item->text(1));
    ui->ProductNameLineEdit->setText(item->text(2));
    ui->PriceLineEdit->setText(item->text(3));
    ui->toolBox->setCurrentIndex(0);
}

void OrderManagerForm::removeItem()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        OrderList.remove(item->text(0).toInt());
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
        ui->treeWidget->update();
    }
}

void OrderManagerForm::on_searchPushButton_clicked()
{
    ui->searchTreeWidget->clear();
    //    for(int i = 0; i < ui->treeWidget->columnCount(); i++)
    int i = ui->searchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->treeWidget->findItems(ui->searchLineEdit->text(), flag, i);

        foreach(auto i, items) {
            ClientItem* c= static_cast<ClientItem*>(i);
            ProductItem* p = static_cast<ProductItem*>(i);
            orderitem *o = static_cast<orderitem*>(i);

            int id = c->id();
            QString ClientName = c->getName();
            QString ProductName = p->getProductName();
            int price = p->getProductPrice();
            int count = o->getCount();
            int total = o->getTotal();

            orderitem* item = new orderitem(id, ClientName, ProductName,price, count, total);
            ui->searchTreeWidget->addTopLevelItem(item);
        }
    }
}

void OrderManagerForm::on_ClientW_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));
    ui->ClientNameLineEdit->setText(item->text(1));
}


void OrderManagerForm::on_ProductW_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->ProductNameLineEdit->setText(item->text(2));
    ui->PriceLineEdit->setText(item->text(3));
}

