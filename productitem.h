#ifndef PRODUCTITEM_H
#define PRODUCTITEM_H

#include <QTreeWidgetItem>


class ProductItem : public QTreeWidgetItem
{
public:
  explicit  ProductItem(int id = 0,  QString Productname = "", QString type = "", int productprice = 0, int Stock = 0);

    QString getProductName() const;
    void setProductName(QString&);
    int getProductPrice()const ;
    void setProductPrice(int&);
    QString getProductType() const;
    void  setProductType(QString&);
    int getStock() const;
    void setStock(int&);
    int id() const;
    bool operator==(const ProductItem &other) const;

};

#endif // PRODUCTITEM_H
