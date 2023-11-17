
#include "SRReceiver.h"
#include<iostream>
using namespace std;
SRReceiver::SRReceiver() :recv_base(0) {
	memset(recv, false, 8);
	ackPkt.acknum = -1;//��ʼ��Ϊ-1
	ackPkt.checksum = 0;
	ackPkt.seqnum = -1;//����
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackPkt.payload[i] = '.';
	}
	ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
}
SRReceiver::~SRReceiver() {

}
void SRReceiver::receive(const Packet& packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);
	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum) {//
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

		if(((recv_base+WINDOW_SIZE-1)%SEQ_SIZE>recv_base&&(packet.seqnum>=recv_base&& packet.seqnum<= (recv_base + WINDOW_SIZE- 1) % SEQ_SIZE))
			|| ((recv_base + WINDOW_SIZE - 1) % SEQ_SIZE < recv_base &&((packet.seqnum>=recv_base&& packet.seqnum<=SEQ_SIZE-1)|| packet.seqnum<= (recv_base + WINDOW_SIZE - 1) % SEQ_SIZE )))
		 {
			ackPkt.acknum = packet.seqnum;  //ȷ����ŵ����յ��ı������
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", ackPkt);
			pns->sendToNetworkLayer(SENDER, ackPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
			if (recv[packet.seqnum] == false) {
				recvPkt[packet.seqnum] = packet;//����
				recv[packet.seqnum] = true;//���յ�
			}
			if (packet.seqnum == recv_base) {
				int i = recv_base;
				for (; i != (recv_base + WINDOW_SIZE)%SEQ_SIZE;i = (i + 1) % SEQ_SIZE) {
					if (recv[i] == true)//���յ�
					{
						//ȡ��Message�����ϵݽ���Ӧ�ò�
						Message msg;
						memcpy(msg.data, recvPkt[i].payload, sizeof(recvPkt[i].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						recv[i] = false;//���Ϊδ�յ�		
					}
					else {
						break;
					}
				}
				cout << "���մ����ƶ�ǰ��" << endl;
				for (int j = recv_base; j != (recv_base+WINDOW_SIZE)%SEQ_SIZE; j = (j + 1) % SEQ_SIZE) {
					//�������

						Message msg;
						memcpy(msg.data, recvPkt[j].payload, sizeof(recvPkt[j].payload));
						msg.data[20] = '\0';
						cout << msg.data << endl;	
				}
				cout << endl;
				recv_base = i;
				cout << "���մ����ƶ���" << endl;
				for (int j = recv_base; j != (recv_base + WINDOW_SIZE) % SEQ_SIZE; j = (j + 1) % SEQ_SIZE) {
					//�������

					Message msg;
					memcpy(msg.data, recvPkt[j].payload, sizeof(recvPkt[j].payload));
					msg.data[20] = '\0';
					cout << msg.data << endl;
				}
				cout << endl;
			}
		}
		else if (((recv_base - WINDOW_SIZE+8) % SEQ_SIZE <(recv_base-1+8)%SEQ_SIZE && (packet.seqnum >= (recv_base - WINDOW_SIZE+8) % SEQ_SIZE && packet.seqnum <= (recv_base - 1+8) % SEQ_SIZE))
			|| ((recv_base - WINDOW_SIZE+8) % SEQ_SIZE > (recv_base - 1+8) % SEQ_SIZE && ((packet.seqnum >= (recv_base - WINDOW_SIZE+8) % SEQ_SIZE && packet.seqnum <= SEQ_SIZE - 1) || packet.seqnum <= (recv_base - 1+8) % SEQ_SIZE)))
		{
			ackPkt.acknum = packet.seqnum;  //ȷ����ŵ����յ��ı������
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", ackPkt);
			pns->sendToNetworkLayer(SENDER, ackPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		}


	}
}