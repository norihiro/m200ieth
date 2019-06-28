
#include "m200ieth-config.h"
#include "options.h"
#include "m200isoc.h"
#include <cstdio>
#include <cstring>

static
int init(int argc, char **argv, options_s &opt)
{
	memset(&opt, 0, sizeof(opt));

	for(int i=1; i<argc; i++) {
		char *ai = argv[i];
		if(*ai=='-') while (int c=*++ai) switch(c) {
			case 'h':
				opt.host = argv[++i];
				break;
			default:
				fprintf(stderr, "Error: unknown option -%c\n", c);
				return 18;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	options_s opt;
	if(int ret = init(argc, argv, opt)) {
		fprintf(stderr, "Error during parsing arguments\n");
		return ret;
	}

	m200isoc soc(opt);
	if(int ret = soc.connect()) {
		fprintf(stderr, "Error to open socket\n");
		return ret;
	}

	return 0;
}
