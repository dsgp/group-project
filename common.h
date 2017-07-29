#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* user options */
//#define VERBOSE_ERRORS         // error reports are verbose
#define ENABLE_FAULT_INJECTION // fault injection is performed
#define DATA_BER   0.00001f    // - BER = upsets rate per transfer
#define STROBE_BER 0.00001f    // - BER = upsets rate per transfer

/* definitions */
#define LCHAR_NULL 0x100
#define LCHAR_FCT  0x101
#define LCHAR_EOP  0x102
#define LCHAR_EEP  0x103
#define LCHAR_ESC  0x104

#define NCHAR_EOP  0x100
#define NCHAR_EEP  0x101

#define BITS_FCT   0x0
#define BITS_EOP   0x1
#define BITS_EEP   0x2
#define BITS_ESC   0x3

#define MAX_CREDIT_COUNT 56
#define MAX_FIFO_SIZE 64

#define CHAR_CONTROL_SIZE 4
#define CHAR_DATA_SIZE 10

#ifdef VERBOSE_ERRORS
#	define VEPRINT(f, fmt, ...) fprintf(f, fmt, ## __VA_ARGS__)
#else
#	define VEPRINT(f, fmt, ...) ((void)0)
#endif

struct port {
	char name[16];
	int addr;
	struct port *endp;

	struct {
		unsigned long long cycle;
		unsigned num_parity_errors;
		unsigned num_credit_errors;
		unsigned num_escape_errors;
		unsigned num_strobe_errors;
		unsigned num_eep;
		unsigned num_eop;
		unsigned num_rx_data_char;
		unsigned num_rx_ctrl_char;
		unsigned num_tx_data_char;
		unsigned num_tx_ctrl_char;
	} info;
	struct {
		int nt, nr, i;
		char tbuf[MAX_FIFO_SIZE], rbuf[MAX_FIFO_SIZE];
	} net;
	struct {
		int tx_credits;
		int tx_outstanding;
	} flow;
	struct {
		int nt, nr;
		int add_fct, got_esc;
		int tx_char, tx_parity, tx_data, tx_strobe, tx_size;
		int rx_char, rx_parity, rx_data, rx_strobe, rx_buffer;
	} sig;
	struct {
		int reset;
		int signal;
	} phys;
};

struct route {
	int addr, port;
	const char *msg;
};

extern struct port ports[], router[];
extern const struct route routes[];

void phys_tx(struct port *);
void phys_rx(struct port *);
void phys_reset(struct port *, int);

int sig_tx(struct port *);
void sig_rx(struct port *, int);
void sig_init(struct port *);

int flow_tx(struct port *);
int flow_rx(struct port *, int);
void flow_init(struct port *);

int net_tx(struct port *);
void net_rx(struct port *, int);
void net_init(struct port *);

#endif
