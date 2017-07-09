#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#define LCHAR_NULL 0x100
#define LCHAR_FCT  0x101
#define LCHAR_EOP  0x102
#define LCHAR_EEP  0x103

#define NCHAR_EOP  0x100
#define NCHAR_EEP  0x101

#define MAX_CREDIT_COUNT 56
#define MAX_FIFO_SIZE 64

struct port {
	char name[16];
	int addr;

	struct {
		int nt, nr, i;
		char tbuf[MAX_FIFO_SIZE], rbuf[MAX_FIFO_SIZE];
	} net;
	struct {
		int tx_allowance;
		int tx_outstanding;
	} flow;
	struct {
		int TODO;
	} sig;
	struct {
		int *nt, *nr;
		int *tbuf, *rbuf;
	} phys;
};

struct route {
	int addr, port;
	const char *msg;
};

extern struct port ports[], router[];
extern const struct route routes[];

void phys_tx(struct port *);
void phys_rx(struct port *);

int sig_tx(struct port *);
void sig_rx(struct port *, int);
void sig_init(struct port *);

int flow_tx(struct port *);
void flow_rx(struct port *, int);
void flow_init(struct port *);

int net_tx(struct port *);
void net_rx(struct port *, int);
void net_init(struct port *);

#endif
