#include "common.h"

void sig_init(struct port *port)
{
	/* TODO */
}

int sig_tx(struct port *port)
{
	/* TODO */
	return flow_tx(port);
}

void sig_rx(struct port *port, int c)
{
	/* TODO */
	flow_rx(port, c);
}
