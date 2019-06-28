#ifndef MIDI_ON_TCP
#define MIDI_ON_TCP

#include <cstdint>

struct midi_on_tcp
{
	int s;
	int dev;

	uint8_t buf_send[200];
	int n_buf_send;

	void add_midi(const uint8_t*, int);
	int send(uint8_t);
};

#endif // MIDI_ON_TCP
