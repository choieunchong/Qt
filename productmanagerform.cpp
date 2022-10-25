#include "productmanagerform.h"
#include "productitem.h"
#include "ui_productmanagerform.h"

#include <QMenu>
#include <QFile>
#include <QString>

ProductManagerForm::ProductManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductManagerForm)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 640 << 300;
    ui->splitter->setSizes(sizes);

    QAction* removeAction = new QAction(tr("&Remove"));
    connect(removeAction, SIGNAL(triggered()), SLOT(removeItem()));

    menu = new QMenu;
    menu->addAction(removeAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->searchLineEdit, SIGNAL(returnPressed()),
            this, SLOT(on_searchPushButton_clicked()));
}


void ProductManagerForm::addProduct(ProductItem*)
{
    emit talktoorder(&ProductList);
}

void ProductManagerForm::loadData()
{
    QFile file("productlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            ProductItem* c = new ProductItem(id, row[1], row[2],row[3].toInt(), row[4].toInt());
            ui->treeWidget->addTopLevelItem(c);
            ProductList.insert(id, c);

            emit productAdded(c);
        }
    }
    file.close( );
}

ProductManagerForm::~ProductManagerForm()
{
    delete ui;

    QFile file("productlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : ProductList) {
        ProductItem* c = v;
        out << c->id() << ", " << c->getProductName() << ", ";
        out << c->getProductType() << ", ";
        out << c->getProductPrice() << ", ";
        out << c->getStock() << "\n";
    }
    file.close( );
}


int ProductManagerForm::makeId( )
{
    if(ProductList.size( ) == 0) {
        return 100;
    } else {
        auto id = ProductList.lastKey();
        return ++id;
    }
}

void ProductManagerForm::removeItem()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        ProductList.remove(item->text(0).toInt());
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
//        delete item;
        ui->treeWidget->update();
    }
}


void ProductManagerForm::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    menu->exec(globalPos);
}



void ProductManagerForm::on_searchPushButton_clicked()
{
    ui->searchTreeWidget->clear();
//    for(int i = 0; i < ui->treeWidget->columnCount(); i++)
    int i = ui->searchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->treeWidget->findItems(ui->searchLineEdit->text(), flag, i);

        foreach(auto i, items) {
            ProductItem* c = static_cast<ProductItem*>(i);

            int id = c->id();
            QString name = c->getProductName();
            QString type = c->getProductType();
            int price = c->getProductPrice();
            int stock = c->getStock();

            ProductItem* item = new ProductItem(id, name, type,price, stock);
            ui->searchTreeWidget->addTopLevelItem(item);
        }
    }
}


void ProductManagerForm::on_addPushButton_clicked()
{
    QString name, type, price, stock;
    int id = makeId( );
    name = ui->ProductNameLineEdit->text();
    type = ui->typeLineEdit->text();
    price = ui->PriceLineEdit->text();
    stock = ui-> stockLineEdit->text();
    if(name.length()) {
        ProductItem* c = new ProductItem(id, name, type, price.toInt(), stock.toInt());
        ProductList.insert(id, c);
        ui->treeWidget->addTopLevelItem(c);
        emit productAdded(c);
    }
}

void ProductManagerForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->idLineEdit->setText(item->text(0));
    ui->ProductNameLineEdit->setText(item->text(1));
    ui->typeLineEdit->setText(item->text(2));
    ui->PriceLineEdit->setText(item->text(3));
       ui->stockLineEdit->setText(item->text(4));
    ui->toolBox->setCurrentIndex(0);
}

void ProductManagerForm::on_modifyPushButton_clicked()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        ProductItem* c = ProductList[key];
        QString name, type;
        int price, stock;
        name = ui->ProductNameLineEdit->text();
        type = ui->typeLineEdit->text();
        price = ui->PriceLineEdit->text().toInt();
        stock = ui->stockLineEdit->text().toInt();
        c->setProductName(name);
        c->setProductType(type);
        c->setProductPrice(price);
        c->setStock(stock);
        ProductList[key] = c;
    }
}












