#include "GBNSender.h"//GBNSender类的声明
#include "Global.h"
#include "stdafx.h"
GBNSender::GBNSender() :base(0), nextseqnum(0)
{
}
GBNSender::~GBNSender()
{
}
bool GBNSender::getWaitingState()//返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true
{
    if (nextseqnum == (base + WINDOW_SIZE) % SEQ_SIZE)
        return true;
    else
        return false;
}

bool GBNSender::send(const Message& message) //发送应用层下来的Message
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

void GBNSender::receive(const Packet& ackPkt) //接受确认Ack,将被NetworkService调用,NetworkService通过调用sendToNetworkLayer()将Ack发送给发送方
{

    int checkSum = pUtils->calculateCheckSum(ackPkt);//计算校验和
    if (checkSum == ackPkt.checksum)//校验和正确
    {
        cout << "窗口移动前："<<endl;
        for (int i = base; i != nextseqnum; i = (i + 1) % SEQ_SIZE) {
            //输出窗口
            Message msg;
            memcpy(msg.data, sendPKT[i].payload, sizeof(sendPKT[i].payload));
            msg.data[21] = '\0';
            cout << msg.data << endl;
        }
        base  = (ackPkt.acknum + 1) % SEQ_SIZE;//base更新为acknum+1
        cout << "窗口移动后："<<endl;
        for (int i = base; i != nextseqnum; i = (i + 1) % SEQ_SIZE) {
            //输出窗口
            Message msg;
            memcpy(msg.data, sendPKT[i].payload, sizeof(sendPKT[i].payload));
            msg.data[21] = '\0';
            cout << msg.data << endl;
        }
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        if (base % SEQ_SIZE == nextseqnum % SEQ_SIZE)
        {
            //pns->stopTimer(SENDER,base % SEQ_SIZE);//关闭定时器
            pns->stopTimer(SENDER, 1);//关闭定时器
        }
        else//acknum在base和nextseqnum之外,说明acknum已经被确认过了
        {
            pns->stopTimer(SENDER, 1);//关闭定时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//启动定时器
        }
    }
    else
    {
        pUtils->printPacket("发送方没有正确收到确认", ackPkt);
    }

}

void GBNSender::timeoutHandler(int seqNum) //Timeout handler，将被NetworkService调用
{
    pns->stopTimer(SENDER, 1);//关闭定时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//启动定时器
    for (int i = base; i != nextseqnum; i = (i + 1) % SEQ_SIZE)
    {
        pns->sendToNetworkLayer(RECEIVER, sendPKT[i]);//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
        pUtils->printPacket("发送方重新发送报文", sendPKT[i]);//调用模拟工具函数显示发出的数据
    }
}