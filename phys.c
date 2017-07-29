#include "common.h"

double data_ber;

void phys_tx(struct port *port)
{
	port->info.cycle++;

	int b = sig_tx(port);
	if (b < 0)
		return;

	/* randomly inject fault into data signal based on data BER */
	if (rand() < data_ber * RAND_MAX) {
		VEPRINT(stderr, "[%s] %016llu: UPSET\n", port->name, port->info.cycle);
		b ^= 0x1;
	}

	port->endp->phys.signal = b;
	port->endp->phys.reset = 0;
}

void phys_rx(struct port *port)
{
	if (!port->phys.reset)
		sig_rx(port, port->phys.signal);
}

static void reset(struct port *port, int err)
{
	VEPRINT(stderr, "[%s] %016llu: RESET\n", port->name, port->info.cycle);

	memset(&port->phys, 0, sizeof port->phys);
	memset(&port->sig, 0, sizeof port->sig);
	memset(&port->flow, 0, sizeof port->flow);

	sig_init(port);
	flow_init(port);

	port->phys.reset = 1;

	if (err)
		net_rx(port, NCHAR_EEP);
}
void phys_reset(struct port *port, int err)
{
	reset(port, err);
	reset(port->endp, err);

	if (err)
		port->endp->info.num_disc_errors++;
}
