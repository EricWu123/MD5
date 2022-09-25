#include <getopt.h>

#include "md5.h"

static const char *shortopts = "h";

struct option longopts[] = {
	{"help", no_argument, NULL, 'h'},
	{0, 0, 0, 0},
};

static void usage()
{
	printf("Usage: mymd5sum [FILE]...\n");
	printf("Print MD5 (128-bit) checksums.\n");
}

static void parse_option(int argc, char **argv)
{
	int c;
	while ((c = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1)
	{
		switch (c)
		{
		case 'h':
			usage();
			exit(0);

		default:
			exit(1);
		}
	}
	return;
}

int main(int argc, char *argv[])
{
	char output[33] = {0};
	int ret = 0;
	if (argc <= 1)
	{
		ret = md5_file("", output, sizeof(output));
		if (ret == 0)
		{
			printf("%s  -\n", output);
		}
		else
		{
			printf("calc md5 failed. file:stdin\n");
		}
		return 0;
	}

	parse_option(argc, argv);

	for (int i = 1; i < argc; i++)
	{
		memset(output, 0, sizeof(output));
		ret = md5_file(argv[i], output, sizeof(output));
		if (ret == 0)
		{
			printf("%s  %s\n", output, argv[i]);
		}
		else
		{
			printf("calc md5 failed. file:%s\n", argv[i]);
		}
	}
	return 0;
}