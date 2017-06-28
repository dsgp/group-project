#include "common.h"

void flow_init(struct port *port)
{
	/* TODO */
}

void flow_rx(struct port *port, int c)
{
	/* TODO */
	net_rx(port, c);
}

int flow_tx(struct port *port)
{
	/* TODO */
	return net_tx(port);
}
