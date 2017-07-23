#include "common.h"
#include <stdlib.h>

void phys_init(struct port *port)
{
	(void) port;
}

void phys_tx(struct port *port)
{
	int b = sig_tx(port);
	if (b < 0)
		return;

	/* fault injection */
#ifdef ENABLE_FAULT_INJECTION
	int error = 0;

	int b_prev = b;

	/* randomly inject fault into data signal based on data BER */
	if (rand() < DATA_BER * RAND_MAX) {
		b ^= 0x1;
		error = 1;
	}

	/* randomly inject fault into strobe signal based on strobe BER */
	if (rand() < STROBE_BER * RAND_MAX) {
		b ^= 0x2;
		error = 1;
	}

	if (error) {
		printf("tx: %u -> %u\n", b_prev, b);
	}
#endif

	port->phys.tbuf[(*port->phys.nt)++] = b;
}

void phys_rx(struct port *port)
{
	if (!*port->phys.nr) {
		if (port->phys.rx_timer++ == DISCONNECT_TIMEOUT)
			sig_rx(port, SIG_DISCONNECT_ERROR);
		return;
	}
	port->phys.rx_timer = 0;

	int b = port->phys.rbuf[--(*port->phys.nr)];
	sig_rx(port, b);
}
