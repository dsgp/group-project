#include "common.h"

void flow_init(struct port *port)
{
	(void) port;
}

int flow_rx(struct port *port, int c)
{
	// receiving an FCT allows transfer of 8 more NCHARs
	if (c == LCHAR_FCT) {
		port->flow.tx_allowance += 8;

		// excessive FCTs triggers a credit error
		if (port->flow.tx_allowance > MAX_CREDIT_COUNT) {
			VEPRINT(stderr, "[%s] %016llu: credit error!\n", port->name, port->cycle);
			phys_reset(port);
			return -1;
		}

		return 0;
	}

	// ignore NULL chars
	if (c == LCHAR_NULL)
		return 0;

	// LCHAR EOP becomes NCHAR EOP
	if (c == LCHAR_EOP) {
		c = NCHAR_EOP;

	// LCHAR EEP becomes NCHAR EEP
	} else if (c == LCHAR_EEP) {
		c = NCHAR_EEP;
	} else if (c < 0x100) {
		// normative character - do nothing
	} else {
		VEPRINT(stderr, "[%s] %016llu: sim error! func=%s line=%d (%d)\n", port->name, port->cycle, __FUNCTION__, __LINE__, c);
	}

	port->flow.tx_outstanding--;
	net_rx(port, c);

	return 0;
}

int flow_tx(struct port *port)
{
	int tx_rem = port->net.nt - port->net.i + 1; // number of entries in TX FIFO valid for transmission
	int rx_rem = MAX_FIFO_SIZE - port->net.nr;   // number of free entries in RX FIFO

	// priority 1: time-codes (unhandled)

	// priority 2: FCTs: send FCT when RX FIFO has at least 8 free entries and outstanding FCTs are 8 transfers less than the MAX_CREDIT_COUNT
	if ((port->flow.tx_outstanding < MAX_CREDIT_COUNT-8) && (rx_rem >= 8)) {
		port->flow.tx_outstanding += 8;
		return LCHAR_FCT;

	// priority 3: NCHARs: send NCHAR if available (NCHAR EOP becomes LCHAR EOP)
	} else if (tx_rem > 0 && port->flow.tx_allowance > 0) {
		port->flow.tx_allowance--;
		int c = net_tx(port);
		return c == NCHAR_EOP ? LCHAR_EOP : c;
	}

	// priority 4: NULL: send NULL for idled link
	return LCHAR_NULL;
}
