
#include "SRReceiver.h"
#include<iostream>
using namespace std;
SRReceiver::SRReceiver() :recv_base(0) {
	memset(recv, false, 8);
	ackPkt.acknum = -1;//初始化为-1
	ackPkt.checksum = 0;
	ackPkt.seqnum = -1;//忽略
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		ackPkt.payload[i] = '.';
	}
	ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
}
SRReceiver::~SRReceiver() {

}
void SRReceiver::receive(const Packet& packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum) {//
		pUtils->printPacket("接收方正确收到发送方的报文", packet);

		if(((recv_base+WINDOW_SIZE-1)%SEQ_SIZE>recv_base&&(packet.seqnum>=recv_base&& packet.seqnum<= (recv_base + WINDOW_SIZE- 1) % SEQ_SIZE))
			|| ((recv_base + WINDOW_SIZE - 1) % SEQ_SIZE < recv_base &&((packet.seqnum>=recv_base&& packet.seqnum<=SEQ_SIZE-1)|| packet.seqnum<= (recv_base + WINDOW_SIZE - 1) % SEQ_SIZE )))
		 {
			ackPkt.acknum = packet.seqnum;  //确认序号等于收到的报文序号
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("接收方发送确认报文", ackPkt);
			pns->sendToNetworkLayer(SENDER, ackPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
			if (recv[packet.seqnum] == false) {
				recvPkt[packet.seqnum] = packet;//存入
				recv[packet.seqnum] = true;//已收到
			}
			if (packet.seqnum == recv_base) {
				int i = recv_base;
				for (; i != (recv_base + WINDOW_SIZE)%SEQ_SIZE;i = (i + 1) % SEQ_SIZE) {
					if (recv[i] == true)//已收到
					{
						//取出Message，向上递交给应用层
						Message msg;
						memcpy(msg.data, recvPkt[i].payload, sizeof(recvPkt[i].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						recv[i] = false;//标记为未收到		
					}
					else {
						break;
					}
				}
				cout << "接收窗口移动前：" << endl;
				for (int j = recv_base; j != (recv_base+WINDOW_SIZE)%SEQ_SIZE; j = (j + 1) % SEQ_SIZE) {
					//输出窗口

						Message msg;
						memcpy(msg.data, recvPkt[j].payload, sizeof(recvPkt[j].payload));
						msg.data[20] = '\0';
						cout << msg.data << endl;	
				}
				cout << endl;
				recv_base = i;
				cout << "接收窗口移动后：" << endl;
				for (int j = recv_base; j != (recv_base + WINDOW_SIZE) % SEQ_SIZE; j = (j + 1) % SEQ_SIZE) {
					//输出窗口

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
			ackPkt.acknum = packet.seqnum;  //确认序号等于收到的报文序号
			ackPkt.checksum = pUtils->calculateCheckSum(ackPkt);
			pUtils->printPacket("接收方发送确认报文", ackPkt);
			pns->sendToNetworkLayer(SENDER, ackPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		}


	}
}