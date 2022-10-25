#ifndef ORDERMANAGERFORM_H
#define ORDERMANAGERFORM_H

#include <QWidget>
#include "clientmanagerform.h"
#include "productmanagerform.h"

class orderitem;
class QMenu;
class QTreeWidgetItem;

namespace Ui {
class OrderManagerForm;

}


class OrderManagerForm : public QWidget
{
    Q_OBJECT
public:
    explicit OrderManagerForm(QWidget *parent = nullptr);
    ~OrderManagerForm();
    void loadData();
    ClientManagerForm *client;
    ProductManagerForm *product;

private slots:

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_addButton_Clicked();
    void on_searchPushButton_clicked();
    void showContextMenu(const QPoint &);
    void removeItem();
    void addProduct(ProductItem*);
    void total(QString);
     void addClient(ClientItem*);
     void on_ClientW_itemClicked(QTreeWidgetItem *item, int column);

     void on_ProductW_itemClicked(QTreeWidgetItem *item, int column);

signals:
    void orderAdded(orderitem*);
    void datasant(QString);
private:
    Ui::OrderManagerForm* ui;

    QMap<int, orderitem*> OrderList;
    int makeId();
    QMenu* menu;
    QTreeWidgetItem* tree;


};

#endif // ORDERMANAGERFORM_H
