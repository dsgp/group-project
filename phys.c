#include "common.h"
#include <stdlib.h>

void phys_tx(struct port *port)
{
	static unsigned long long cycle = 0;
	cycle++;

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

	if (error)
		VEPRINT(stderr, "%016llu: tx: %u -> %u\n", cycle, b_prev, b);
#endif

	port->endp->phys.signal = b;
	port->endp->phys.reset = 0;
}

void phys_rx(struct port *port)
{
	if (!port->phys.reset)
		sig_rx(port, port->phys.signal);
}

void phys_reset(struct port *port)
{
	sig_init(port);
	flow_init(port);
	sig_init(port->endp);
	flow_init(port->endp);
	port->phys.reset = port->endp->phys.reset = 1;
}
