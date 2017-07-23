#include "common.h"

void sig_init(struct port *port)
{
	// Start a running parity of 1 so that our first parity bit is 1
	port->sig.tx_parity = 1;
	port->sig.rx_char = -1;
}

void sig_rx_reset(struct port *port)
{
	port->sig.rx_strobe = 0;
	port->sig.rx_data = 0;
	port->sig.rx_parity = 0;
	port->sig.rx_char = -1;
	port->sig.nr = 0;
	port->sig.rx_buffer = 0;
}

void sig_rx(struct port *port, int c)
{
	// Pass disconnect errors directly up to Flow
	if (c == SIG_DISCONNECT_ERROR)
	{
		sig_rx_reset(port);
//TODO		flow_rx(port, SIG_DISCONNECT_ERROR);
		VEPRINT(stderr, "disconnect error!\n");
		return;
	}

	int character;
	int data = c & 0x1;
	int strobe = (c >> 1) & 0x1;
	int buffer = (port->sig.rx_buffer | (data << port->sig.nr++));

	port->sig.rx_parity = (port->sig.rx_parity ^ data);

	// Check for strobe error
	/*if (data == port->sig.rx_data && strobe == port->sig.rx_strobe) {
		sig_rx_reset(port);
		VEPRINT(stderr, "strobe error!\n");
		return;
	}*/

	port->sig.rx_data = data;
	port->sig.rx_strobe = strobe;

	// Check if it's a control character
	 if (port->sig.nr == CHAR_CONTROL_SIZE && (buffer & 0x2)) {
		// Strip parity and DC flag bits
		int payload = (buffer >> 2) & 0x3;

		if (port->sig.got_esc) {
			if (payload == BITS_FCT) {
				character = LCHAR_NULL;
			} else {
				VEPRINT(stderr, "escape error!\n");
			}
			port->sig.got_esc = 0;
		} else if (payload == BITS_FCT) {
			character = LCHAR_FCT;
		} else if (payload == BITS_EOP) {
			character = LCHAR_EOP;
		} else if (payload == BITS_EEP) {
			character = LCHAR_EEP;
		} else { // BITS_ESC
			port->sig.got_esc = 1;
		}
	}
	// Check if it's a data character
	else if (port->sig.nr == CHAR_DATA_SIZE && !(buffer & 0x2)) {
		// Strip parity and DC flag bits
		character = (buffer >> 2) & 0xff;
	}
	// Check for errors
	else if (port->sig.nr >= CHAR_DATA_SIZE) {
		// This only happens when sig.nr > CHAR_DATA_SIZE
		VEPRINT(stderr, "sig.nr corrupted! (%d)\n", port->sig.nr);
	}
	// Keep buffering bits
	else {
		// Check parity before escalating to Flow
		if (port->sig.nr == 2) {
			if (port->sig.rx_char != -1) {
				if (!port->sig.rx_parity) {
					VEPRINT(stderr, "parity error!\n");
				} else if (!port->sig.got_esc) {
					flow_rx(port, port->sig.rx_char);
				}
			}

			// Reset running parity for next coverage area
			port->sig.rx_parity = 0;
		}

		port->sig.rx_buffer = buffer;
		return;
	}

	port->sig.rx_char = character;
	port->sig.nr = 0;
	port->sig.rx_buffer = 0;
}

int sig_tx(struct port *port)
{
	int i = port->sig.nt;

	// Build character
	if (i == 0) {
		int c;

		// When sending a NULL, follow the first ESC with an FCT before fetching a new character
		if (port->sig.add_fct) {
			c = LCHAR_FCT;
			port->sig.add_fct = 0;
		}	
		else
			c = flow_tx(port);

		// Default to Control Char
		int dc_flag = 1;

		// Interpret character type
		if (c == LCHAR_NULL) {
			c = BITS_ESC;
			port->sig.add_fct = 1;
		}
		else if (c == LCHAR_FCT)
			c = BITS_FCT;
		else if (c == LCHAR_EOP)
			c = BITS_EOP;
		else if (c == LCHAR_EEP)
			c = BITS_EEP;
		else if (c == LCHAR_ESC)
			c = BITS_ESC;
		// Other values must be data characters
		else
			dc_flag = 0;

		port->sig.tx_size = dc_flag ? CHAR_CONTROL_SIZE : CHAR_DATA_SIZE;

		// Calculate parity
		int parity = (!port->sig.tx_parity != !dc_flag);

		// Shift parity and data control flag in front of data
		port->sig.tx_char = (parity) | (dc_flag << 1) | ((c & 0xff) << 2);
		port->sig.tx_parity = 1;
	}

	// Get next bit
	int data = (port->sig.tx_char >> i) & 1;

	// Keep a running parity check
	if (i >= 2)
		port->sig.tx_parity = (!port->sig.tx_parity != !data);

	// Toggle strobe when data bit doesn't change
	port->sig.tx_strobe = (!port->sig.tx_strobe != !(data == port->sig.tx_data));
	port->sig.tx_data = data;

	// Prepare next bit
	i = (i + 1) % port->sig.tx_size;
	port->sig.nt = i;

	// Return first bit as data, second as strobe
	return data | (port->sig.tx_strobe << 1);
}
