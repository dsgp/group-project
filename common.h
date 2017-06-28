#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#define EOP 0x100

struct port {
	char name[16];
	int addr;

	struct {
		int nt, nr, i;
		char tbuf[64], rbuf[64];
	} net;
	struct {
		int TODO;
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
