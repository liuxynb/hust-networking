#include "RdtReceiver.h"
#define WINDOW_SIZE 4//���ڳ���
#define SEQ_SIZE 8 //��ŵ����ֵ

class GBNReceiver :public RdtReceiver
{
private:
    int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
    Packet ackPkt;			// ����ȷ�ϵ�ACK���ģ�ֻ��Ҫһ������
public:
    GBNReceiver(/* args */);
    ~GBNReceiver();
    void receive(const Packet& packet);	//���ձ���
};
