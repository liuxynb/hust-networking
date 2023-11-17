#include "RdtReceiver.h"
#define WINDOW_SIZE 4//窗口长度
#define SEQ_SIZE 8 //序号的最大值

class GBNReceiver :public RdtReceiver
{
private:
    int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
    Packet ackPkt;			// 发送确认的ACK报文，只需要一个即可
public:
    GBNReceiver(/* args */);
    ~GBNReceiver();
    void receive(const Packet& packet);	//接收报文
};
