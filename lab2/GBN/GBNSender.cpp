#include "GBNSender.h"//GBNSender�������
#include "Global.h"
#include "stdafx.h"
GBNSender::GBNSender() :base(0), nextseqnum(0)
{
}
GBNSender::~GBNSender()
{
}
bool GBNSender::getWaitingState()//����RdtSender�Ƿ��ڵȴ�״̬��������ͷ����ȴ�ȷ�ϻ��߷��ʹ�������������true
{
    if (nextseqnum == (base + WINDOW_SIZE) % SEQ_SIZE)
        return true;
    else
        return false;
}

bool GBNSender::send(const Message& message) //����Ӧ�ò�������Message
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

void GBNSender::receive(const Packet& ackPkt) //����ȷ��Ack,����NetworkService����,NetworkServiceͨ������sendToNetworkLayer()��Ack���͸����ͷ�
{

    int checkSum = pUtils->calculateCheckSum(ackPkt);//����У���
    if (checkSum == ackPkt.checksum)//У�����ȷ
    {
        cout << "�����ƶ�ǰ��"<<endl;
        for (int i = base; i != nextseqnum; i = (i + 1) % SEQ_SIZE) {
            //�������
            Message msg;
            memcpy(msg.data, sendPKT[i].payload, sizeof(sendPKT[i].payload));
            msg.data[21] = '\0';
            cout << msg.data << endl;
        }
        base  = (ackPkt.acknum + 1) % SEQ_SIZE;//base����Ϊacknum+1
        cout << "�����ƶ���"<<endl;
        for (int i = base; i != nextseqnum; i = (i + 1) % SEQ_SIZE) {
            //�������
            Message msg;
            memcpy(msg.data, sendPKT[i].payload, sizeof(sendPKT[i].payload));
            msg.data[21] = '\0';
            cout << msg.data << endl;
        }
        pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
        if (base % SEQ_SIZE == nextseqnum % SEQ_SIZE)
        {
            //pns->stopTimer(SENDER,base % SEQ_SIZE);//�رն�ʱ��
            pns->stopTimer(SENDER, 1);//�رն�ʱ��
        }
        else//acknum��base��nextseqnum֮��,˵��acknum�Ѿ���ȷ�Ϲ���
        {
            pns->stopTimer(SENDER, 1);//�رն�ʱ��
            pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//������ʱ��
        }
    }
    else
    {
        pUtils->printPacket("���ͷ�û����ȷ�յ�ȷ��", ackPkt);
    }

}

void GBNSender::timeoutHandler(int seqNum) //Timeout handler������NetworkService����
{
    pns->stopTimer(SENDER, 1);//�رն�ʱ��
    pns->startTimer(SENDER, Configuration::TIME_OUT, 1);//������ʱ��
    for (int i = base; i != nextseqnum; i = (i + 1) % SEQ_SIZE)
    {
        pns->sendToNetworkLayer(RECEIVER, sendPKT[i]);//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
        pUtils->printPacket("���ͷ����·��ͱ���", sendPKT[i]);//����ģ�⹤�ߺ�����ʾ����������
    }
}