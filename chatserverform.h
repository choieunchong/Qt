#ifndef CHATSERVERFORM_H
#define CHATSERVERFORM_H

#include <QWidget>
#include <QList>
#include <QHash>


class QLabel;
class QTcpServer;
class QTcpSocket;
class QFile;
class QProgressDialog;
class LogThread;

namespace Ui {
class chatserverForm;
}

typedef struct {
    int type;
    char data[1020];
} chatProtocolType;

typedef enum {
    Chat_Login,             // 로그인(서버 접속)   --> 초대를 위한 정보 저장
    Chat_In,                // 채팅방 입장
    Chat_Talk,              // 채팅
    Chat_Out,               // 채팅방 퇴장         --> 초대 가능
    Chat_LogOut,            // 로그 아웃(서버 단절) --> 초대 불가능
    Chat_Invite,            // 초대
    Chat_KickOut,           // 강퇴
   // Chat_Chatting  //실시간 채팅
} Chat_Status;

class ChatServerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChatServerForm(QWidget *parent = nullptr);
    ~ChatServerForm();

private:
    const int BLOCK_SIZE = 1024;
    const int PORT_NUMBER = 8000; //포트번호 고정

    Ui::chatserverForm *ui;
    QTcpServer *chatServer; //채팅서버
    QTcpServer *fileServer; // 파일 서버
   // QTcpServer *

    QList<QTcpSocket*> clientList; //고객 리스트를 담기위해 사용
    QList<int> clientIDList;
    QHash<quint16, QString> clientNameHash;
    QHash<QString, QTcpSocket*> clientSocketHash;
    QHash<QString, int> clientIDHash;
    QMenu* menu;
    QFile* file;
    QProgressDialog* progressDialog;
    qint64 totalSize;
    qint64 byteReceived;
    QByteArray inBlock;
    LogThread* logThread;

private slots:

    void acceptConnection();                /* 파일 서버 */
    void readClient();
    void clientConnect( );                       /* 채팅 서버 */
    void receiveData( );
    void removeClient( );
    void addClient(int, QString);
    void inviteClient();
    void kickOut();


    void on_exitbutton_clicked();
    void on_connectbutton_clicked();
    void on_connectbutton_customContextMenuRequested(const QPoint &pos);
    void on_clientTreeWidget_customContextMenuRequested(const QPoint &pos);
};

#endif // CHATSERVERFORM_H
