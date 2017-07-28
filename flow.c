#include "common.h"

void flow_init(struct port *port)
{
	port->flow.tx_allowance = 0;
	port->flow.tx_outstanding = 0;
}

void flow_rx(struct port *port, int c)
{
	// receiving an FCT allows transfer of 8 more NCHARs
	if (c == LCHAR_FCT) {
		port->flow.tx_allowance += 8;

		// excessive FCTs triggers a credit error
		if (port->flow.tx_allowance > MAX_CREDIT_COUNT) {
			VEPRINT(stderr, "credit error!\n");
			phys_reset(port);
		}

		return;

	// ignore NULL chars
	} else if (c == LCHAR_NULL) {
		return;

	// LCHAR EOP becomes NCHAR EOP
	} else if (c == LCHAR_EOP) {
		c = NCHAR_EOP;

	// LCHAR EEP becomes NCHAR EEP
	} else if (c == LCHAR_EEP) {
		c = NCHAR_EEP;
	} else if (c < 0x100) {
		// normative character - do nothing
	} else {
		VEPRINT(stderr, "sim error! (%d)\n", c);
	}

	port->flow.tx_outstanding--;
	net_rx(port, c);
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
