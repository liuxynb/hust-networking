#include "RdtReceiver.h"
#define WINDOW_SIZE 4//���ڳ���
#define SEQ_SIZE 8 //��ŵ����ֵ

class TCPReceiver :public RdtReceiver
{
private:
    int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
    Packet ackPkt;			// ����ȷ�ϵ�ACK���ģ�ֻ��Ҫһ������
public:
    TCPReceiver(/* args */);
    ~TCPReceiver();
    void receive(const Packet& packet);	//���ձ���
};
