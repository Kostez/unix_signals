#include "common.h"

pid_t child;
pid_t parent;
struct list *first;
struct list *last;

void sign_handler(int signo, siginfo_t *siginf, void *ptr)
{
	switch(signo)
	{
		case SIGHUP:
		{
			printf("SIGNO %d PID %d GID %d\n",signo,siginf->si_pid,getpgid(siginf->si_pid));
			break;
		};
		case SIGUSR1:
		{
			printf("SIGNO %d PID %d GID %d\n",signo,siginf->si_pid,getpgid(siginf->si_pid));
			break;
		};
		case SIGUSR2:
		{
			printf("SIGNO %d PID %d GID %d\n",signo,siginf->si_pid,getpgid(siginf->si_pid));
			break;
		};
		
		case SIGCHLD:
		{
			printf("siginfo_t fields:\n");
			printf("si_signo: %d \n", siginf->si_signo);
			printf("si_errno: %d \n", siginf->si_errno);
			printf("si_code: %d \n", siginf->si_code);
			printf("si_pid: %d \n", siginf->si_pid);
			printf("si_uid: %d \n", siginf->si_uid);
			printf("si_status: %d \n", siginf->si_status);
			printf("si_utime: %li \n", siginf->si_utime);
			printf("si_stime: %li \n", siginf->si_stime);
			exit(EXIT_SUCCESS);
			break;
		};
		
		case SIGPIPE:
		{
			printf("Listner is dead..\n");
			exit(EXIT_SUCCESS);
			break;
		};
	}
	return;
}

void sign_handler_posix(int signo, siginfo_t *siginf, void *ptr)
{
	if(signo==SIGCHLD)
	{
		struct list *cur = first;
		printf("[PARENT]\n");
		printf("|  N  | MYPID | CHILD | SIGNO | VALUE |\n");
		int i =1;
		while(cur!=NULL)
		{
			printf("|%5d|%7d|%7d|%7d|%7d|\n",i,parent,child,cur->siginfo.si_signo,cur->siginfo.si_value.sival_int);
			i++;
			cur = cur->next;
		}
		exit(EXIT_SUCCESS);
	}
	else
	{
		if(first==NULL)
		{
			struct list *cur = (struct list*)malloc(sizeof(struct list));
			if(cur==NULL)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			memcpy(&(cur->siginfo),siginf,sizeof(siginfo_t));
			cur->next = NULL;
			first = cur;
			last = cur;
		}
		else
		{
			struct list *cur = (struct list*)malloc(sizeof(struct list));
			if(cur==NULL)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			memcpy(&(cur->siginfo),siginf,sizeof(siginfo_t));
			cur->next = NULL;
			last->next=cur;
			last = cur;
		}
	}
	return;
}

void mode_init(params *cmd)
{
	/*
	 * structure
	 * {
	 * sa_handler - function or special const
	 * sigset_t sa_mask - block signals of set when handler is called
	 * sa_flags - flags
	 * }
	 */
	struct sigaction sa;
	/*
	 * init empty set and fill with all signals
	 */
	sigemptyset(&(sa.sa_mask));
	sigfillset(&(sa.sa_mask));
	
	sa.sa_flags = SA_SIGINFO|SA_NOCLDWAIT;
	sa.sa_sigaction = sign_handler;
	
	switch(cmd->mode)
	{
		case m_std:
		{
			if(sigaction(SIGHUP, &sa, 0)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			if(sigaction(SIGUSR1, &sa, 0)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			if(sigaction(SIGUSR2, &sa, 0)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			break;
		};
		case m_posix:
		{
			sa.sa_sigaction = sign_handler_posix;
			for(int i=SIGRTMIN; i<=SIGRTMAX; i++)
			{
				if(sigaction(i, &sa, 0)==-1)
				{
					perror(NULL);
					exit(EXIT_FAILURE);
				}
			}
			if(sigaction(SIGCHLD, &sa, 0)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			int diap = SIGRTMAX - SIGRTMIN;
			parent = getpid();
			child = fork();			
			switch(child)
			{
				case -1:
				{
					perror(NULL);
					exit(EXIT_FAILURE);
					break;
				};
				case 0: //fork
				{
					child = getpid();
					union sigval qval1;
					int signo=0;
					srand(time(0));
					printf("[CHILD]\n");
					printf("|  N  | MYPID | PARENT | SIGNO | VALUE |\n");
					for(int i=0;i<cmd->amount;i++)
					{
						qval1.sival_int = rand()%100;
						signo = SIGRTMIN+rand()%diap;
						sigqueue(parent,signo,qval1);
						printf("|%5d|%7d|%8d|%7d|%7d|\n",i+1,child,parent,signo,qval1.sival_int);
					}
					sleep(1);
					exit(3);
				};
				default:
				{
					break;
				};
			}
			break;
		};
		case m_kill:
		{
			if(kill(cmd->pid, cmd->usignal)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
			break;
		};
		case m_child:
		{
			if(sigaction(SIGCHLD, &sa, 0)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			pid_t child_pid = fork();
			switch(child_pid)
			{
				case -1:
				{
					perror(NULL);
					exit(EXIT_FAILURE);
					break;
				};
				case 0: //fork
				{
					srand(time(0));
					sleep(rand()%10+1);
					exit(3);
				};
				default:
				{
					break;
				};
			}
			break;
		};
		case m_pipe:
		{			
			if(sigaction(SIGPIPE, &sa, 0)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			int fd[2];
			if(pipe(fd)==-1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			pid_t child_pid = fork();
			switch(child_pid)
			{
				case -1:
				{
					perror(NULL);
					exit(EXIT_FAILURE);
					break;
				};
				case 0: //fork
				{
					if(close(fd[1])==-1) //close output
					{
						perror(NULL);
						exit(EXIT_FAILURE);
					}
					srand(time(0));
					sleep(rand()%10+1);
					exit(3);
					break;
				};
				default:
				{
					if(close(fd[0])==-1)//close input
					{
						perror(NULL);
						exit(EXIT_FAILURE);
					}
					while(1)
					{
						if(write(fd[1],"0",1)==-1)
						{
							perror(NULL);
							exit(EXIT_FAILURE);
						}
						sleep(1);
					}
					break;
				};
			}
			break;
		};		
	}
}
