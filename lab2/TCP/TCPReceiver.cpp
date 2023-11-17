#include "TCPReceiver.h"
#include "Global.h"
#include "stdafx.h"
TCPReceiver::TCPReceiver() :expectSequenceNumberRcvd(0) //��ʼ��
{
    ackPkt.seqnum = -1;
    ackPkt.acknum = -1; // ��ʼ��acknumΪ-1
    ackPkt.checksum = 0;//У���Ϊ0
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
        ackPkt.payload[i] = '.';//��ʼ��ackPkt��payloadΪ'.'
    ackPkt.checksum = pUtils->calculateCheckSum(ackPkt); //����У���
}

TCPReceiver::~TCPReceiver()
{
}

//���ձ���
void TCPReceiver::receive(const Packet& packet)
{
    int checkSum = pUtils->calculateCheckSum(packet); //����У���
    if (checkSum == packet.checksum && packet.seqnum == expectSequenceNumberRcvd) //У�����ȷ���յ��ı������Ϊ�ڴ��յ��ı������
    {
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload)); //��packet��payload���Ƶ�msg��data��
        pns->delivertoAppLayer(RECEIVER, msg); //���ϵݽ���Ӧ�ò�
        ackPkt.acknum = expectSequenceNumberRcvd; //����ackPkt��acknumΪ�ڴ��յ��ı������
        ackPkt.checksum = pUtils->calculateCheckSum(ackPkt); //����У���
        pUtils->printPacket("���շ�����ȷ�ϱ���", ackPkt); //��ӡackPkt
        pns->sendToNetworkLayer(SENDER, ackPkt); //���·���ackPkt
        expectSequenceNumberRcvd = (expectSequenceNumberRcvd + 1) % SEQ_SIZE; //�ڴ��յ��ı�����ż�1
    }
    else if (checkSum != packet.checksum)//У��ʹ���
    {
        pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet); //��ӡpacket
        pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", ackPkt); //��ӡackPkt
        pns->sendToNetworkLayer(SENDER, ackPkt); //���·���ackPkt
    }
    else //У�����ȷ���յ��ı�����Ų����ڴ��յ��ı������
    {
        pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet); //��ӡpacket
        pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", ackPkt); //��ӡackPkt
        pns->sendToNetworkLayer(SENDER, ackPkt); //���·���ackPkt
    }
}