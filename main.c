#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PORTS 8

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

	/* links */
	endp->phys.tbuf = port->phys.rbuf = calloc(64, sizeof(int));
	endp->phys.rbuf = port->phys.tbuf = calloc(64, sizeof(int));
	endp->phys.nt = port->phys.nr = calloc(1, sizeof(int));
	endp->phys.nr = port->phys.nt = calloc(1, sizeof(int));

	if (route->msg) {
		port->net.nt = strlen(route->msg) + 1;
		strcpy(port->net.tbuf, route->msg);
	}

	phys_init(port);
	sig_init(port);
	flow_init(port);
	net_init(port);
	phys_init(endp);
	sig_init(endp);
	flow_init(endp);
	net_init(endp);
}

int main(int argc, char **argv)
{
	size_t seed;

	printf("usage: %s [seed]\n", *argv);

	// seed
	seed = argc > 1 ? atoi(argv[1]) : time(0);
	srand(seed);
	printf("seed: %zu\n", seed);

	for (unsigned i = 0; routes[i].addr; i++)
		port_init(routes + i);

	for (;;)
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
}
