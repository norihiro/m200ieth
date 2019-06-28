#ifndef M200ISOC
#define M200ISOC

struct m200isoc
{
	int s1, s2;
	int port_listening;
	struct options_s &opt;

	m200isoc(struct options_s &o) : opt(o) { s1=0; s2=0; port_listening=0; }
	int connect();
};

#endif // M200ISOC
