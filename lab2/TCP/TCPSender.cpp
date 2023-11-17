#include "TCPSender.h"//TCPSender�������
#include "Global.h"
#include "stdafx.h"
TCPSender::TCPSender() :base(0), nextseqnum(0),repeatNum(0)
{
}
TCPSender::~TCPSender()
{
}
bool TCPSender::getWaitingState()//����RdtSender�Ƿ��ڵȴ�״̬��������ͷ����ȴ�ȷ�ϻ��߷��ʹ�������������true
{
    if (nextseqnum == (base + WINDOW_SIZE) % SEQ_SIZE)
        return true;
    else
        return false;
}

bool TCPSender::send(const Message& message) //����Ӧ�ò�������Message
{
    if (getWaitingState())
        return false;
    sendPKT[nextseqnum].acknum = -1;//���Ը��ֶ�
    sendPKT[nextseqnum].seqnum = nextseqnum; //��ǰ�������
    sendPKT[nextseqnum].checksum = 0; //�Ƚ�У�����Ϊ0
    memcpy(sendPKT[nextseqnum].payload, message.data, sizeof(message.data));
    //�������message�������͵İ���
    sendPKT[nextseqnum].checksum = pUtils->calculateCheckSum(sendPKT[nextseqnum]);//����У���
    pUtils->printPacket("���ͷ����ͱ���", sendPKT[nextseqnum]);//����ģ�⹤�ߺ������͸ñ���
    if (base % WINDOW_SIZE == nextseqnum % WINDOW_SIZE)//���ͷ����ڵȴ�ȷ��״̬,base % WINDOW_SIZE == nextseqnum % WINDOW_SIZE˵����������
    {
        pns->stopTimer(SENDER, 1);
        pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
    }
    pns->sendToNetworkLayer(RECEIVER, sendPKT[nextseqnum]);//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
    nextseqnum = (nextseqnum + 1) % SEQ_SIZE;//�����������1
    return true;
}

void TCPSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) 
    {//baseûȷ����
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((base + WINDOW_SIZE - 1) % SEQ_SIZE > base && (ackPkt.acknum >= base && ackPkt.acknum <= (base + WINDOW_SIZE - 1) % SEQ_SIZE))
				|| ((base + WINDOW_SIZE - 1) % SEQ_SIZE < base && ((ackPkt.acknum >= base && ackPkt.acknum <= SEQ_SIZE - 1) || ackPkt.acknum <= (base + WINDOW_SIZE - 1) % SEQ_SIZE)))
			{
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��\n", ackPkt);
                printf("���ͷ���ȷ�յ�ȷ�ϣ����ʹ����ƶ�ǰ\n");
                for (int j = base; j != nextseqnum; j = (j + 1) % SEQ_SIZE) {
                    //�������
                    Message msg;
                    memcpy(msg.data, sendPKT[j].payload, sizeof(sendPKT[j].payload));
                    msg.data[21] = '\0';
                    cout << msg.data << endl;                
                }
                cout << endl;
                base = (ackPkt.acknum + 1) % SEQ_SIZE;//����base
                printf("���ͷ���ȷ�յ�ȷ�ϣ����ʹ����ƶ���\n");
                for (int j = base; j != nextseqnum; j = (j + 1) % SEQ_SIZE) {
                    //�������
                    Message msg;
                    memcpy(msg.data, sendPKT[j].payload, sizeof(sendPKT[j].payload));
                    msg.data[21] = '\0';
                    cout << msg.data << endl;
                }
                cout << endl;
                if (base == nextseqnum) {
                    pns->stopTimer(SENDER, 1);//�رն�ʱ��
                }
                else {
                    pns->stopTimer(SENDER, 1);
                    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
                }
			}
            else
            {
                repeatNum++;
                if(repeatNum == 3)
                {
                    pUtils->printPacket("�յ���������ack���ط�����\n", sendPKT[base]);
                    pns->sendToNetworkLayer(RECEIVER, sendPKT[base]);//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
                    pns->stopTimer(SENDER, 1);//�رն�ʱ��,����������ʱ��,ע��Ѽ�ʱ����seqnumҲҪ����,������1,��ʾ��Ӧsendpkt[0],����Ӧbase
                    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
                    repeatNum = 0;
                }
            }
		}
	}
}

void TCPSender::timeoutHandler(int seqNum) //Timeout handler������NetworkService����
{
    pns->stopTimer(SENDER, 1);//�رն�ʱ��
    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//������ʱ��
    pns->sendToNetworkLayer(RECEIVER, sendPKT[base]);
    pUtils->printPacket("���ͷ����·��ͱ���", sendPKT[base]);

}