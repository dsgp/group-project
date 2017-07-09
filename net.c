#include "common.h"
#include <string.h>

void net_init(struct port *port)
{
	if (!port->net.nt)
		port->net.nt = -1;
}

void net_rx(struct port *port, int c)
{
	if (c == NCHAR_EOP) {
		printf("[%s] recv: %s\n", port->name, port->net.rbuf + 1);
		if (*port->name == 'r')
			for (int i = 0; routes[i].addr; i++)
				if (routes[i].addr == port->net.rbuf[0]) {
					router[i].net.nt = port->net.nr - 1;
					strcpy(router[i].net.tbuf, port->net.rbuf);
					break;
				}
		port->net.nr = 0;
		return;
	}

	port->net.rbuf[port->net.nr++] = c;
}

int net_tx(struct port *port)
{
	if (port->net.nt < 0)
		return -1;

	if (port->net.i == port->net.nt) {
		printf("[%s] sent: %s\n", port->name, port->net.tbuf + 1);
		port->net.i = 0;
		port->net.nt = -1;
		return NCHAR_EOP;
	}

	return port->net.tbuf[port->net.i++];
}
