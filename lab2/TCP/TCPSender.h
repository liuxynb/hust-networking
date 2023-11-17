#include "RdtSender.h" 
#define WINDOW_SIZE 4 //窗口长度
#define SEQ_SIZE 8 //序号的最大值, 窗格大小必須是序號大小的一半
class TCPSender :public RdtSender
{
private:
    int base; //基序号,最早未确认的报文序号
    int nextseqnum; //下一个发送序号
    Packet sendPKT[SEQ_SIZE];
    int repeatNum; //重复发送的报文数

public:
    bool send(const Message& message); //发送应用层下来的Message
    void receive(const Packet& ackPkt); //接受确认Ack,将被NetworkService调用,NetworkService通过调用sendToNetworkLayer()将Ack发送给发送方
    void timeoutHandler(int seqNum); //Timeout handler，将被NetworkService调用
    bool getWaitingState(); //返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true

public:
    TCPSender();//构造函数
    virtual ~TCPSender();
};

