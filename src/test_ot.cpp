#include "emp-ot.h"
#include <emp-tool/emp-tool.h>
#include <iostream>
using namespace std;

template<typename T>
double test_ot(NetIO * io, int party, int length, T* ot = nullptr, int TIME = 10) {
	block *b0 = new block[length], *b1 = new block[length], *r = new block[length];
	PRG prg(fix_key);
	prg.random_block(b0, length);
	prg.random_block(b1, length);
	bool *b = new bool[length];
	for(int i = 0; i < length; ++i) {
		b[i] = (rand()%2)==1;
	}

	long long t1 = 0, t = 0;
	io->sync();
	io->set_nodelay();
	for(int i = 0; i < TIME; ++i) {
		t1 = timeStamp();
		if(ot == nullptr) 
			ot = new T(io);
		if (party == ALICE) {
			ot->send(b0, b1, length);
		} else {
			ot->recv(r, b, length);
		}
		t += timeStamp()-t1;
	}
	if(party == BOB) for(int i = 0; i < length; ++i) {
		if (b[i]) assert(block_cmp(&r[i], &b1[i], 1));
		else assert(block_cmp(&r[i], &b0[i], 1));
	}
	delete ot;
	delete[] b0;
	delete[] b1;
	delete[] r;
	delete[] b;
	return (double)t/TIME;
}
int main(int argc, char** argv) {
	int port, party;
	parse_party_and_port(argv, &party, &port);

	NetIO * io = new NetIO(party==ALICE ? nullptr:SERVER_IP, port);
//	cout <<"NPOT\t"<<test_ot<OTNP>(io, party, 1024)<<endl;
	cout <<"COOT\t"<<test_ot<OTCO>(io, party, 1024)<<endl;
	cout <<"8M Semi Honest OT Extension\t"<<test_ot<OTExtension>(io, party, 1024*1024*8)<<endl;
	OTExtension * ot = new OTExtension(io, 40);
	cout <<"8M Malicious OT Extension\t"<<test_ot<OTExtension>(io, party, 1024*1024*8, ot)<<endl;
	ot = new OTExtension(io, 40);
	cout <<"10M Malicious OT Extension\t"<<test_ot<OTExtension>(io, party, 1024*1024*10, ot)<<endl;
	delete io;
}
