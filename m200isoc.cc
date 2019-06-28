
#include "m200ieth-config.h"
#include "m200isoc.h"
#include "options.h"
#include <cstdio>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

static
int send_req(const char *host, int port)
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in addr = {0};

	// 0000   52 44 44 50 76 31 c0 c4                           RDDPv1..
	char peer0_0[] = { 0x52, 0x44, 0x44, 0x50, 0x76, 0x31, (char)(port>>8), (char)port };

	addr.sin_family = AF_INET;
	addr.sin_port = htons(9314);
	if(host) addr.sin_addr.s_addr = inet_addr(host);
	sendto(s, peer0_0, sizeof(peer0_0), 0, (sockaddr*)&addr, sizeof(addr));

	close(s);
	return 0;
}

int create_tcp_listen(m200isoc &m)
{
	m.s1 = socket(AF_INET, SOCK_STREAM, 0);
	if(m.s1 < 0) {
		perror("socket");
		return 27;
	}

	int opt = 1;
	setsockopt(m.s1, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

#if 0
	// shouldn't use bind
	// https://stackoverflow.com/questions/741061/listen-without-calling-bind
	for(int p=49348; p<49348+200; p++) {
		struct sockaddr_in me = {0};
		me.sin_family = AF_INET;
		me.sin_port = p;

		if(bind(m.s1, (const sockaddr*)&me, sizeof(me))==0) {
			m.port_listening = p;
			listen(m.s1, 1);
			return 0;
		}
		if(errno!=EADDRINUSE) {
			perror("bind");
		}
	}

	return 44;
#else
	listen(m.s1, 1);

	struct sockaddr_in me = {0};
	socklen_t len = sizeof(me);
	getsockname(m.s1, (sockaddr*)&me, &len);
	m.port_listening = ntohs(me.sin_port);
	return 0;
#endif
}

int m200isoc::connect()
{
	// s1: listening TCP
	// s2: acctual connection

	if(int ret = create_tcp_listen(*this)) {
		return ret;
	}

	send_req(opt.host, port_listening);

	// M-200i will send data and soon it will close.
	// 00000000  52 44 44 50 76 31 83 d8  00 05 5f 68 03 09 00 00   RDDPv1.. .._h....
	// 00000010  00 00 00 00 52 6f 6c 61  6e 64 00 00 00 00 00 00   ....Rola nd......
	// 00000020  00 00 00 00 34 31 32 39  35 00 00 00 31 2c 30 35   ....4129 5...1,05
	// 00000030  31 00 00 00 4d 2d 32 30  30 69 00 00 00 00 00 00   1...M-20 0i......
	// 00000040  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00   ........ ........
	// 00000050  00 00 00 00                                        ....
	// in this example, then iPad will connect to 192.168.124.21:33752
	// 33752 = 83D8h
	struct sockaddr_in server;
	socklen_t len_server = sizeof(server);
	int sx = accept(s1, (sockaddr*)&server, &len_server);
	if(sx<0) {
		perror("accept");
		return 85;
	}
	uint8_t data[84] = {0};
	int ret = recv(sx, data, sizeof(data), 0);
	close(sx);
	close(s1); s1=0;
	int p2 = data[6]<<8 | data[7];

	server.sin_family = AF_INET;
	server.sin_port = htons(p2);
	s2 = socket(AF_INET, SOCK_STREAM, 0);
	::connect(s2, (const sockaddr*)&server, sizeof(server));
	// TODO: send 0000-0000
	// and continue sending for every 1s even though there are packets

	return 0;
}
