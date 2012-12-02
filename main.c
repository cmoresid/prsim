#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "prsim.h"

void print_usage();

int main(int argc, char* argv[]) {
	/* Contains the raw string from getopt */
	char* strategy_full = NULL;
	/* After strategy_full sanitized, contains replacement policy */
	char strategy = '0';
	/* Size of the pages */
	int pagesize = 0;
	/* Size of the real memory */
	int memsize = 0;
	/* Scaled size of real memory, if needed */
	int new_memsize = 0;
	/* Used by getopt to store state */
	int c;

	// Make sure there are the proper # of arguments
	if (argc != 7) {
		print_usage();
		exit(1);
	}

	// Parse parameters
	opterr = 0;	
	while ( (c = getopt(argc, argv, "s:p:m:h::")) != -1) {
		switch (c) {
			case 's':
				strategy_full = optarg;
				break;
			case 'p':
				pagesize = atoi(optarg);
				break;
			case 'm':
				memsize = atoi(optarg);
				break;
			case 'h':
				print_usage();
				exit(0);
				break;
			case '?':
				if (optopt == 's' || optopt == 'p' || optopt == 'm')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
			default:
				print_usage();
				exit(1);
		}
	}
	
	// Verify strategy parameter
	if (strlen(strategy_full) > 1 || (strategy_full[0] != 'r' && strategy_full[0] != 'f' && strategy_full[0] != 'l')) {
		fprintf(stderr, "Strategy must be either 'r', 'f', or 'l'\n");
		exit(1);
	} else {
		strategy = strategy_full[0];
	}
	
	// Verify page size parameter
	if (pagesize < 32 || pagesize > 8192) {
		fprintf(stderr, "Page size must be between 32 and 8192 bytes, inclusive\n");
		exit(1);
	}
	if (!((pagesize & (~pagesize + 1)) == pagesize)) {
		fprintf(stderr, "Page size must be a power of 2.\n");
		exit(1);
	}
	
	// Verify memory size parameter
	if (memsize <= 0) {
		fprintf(stderr, "Memory size must be greater than zero.\n");
		exit(1);
	}
	
	// Round memory size up to nearest multiple of page size if necessary
	new_memsize = PAGE_ROUND_UP(pagesize, memsize);
	if (new_memsize != memsize) {
		printf("Note: Memory size scaled to %d bytes.\n", new_memsize);
	}
	memsize = new_memsize;
	
	// Now start simulation!
	return start_simulation(strategy, pagesize, memsize);
}

void print_usage() {
	fprintf(stderr, "Usage: prsim -s strategy -p pagesize -m memsize\n");
}
