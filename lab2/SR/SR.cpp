#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "SRSender.h"
#include "SRReceiver.h"
int main(int argc, char* argv[])
{
	RdtSender* ps = new SRSender();
	RdtReceiver* pr = new SRReceiver();
	pns->setRunMode(1);  //VERBOSģʽ
	//pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("C:\\Users\\20963\\Desktop\\Grade3\\���������\\lab\\lab2\\input.txt");
	pns->setOutputFile("C:\\Users\\20963\\Desktop\\Grade3\\���������\\lab\\lab2\\output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;
}
