#include "orderitem.h"

orderitem::orderitem(int id, QString Clientname,QString ProductName, int Price, int Count, int Total)
{
    setText(0, QString::number(id));
    setText(1, Clientname);
    setText(2, ProductName);
    setText(3, QString::number(Price));
    setText(4, QString::number(Count));
    setText(5, QString::number(Total));
}

int orderitem::id() const
{
    return text(0).toInt();
}

QString orderitem::getName() const
{
    return text(1);
}

void orderitem::setName(QString& ClientName)
{
    setText(1, ClientName);
}

QString orderitem::getProductName()const
{
    return text(2);
}

void orderitem::setProductName(QString& ProductName)
{
    setText(2, ProductName);
}

int orderitem::getPrice()const
{
    return text(3).toInt();
}

void orderitem::setPrice(int &price)
{
    setText(3,QString::number(price));
}

int orderitem::getCount()const
{
    return text(4).toInt();
}

void orderitem::setCount(int &count)
{
    setText(4,QString::number(count));
}

int orderitem::getTotal()const
{
    return text(5).toInt();
}

void orderitem::setTotal(int &count)
{
    setText(5,QString::number(count));
}

