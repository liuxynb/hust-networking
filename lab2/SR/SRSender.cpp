
#include "SRSender.h"
#include<iostream>
using namespace std;
SRSender::SRSender() :send_base(0), nextseqnum(0) {
	memset(sended, false, 8);//初始化为false
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
	sended[nextseqnum] = false;//未确认
	sndPkt[nextseqnum].acknum = -1;//忽略该字段
	sndPkt[nextseqnum].seqnum = nextseqnum;//当前包的序号即为nextseqnum
	sndPkt[nextseqnum].checksum = 0;//校验和为0
	memcpy(sndPkt[nextseqnum].payload, message.data, sizeof(message.data));
	//message打包
	sndPkt[nextseqnum].checksum = pUtils->calculateCheckSum(sndPkt[nextseqnum]);
	//计算当前包的校验和
	pUtils->printPacket("发送方发送报文", sndPkt[nextseqnum]);
	pns->stopTimer(SENDER, nextseqnum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);//为第一个元素设置时钟
	pns->sendToNetworkLayer(RECEIVER, sndPkt[nextseqnum]);//发送数据
	nextseqnum = (nextseqnum + 1) % SEQ_SIZE;//后移
	return true;
}

void SRSender::receive(const Packet& ackPkt) {
	if (send_base != nextseqnum) {//base没确认完
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((send_base + WINDOW_SIZE - 1) % SEQ_SIZE > send_base && (ackPkt.acknum >= send_base && ackPkt.acknum <= (send_base + WINDOW_SIZE - 1) % SEQ_SIZE))
				|| ((send_base + WINDOW_SIZE - 1) % SEQ_SIZE < send_base && ((ackPkt.acknum >= send_base && ackPkt.acknum <= SEQ_SIZE - 1) || ackPkt.acknum <= (send_base + WINDOW_SIZE - 1) % SEQ_SIZE)))
			{
				sended[ackPkt.acknum] = true;//标记为已确认
				pns->stopTimer(SENDER, ackPkt.acknum);
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (send_base == ackPkt.acknum) {
					int i = send_base;
					for (; i != nextseqnum;)
					{
						if (sended[i] == false) break;
						i = (i + 1) % SEQ_SIZE;
					}
					printf("发送方正确收到确认，发送窗口移动前\n");
					for (int j = send_base; j != nextseqnum; ) {
						//输出窗口
						Message msg;
						memcpy(msg.data, sndPkt[j].payload, sizeof(sndPkt[j].payload));
						msg.data[20] = '\0';
						cout << msg.data << endl;
						j = (j + 1) % SEQ_SIZE;
					}
					cout << endl;
					send_base = i;
					printf("发送方正确收到确认，发送窗口移动后\n");
					for (int j = send_base; j != nextseqnum; ) {
						//输出窗口
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
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);//为元素设置时钟
		//开始重发
	pns->sendToNetworkLayer(RECEIVER, sndPkt[seqNum]);//发送数据
	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", sndPkt[seqNum]);
}