#include "common.h"

#define NUM_PORTS 8

int verbose;

struct port nodes[NUM_PORTS];
struct port router[NUM_PORTS];

const struct route routes[] = {
{ 0x20, 0, "\x21hello world" },
{ 0x21, 1, 0 },
{ 0 },
};

static void port_init(const struct route *route)
{
	struct port *port = nodes + route->port;
	struct port *endp = router + route->port;

	snprintf(port->name, sizeof port->name, "node%x.0", route->addr);
	snprintf(endp->name, sizeof endp->name, "router0.%x", route->port);
	port->addr = route->addr;
	endp->addr = route->addr;
	port->endp = endp;
	endp->endp = port;

	if (route->msg) {
		port->net.nt = strlen(route->msg) + 1;
		strcpy(port->net.tbuf, route->msg);
	}

	phys_reset(port, 0);
	net_init(port);
	net_init(endp);
}

static void dump(const struct port *port)
{
	if (!port->addr)
		return;

	if (verbose) {
		printf("[%s]\n"
			"- credit errors: %u\n"
			"- parity errors: %u\n"
			"- escape errors: %u\n"
			"- disc   errors: %u\n"
			"- EOP: %u\n"
			"- EEP: %u\n"
			"- TX DATA: %u\n"
			"- TX CONTROL: %u\n"
			"- RX DATA: %u\n"
			"- RX CONTROL: %u\n",
			port->name,
			port->info.num_credit_errors,
			port->info.num_parity_errors,
			port->info.num_escape_errors,
			port->info.num_disc_errors,
			port->info.num_eop,
			port->info.num_eep,
			port->info.num_tx_data_char,
			port->info.num_tx_ctrl_char,
			port->info.num_rx_data_char,
			port->info.num_rx_ctrl_char
		);
	} else {
		printf("\"[%s]\" %f %u %u %u %u %u %u %u %u %u %u;\n",
			port->name,
			data_ber,
			port->info.num_credit_errors,
			port->info.num_parity_errors,
			port->info.num_escape_errors,
			port->info.num_disc_errors,
			port->info.num_eop,
			port->info.num_eep,
			port->info.num_tx_data_char,
			port->info.num_tx_ctrl_char,
			port->info.num_rx_data_char,
			port->info.num_rx_ctrl_char
		);
	}
}


int main(int argc, char **argv)
{
	size_t seed;
	unsigned long long sim_cycles;


	// seed
	seed = argc > 1 && *argv[1] != '.' ? atoi(argv[1]) : time(0);
	sim_cycles = argc > 2 && *argv[2] != '.' ? atoi(argv[2]) : -1;
	data_ber = argc > 3 && *argv[3] != '.' ? atof(argv[3]) : 0;
	strobe_ber = argc > 4 && *argv[4] != '.' ? atof(argv[4]) : 0;
	verbose = argc > 5 && *argv[5] != '.';

	srand(seed);
	if (verbose) {
		printf("usage: %s [seed] [sim_cycles] [data-ber] [strobe-ber] [verbose]\n", *argv);
		printf("seed: %zu\n", seed);
		printf("sim_cycles: %016llu\n", sim_cycles);
		printf("data_ber: %f\n", data_ber);
		printf("strobe_ber: %f\n", strobe_ber);
	}

	for (unsigned i = 0; routes[i].addr; i++)
		port_init(routes + i);

	for (;;) {
		for (unsigned i = 0; i < NUM_PORTS; i++) {
			if (nodes[i].addr) {
				phys_tx(nodes + i);
				phys_rx(nodes + i);
			}
			if (router[i].addr) {
				phys_tx(router + i);
				phys_rx(router + i);
			}
		}

		// end simulation when a port exceeds the number of simulation cycles
		for (unsigned i = 0; i < NUM_PORTS; i++) {
			if (nodes[i].addr && nodes[i].info.cycle >= sim_cycles) {
				goto end;
			}
			if (router[i].addr && router[i].info.cycle >= sim_cycles) {
				goto end;
			}
		}
	}

end:
	for (unsigned i = 0; i < NUM_PORTS; i++) {
		dump(nodes + i);
		dump(router + i);
	}
	return 0;
}
