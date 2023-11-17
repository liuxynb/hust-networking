#include "TCPSender.h"//TCPSender类的声明
#include "Global.h"
#include "stdafx.h"
TCPSender::TCPSender() :base(0), nextseqnum(0),repeatNum(0)
{
}
TCPSender::~TCPSender()
{
}
bool TCPSender::getWaitingState()//返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true
{
    if (nextseqnum == (base + WINDOW_SIZE) % SEQ_SIZE)
        return true;
    else
        return false;
}

bool TCPSender::send(const Message& message) //发送应用层下来的Message
{
    if (getWaitingState())
        return false;
    sendPKT[nextseqnum].acknum = -1;//忽略该字段
    sendPKT[nextseqnum].seqnum = nextseqnum; //当前包的序号
    sendPKT[nextseqnum].checksum = 0; //先将校验和置为0
    memcpy(sendPKT[nextseqnum].payload, message.data, sizeof(message.data));
    //打包数据message到待发送的包中
    sendPKT[nextseqnum].checksum = pUtils->calculateCheckSum(sendPKT[nextseqnum]);//计算校验和
    pUtils->printPacket("发送方发送报文", sendPKT[nextseqnum]);//调用模拟工具函数发送该报文
    if (base % WINDOW_SIZE == nextseqnum % WINDOW_SIZE)//发送方处于等待确认状态,base % WINDOW_SIZE == nextseqnum % WINDOW_SIZE说明窗口满了
    {
        pns->stopTimer(SENDER, 1);
        pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
    }
    pns->sendToNetworkLayer(RECEIVER, sendPKT[nextseqnum]);//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
    nextseqnum = (nextseqnum + 1) % SEQ_SIZE;//发送序号增加1
    return true;
}

void TCPSender::receive(const Packet& ackPkt) {
	if (base != nextseqnum) 
    {//base没确认完
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);
		if (checkSum == ackPkt.checksum) {
			if (((base + WINDOW_SIZE - 1) % SEQ_SIZE > base && (ackPkt.acknum >= base && ackPkt.acknum <= (base + WINDOW_SIZE - 1) % SEQ_SIZE))
				|| ((base + WINDOW_SIZE - 1) % SEQ_SIZE < base && ((ackPkt.acknum >= base && ackPkt.acknum <= SEQ_SIZE - 1) || ackPkt.acknum <= (base + WINDOW_SIZE - 1) % SEQ_SIZE)))
			{
				pUtils->printPacket("发送方正确收到确认\n", ackPkt);
                printf("发送方正确收到确认，发送窗口移动前\n");
                for (int j = base; j != nextseqnum; j = (j + 1) % SEQ_SIZE) {
                    //输出窗口
                    Message msg;
                    memcpy(msg.data, sendPKT[j].payload, sizeof(sendPKT[j].payload));
                    msg.data[21] = '\0';
                    cout << msg.data << endl;                
                }
                cout << endl;
                base = (ackPkt.acknum + 1) % SEQ_SIZE;//更新base
                printf("发送方正确收到确认，发送窗口移动后\n");
                for (int j = base; j != nextseqnum; j = (j + 1) % SEQ_SIZE) {
                    //输出窗口
                    Message msg;
                    memcpy(msg.data, sendPKT[j].payload, sizeof(sendPKT[j].payload));
                    msg.data[21] = '\0';
                    cout << msg.data << endl;
                }
                cout << endl;
                if (base == nextseqnum) {
                    pns->stopTimer(SENDER, 1);//关闭定时器
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
                    pUtils->printPacket("收到三个冗余ack，重发报文\n", sendPKT[base]);
                    pns->sendToNetworkLayer(RECEIVER, sendPKT[base]);//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
                    pns->stopTimer(SENDER, 1);//关闭定时器,重新启动定时器,注意把计时器的seqnum也要填上,这里填1,表示对应sendpkt[0],即对应base
                    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);
                    repeatNum = 0;
                }
            }
		}
	}
}

void TCPSender::timeoutHandler(int seqNum) //Timeout handler，将被NetworkService调用
{
    pns->stopTimer(SENDER, 1);//关闭定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//启动定时器
    pns->sendToNetworkLayer(RECEIVER, sendPKT[base]);
    pUtils->printPacket("发送方重新发送报文", sendPKT[base]);

}