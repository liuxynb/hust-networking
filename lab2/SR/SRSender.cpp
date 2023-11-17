
#include "SRSender.h"
#include<iostream>
using namespace std;
SRSender::SRSender() :send_base(0), nextseqnum(0) {
	memset(sended, false, 8);//��ʼ��Ϊfalse
}

SRSender::~SRSender() {

}

bool SRSender::getWaitingState() {
	if (nextseqnum == (send_base + 4) % 8) {
		return true;
	}
	else return false;
}

bool SRSender::send(const Message& message) {
	if (getWaitingState() == true)
		return false;
	sended[nextseqnum] = false;//δȷ��
	sndPkt[nextseqnum].acknum = -1;//���Ը��ֶ�
	sndPkt[nextseqnum].seqnum = nextseqnum;//��ǰ������ż�Ϊnextseqnum
	sndPkt[nextseqnum].checksum = 0;//У���Ϊ0
	memcpy(sndPkt[nextseqnum].payload, message.data, sizeof(message.data));
	//message���
	sndPkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndPkt[nextseqnum]);
	//���㵱ǰ����У���
	pUtils->printPacket("���ͷ����ͱ���", sndPkt[nextseqnum]);
	pns->stopTimer(SENDER, nextseqnum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//Ϊ��һ��Ԫ������ʱ��
	pns->sendToNetworkLayer(RECEIVER, sndPkt[nextseqnum]);//��������
	nextseqnum = (nextseqnum + 1) % SEQ_SIZE;//����
	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	if (send_base != nextseqnum) {//baseûȷ����
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((send_base + WINDOW_SIZE - 1) % SEQ_SIZE > send_base && (ackPkt.acknum >= send_base && ackPkt.acknum <= (send_base + WINDOW_SIZE - 1) % SEQ_SIZE))
				|| ((send_base + WINDOW_SIZE - 1) % SEQ_SIZE < send_base && ((ackPkt.acknum >= send_base && ackPkt.acknum <= SEQ_SIZE - 1) || ackPkt.acknum <= (send_base + WINDOW_SIZE - 1) % SEQ_SIZE)))
			{
				sended[ackPkt.acknum] = true;//���Ϊ��ȷ��
				pns->stopTimer(SENDER, ackPkt.acknum);
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				if (send_base == ackPkt.acknum) {
					int i = send_base;
					for (; i != nextseqnum;)
					{
						if (sended[i] == false) break;
						i = (i + 1) % SEQ_SIZE;
					}
					printf("���ͷ���ȷ�յ�ȷ�ϣ����ʹ����ƶ�ǰ\n");
					for (int j = send_base; j != nextseqnum; ) {
						//�������
						Message msg;
						memcpy(msg.data, sndPkt[j].payload, sizeof(sndPkt[j].payload));
						msg.data[20] = '\0';
						cout << msg.data << endl;
						j = (j + 1) % SEQ_SIZE;
					}
					cout << endl;
					send_base = i;
					printf("���ͷ���ȷ�յ�ȷ�ϣ����ʹ����ƶ���\n");
					for (int j = send_base; j != nextseqnum; ) {
						//�������
						Message msg;
						memcpy(msg.data, sndPkt[j].payload, sizeof(sndPkt[j].payload));
						msg.data[20] = '\0';
						cout << msg.data << endl;
						j = (j + 1) % SEQ_SIZE;
					}
					cout << endl;
				}
			}
		}
	}

}

void SRSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);//ΪԪ������ʱ��
		//��ʼ�ط�
	pns->sendToNetworkLayer(RECEIVER, sndPkt[seqNum]);//��������
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", sndPkt[seqNum]);
}