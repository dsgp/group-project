#include "common.h"

#define NUM_PORTS 8

int verbose;

struct port nodes[NUM_PORTS];
struct port router[NUM_PORTS];

const struct route routes[] = {
{ 0x20, 0, "\x21msg 0x20->0x21" },
{ 0x21, 1, "\x22msg 0x21->0x22" },
{ 0x22, 2, "\x23msg 0x22->0x23" },
{ 0x23, 3, "\x20msg 0x23->0x20" },
{ 0 },
};

static void port_init(const struct route *route)
{
	struct port *port = nodes + route->port;
	struct port *endp = router + route->port;

	snprintf(port->name, sizeof port->name, "node%x.0", route->addr);
	snprintf(endp->name, sizeof endp->name, "router0.%x", route->port);

	snprintf(port->port_addr, sizeof port->port_addr, "%x.0", route->addr);
	snprintf(endp->port_addr, sizeof endp->port_addr, "0.%x", route->port);

	port->addr = route->addr;
	endp->addr = route->addr;
	port->endp = endp;
	endp->endp = port;

	phys_reset(port, 0);
	net_init(port, route->msg);
	net_init(endp, 0);
}

static void dump(const struct port *port)
{
	if (!port->addr)
		return;

	if (verbose) {
		printf("[%s]\n"
			"- NUM CREDIT ERRORS: %u\n"
			"- NUM PARITY ERRORS: %u\n"
			"- NUM ESCAPE ERRORS: %u\n"
			"- NUM DISC   ERRORS: %u\n"
			"- NUM NET DATA: %u\n"
			"- NUM NET EOP: %u\n"
			"- NUM NET EEP: %u\n"
			"- NUM DDL DATA: %u\n"
			"- NUM DDL CONTROL: %u\n"
			"- NUM DDL EOP: %u\n"
			"- NUM DDL EEP: %u\n"
			"- NUM DDL FCT: %u\n"
			"- NUM DDL ESC: %u\n",
			port->name,
			port->info.num_credit_errors,
			port->info.num_parity_errors,
			port->info.num_escape_errors,
			port->info.num_disc_errors,
			port->info.num_net_data,
			port->info.num_net_eop,
			port->info.num_net_eep,
			port->info.num_dll_data,
			port->info.num_dll_ctrl,
			port->info.num_dll_eop,
			port->info.num_dll_eep,
			port->info.num_dll_fct,
			port->info.num_dll_esc
		);
	} else {
		printf("%s %f %u %u %u %u %u %u %u %u %u %u %u %u %u;\n",
			port->port_addr,
			data_ber,
			port->info.num_credit_errors,
			port->info.num_parity_errors,
			port->info.num_escape_errors,
			port->info.num_disc_errors,
			port->info.num_net_data,
			port->info.num_net_eop,
			port->info.num_net_eep,
			port->info.num_dll_data,
			port->info.num_dll_ctrl,
			port->info.num_dll_eop,
			port->info.num_dll_eep,
			port->info.num_dll_fct,
			port->info.num_dll_esc
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
	verbose = argc > 4 && *argv[4] != '.';

	srand(seed);
	if (verbose) {
		printf("usage: %s [seed] [sim_cycles] [data-ber] [verbose]\n", *argv);
		printf("seed: %zu\n", seed);
		printf("sim_cycles: %016llu\n", sim_cycles);
		printf("data_ber: %f\n", data_ber);
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
