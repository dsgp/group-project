#include "common.h"

void phys_tx(struct port *port)
{
	port->info.cycle++;

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
		VEPRINT(stderr, "[%s] %016llu: UPSET: tx: %u -> %u\n", port->name, port->info.cycle, b_prev, b);
	}
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
	VEPRINT(stderr, "[%s] %016llu: RESET\n", port->name, port->info.cycle);

	memset(&port->phys, 0, sizeof port->phys);
	memset(&port->sig, 0, sizeof port->sig);
	memset(&port->flow, 0, sizeof port->flow);

	memset(&port->endp->phys, 0, sizeof port->endp->phys);
	memset(&port->endp->sig, 0, sizeof port->endp->sig);
	memset(&port->endp->flow, 0, sizeof port->endp->flow);
 
	sig_init(port);
	flow_init(port);
	sig_init(port->endp);
	flow_init(port->endp);
	port->phys.reset = port->endp->phys.reset = 1;

	/* flow control sends EEP to packet level */
//XXX causings floating point exception: "i = (i + 1) % port->sig.tx_size;" in sig.c - divide by 0?
//	net_rx(port, NCHAR_EEP);
//	net_rx(port->endp, NCHAR_EEP);
}
