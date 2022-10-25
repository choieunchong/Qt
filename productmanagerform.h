#ifndef PRODUCTMANAGERFORM_H
#define PRODUCTMANAGERFORM_H

#include <QWidget>
#include <QHash>

class ProductItem;
class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ProductManagerForm;
}

class ProductManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProductManagerForm(QWidget *parent = nullptr);
    ~ProductManagerForm();
   void loadData();

 QMap<int, ProductItem*>& getProducts(){return ProductList;}

private slots:
    /* QTreeWidget을 위한 슬롯 */
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &);
    void removeItem();              /* QAction을 위한 슬롯 */
    void on_addPushButton_clicked();
    void on_modifyPushButton_clicked();
    void on_searchPushButton_clicked();

    void addProduct(ProductItem*);

signals:
    void productAdded(ProductItem*);

    void talktoorder(QMap<int, ProductItem*>*);

private:
    Ui::ProductManagerForm *ui;
    int makeId();
    QMap<int, ProductItem*> ProductList;
    QMenu* menu;


};

#endif // PRODUCTMANAGERFORM_H
