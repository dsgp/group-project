#include "common.h"

void phys_tx(struct port *port)
{
	int b = sig_tx(port);
	if (b < 0)
		return;
	port->phys.tbuf[(*port->phys.nt)++] = b;
}

void phys_rx(struct port *port)
{
	if (!*port->phys.nr)
		return;

	int b = port->phys.rbuf[--(*port->phys.nr)];
	sig_rx(port, b);
}
