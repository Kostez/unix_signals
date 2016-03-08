#ifndef common
#define common

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

typedef enum
{
	m_std = 0,
	m_posix,
	m_kill,
	m_child,
	m_pipe
} modes;

typedef struct
{
	int usignal;
	int amount;
	int pid;
	modes mode;
}params;

struct list
{
	siginfo_t siginfo;
	struct list *next;
};

void sign_handler(int signo, siginfo_t *siginf, void *ptr);
void mode_init(params *cmd);

#endif
