#include "common.h"

void net_init(struct port *port)
{
	if (!port->net.nt)
		port->net.nt = -1;
}

void net_rx(struct port *port, int c)
{
	switch (c) {
	case NCHAR_EOP:
		port->info.num_eop++;
		verbose && printf("[%s] recv: %s\n", port->name, port->net.rbuf + 1);
		if (*port->name == 'r')
			for (int i = 0; routes[i].addr; i++)
				if (routes[i].addr == port->net.rbuf[0]) {
					router[i].net.nt = port->net.nr - 1;
					strcpy(router[i].net.tbuf, port->net.rbuf);
					break;
				}
		port->net.nr = 0;
		break;
	case NCHAR_EEP:
		port->info.num_eep++;
		port->net.nr = 0;
		break;
	default:
		port->net.rbuf[port->net.nr++] = c;
	}
}

int net_tx(struct port *port)
{
	if (port->net.nt < 0)
		return -1;

	if (port->net.i == port->net.nt) {
		verbose && printf("[%s] sent: %s\n", port->name, port->net.tbuf + 1);
		port->net.i = 0;
		port->net.nt = -1;
		return NCHAR_EOP;
	}

	return port->net.tbuf[port->net.i++];
}
