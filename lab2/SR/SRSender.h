#include "RdtSender.h"
#include "Global.h"
#include <string.h>
#define WINDOW_SIZE 4//���ڴ�С
#define SEQ_SIZE 8//���

class SRSender :public RdtSender {
private:
	//��Ų���3λ���룬��0-7������ģ8����
	int send_base;//����δȷ�Ϸ������
	int nextseqnum;//��һ�������ͷ������
	Packet sndPkt[SEQ_SIZE];
	bool sended[SEQ_SIZE];

public:
	bool send(const Message& message);	//����Ӧ�ò�������Message����NetworkService����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ�ȷ��״̬���ʹ����������ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);	//����ȷ��Ack������NetworkService����	
	void timeoutHandler(int seqNum);    //Timeout handler������NetworkService����
	bool getWaitingState();				//����RdtSender�Ƿ��ڵȴ�״̬��������ͷ����ȴ�ȷ�ϻ��߷��ʹ�������������true
public:
	SRSender();
	~SRSender();
};
