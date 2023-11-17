#include "GBNReceiver.h"
#include "Global.h"
#include "stdafx.h"
GBNReceiver::GBNReceiver() :expectSequenceNumberRcvd(0) //初始化
{
    ackPkt.seqnum = -1;
    ackPkt.acknum = -1; // 初始化acknum为-1
    ackPkt.checksum = 0;//校验和为0
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        ackPkt.payload[i] = '.';//初始化ackPkt的payload为'.'
    ackPkt.checksum = pUtils->calculateCheckSum(ackPkt); //计算校验和
}

GBNReceiver::~GBNReceiver()
{
}

//接收报文
void GBNReceiver::receive(const Packet& packet)
{
    int checkSum = pUtils->calculateCheckSum(packet); //计算校验和
    if (checkSum == packet.checksum && packet.seqnum == expectSequenceNumberRcvd) //校验和正确且收到的报文序号为期待收到的报文序号
    {
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload)); //将packet的payload复制到msg的data中
        pns->delivertoAppLayer(RECEIVER, msg); //向上递交给应用层
        ackPkt.acknum = expectSequenceNumberRcvd; //设置ackPkt的acknum为期待收到的报文序号
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt); //计算校验和
        pUtils->printPacket("接收方发送确认报文", ackPkt); //打印ackPkt
        pns->sendToNetworkLayer(SENDER, ackPkt); //向下发送ackPkt
        expectSequenceNumberRcvd = (expectSequenceNumberRcvd + 1) % SEQ_SIZE; //期待收到的报文序号加1
    }
    else if (checkSum != packet.checksum)//校验和错误
    {
        pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet); //打印packet
        pUtils->printPacket("接收方重新发送上次的确认报文", ackPkt); //打印ackPkt
        pns->sendToNetworkLayer(SENDER, ackPkt); //向下发送ackPkt
    }
    else //校验和正确但收到的报文序号不是期待收到的报文序号
    {
        pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet); //打印packet
        pUtils->printPacket("接收方重新发送上次的确认报文", ackPkt); //打印ackPkt
        pns->sendToNetworkLayer(SENDER, ackPkt); //向下发送ackPkt
    }
}