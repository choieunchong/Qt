#ifndef ORDERITEM_H
#define ORDERITEM_H

#include <QTreeWidgetItem>

class orderitem  : public QTreeWidgetItem
{

public:
    explicit orderitem(int id = 0, QString Clientname= "", QString ProductName= "", int Price= 0, int Count= 0, int Total= 0);


    QString getName()const;
    void setName(QString&);
    QString getProductName()const;
    void setProductName(QString&);
    int id() const;
    int getPrice()const;
    void setPrice(int&);
    int getCount()const;
    void setCount(int&);
    int getTotal()const;
    void setTotal(int&);
    bool operator==(const orderitem &other) const;

};

#endif // ORDERITEM_H
