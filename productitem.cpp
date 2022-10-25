#include "productitem.h"

ProductItem::ProductItem(int id ,  QString Productname , QString type , int productprice , int Stock )
{
    setText(0, QString::number(id));
    setText(1, Productname);
    setText(2, type);
    setText(3, QString::number(productprice));
    setText(4, QString::number(Stock));
}

QString ProductItem::getProductName()const
{
    return text(1);
}

void ProductItem::setProductName(QString& name)
{
    setText(1, name);
}


QString ProductItem::getProductType() const
{
    return text(2);
}

void ProductItem::setProductType(QString& type)
{
     setText(2, type);
}

int ProductItem::getProductPrice()const
{
    return text(3).toInt();
}

void ProductItem::setProductPrice(int& price)
{
   setText(3,QString::number(price));
}

int ProductItem::getStock()const
{
    return text(4).toInt();
}
void ProductItem::setStock(int& stock)
{
    setText(4,QString::number(stock));

}

bool ProductItem::operator==(const ProductItem &other) const{
    return (this->text(1)== other.text(1));
}



int ProductItem::id() const
{
    return text(0).toInt();
}











