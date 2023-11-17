#include "RdtSender.h" 
#define WINDOW_SIZE 4 //���ڳ���
#define SEQ_SIZE 8 //��ŵ����ֵ, �����С�������̖��С��һ��
class GBNSender :public RdtSender
{
private:
    int base; //�����,����δȷ�ϵı������
    int nextseqnum; //��һ���������
    int windowSize; //���ڳ���
    Packet sendPKT[SEQ_SIZE];

public:
    bool send(const Message& message); //����Ӧ�ò�������Message
    void receive(const Packet& ackPkt); //����ȷ��Ack,����NetworkService����,NetworkServiceͨ������sendToNetworkLayer()��Ack���͸����ͷ�
    void timeoutHandler(int seqNum); //Timeout handler������NetworkService����
    bool getWaitingState(); //����RdtSender�Ƿ��ڵȴ�״̬��������ͷ����ȴ�ȷ�ϻ��߷��ʹ�������������true

public:
    GBNSender();//���캯��
    virtual ~GBNSender();
};

