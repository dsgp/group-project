#include "common.h"

void flow_init(struct port *port)
{
	(void) port;
}

int flow_rx(struct port *port, int c)
{

	// receiving an FCT allows transfer of 8 more NCHARs
	if (c == LCHAR_FCT) {
		port->flow.tx_credits += 8;

		// excessive FCTs triggers a credit error
		if (port->flow.tx_credits > MAX_CREDIT_COUNT) {
			VEPRINT(stderr, "[%s] %016llu: credit error!\n", port->name, port->info.cycle);
			port->info.num_credit_errors++;
			phys_reset(port, 1);
			return -1;
		}

		port->info.num_rx_ctrl_char++;
		return 0;
	}

	// ignore NULL chars
	if (c == LCHAR_NULL) {
		port->info.num_rx_ctrl_char++;
		return 0;

	// LCHAR EOP becomes NCHAR EOP
	} else if (c == LCHAR_EOP) {
		port->info.num_rx_ctrl_char++;
		c = NCHAR_EOP;

	// LCHAR EEP becomes NCHAR EEP
	} else if (c == LCHAR_EEP) {
		port->info.num_rx_ctrl_char++;
		c = NCHAR_EEP;
	// normative character
	} else {
		assert(c < 0x100);
	}

	port->info.num_rx_data_char++;

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
		port->info.num_tx_ctrl_char++;
		return LCHAR_FCT;

	// priority 3: NCHARs: send NCHAR if available (NCHAR EOP becomes LCHAR EOP)
	} else if (tx_rem > 0 && port->flow.tx_credits > 0) {
		port->flow.tx_credits--;
		int c = net_tx(port);

		if (c == NCHAR_EOP) {
			port->info.num_tx_ctrl_char++;
			c = LCHAR_EOP;
		} else {
			port->info.num_tx_data_char++;
		}

		return c;
	}

	// priority 4: NULL: send NULL for idled link
	port->info.num_tx_ctrl_char++;
	return LCHAR_NULL;
}
