#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "common.h"

void usage();

int main(int argc, char **argv)
{
	const char *rmodes[] = {"std","posix","kill","child","pipe"};
	int modes_count = m_pipe - m_std + 1;
	params cmd;
	cmd.usignal = 1000000;
	cmd.amount = 1000000;
	cmd.pid = 1000000;
	cmd.mode = m_std;
	
	const char* short_options = "m:ha:s:p:";

	const struct option long_options[] =
	{
		{"help",no_argument,NULL,'h'},
		{"mode",optional_argument,NULL,'m'},
		{"amount",optional_argument,NULL,'a'},
		{"signal",optional_argument,NULL,'s'},
		{"pid",optional_argument,NULL,'p'},
		{NULL,0,NULL,0}
	};

	int rez;
	int option_index;

	while ((rez=getopt_long(argc,argv,short_options,
		long_options,&option_index))!=-1){

		switch(rez){
			case 'h': 
			{
				usage();
				break;
			};
			case 's':
			{
				cmd.usignal = atoi(optarg);
				break;
			};
	
			case 'a':
			{
				cmd.amount = atoi(optarg);
				break;
			};
			case 'm':
			{
				int i=0;
				for(;i<modes_count;i++)
				{
					if(strcmp(optarg,rmodes[i]) == 0)
					{
						cmd.mode = i;
						i=modes_count+1;
					}
				}
				if(i==modes_count)
				{
					printf("[E]: Unknown mode!\n");
					usage();
				}
				break;
			};
			case 'p':
			{
				cmd.pid = atoi(optarg);
				break;
			};
			
			default:
			{
				printf("found unknown option\n");
				usage();
				break;
			};
		};
	};
	
	switch(cmd.mode)
	{
		case m_kill:
		{
			if(cmd.usignal<0 || cmd.usignal>31)
			{
				printf("[E]: Invalid signal value\n");
				usage();
			}
			if(cmd.pid == 1000000)
			{
				printf("[E]: pid is required for mode kill\n");
				usage();
			}
			break;
		};
		case m_posix:
		{
			if(cmd.amount == 1000000)
			{
				printf("[E]: amount is required for mode posix\n");
				usage();				
			}
			break;
		};
	}
	
	mode_init(&cmd);
	
	while(1){}
	return 0;
}


void usage()
{
	printf(" -h or --help show this message and exit.\n");
	printf(" -m or --mode set mode(child,std,posix,kill,pipe).\n");
	printf(" -a or --amount used with -m posix. -amount N where N is number of signals.\n");
	printf(" -s or --signal used with -m kill. singlan to send.\n");
	printf(" -p or --pid used with -m kill. process pid which should receive signal.\n");
	exit(EXIT_SUCCESS);
};
