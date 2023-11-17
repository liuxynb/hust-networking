#include "RdtReceiver.h"
#include "Global.h"
#include <string.h>
#define WINDOW_SIZE 4
#define SEQ_SIZE 8

class SRReceiver :public RdtReceiver {
	bool recv[SEQ_SIZE];
	Packet recvPkt[SEQ_SIZE];
	int recv_base;
	Packet ackPkt;

public:
	void receive(const Packet& packet);	
	SRReceiver();
	~SRReceiver();

};