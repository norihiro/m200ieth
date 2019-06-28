#include "m200ieth-config.h"
#include "midi_on_tcp.h"
#include <sys/socket.h>

void midi_on_tcp::add_midi(const uint8_t *b, int n)
{
	if(n_buf_send<8) {
		n_buf_send = 8;
	}

	for(; n>0; b++, n--) {
		if((n_buf_send%4)==0)
			n_buf_send++;
		buf_send[n_buf_send++] = *b;
	}
}

int midi_on_tcp::send(uint8_t flg)
{
	int n4 = (n_buf_send + 3) & ~3;
	buf_send[0] = 0x02;
	buf_send[1] = 0x00;
	buf_send[2] = (uint8_t)((n4-4)>>8);
	buf_send[3] = (uint8_t)(n4-4);
	buf_send[4] = 0;
	buf_send[5] = 0;
	buf_send[6] = 0;
	buf_send[7] = 0;
	for(int i=8; i<n4-4; i+=4)
		buf_send[i] = flg | 0x04;
	buf_send[n4-4] = (flg | 0x04) + (n_buf_send-(n4-3));
	for(int i=n_buf_send; i<n4; i++) buf_send[i] = 0;

	int ret = ::send(s, buf_send, n4, 0);
	n_buf_send = 0;
	return ret>0 ? 0 : 21;
}
