/*
 * =====================================================================================
 *
 *       Filename:  prsim.h
 *
 *    Description:  Simulates a virtual memory environment for a single process. 
 *
 *        Version:  1.0
 *        Created:  28/11/2012 21:16:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Connor Moreside (conman720), cmoresid@ualberta.ca
 *   Organization:  CMPUT379
 *
 * =====================================================================================
 */

#define PAGE_ROUND_UP(pagesize,x) ( (((int)(x)) + pagesize-1)  & (~(pagesize-1)) )

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  print_usage()
 *  Description:  Prints parameters that should be passed to prsim
 *	  Arguments:  None
 *      Returns:  (void)
 * =====================================================================================
 */
void print_usage();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  start_simulation(char strategy, int pagesize, int memsize)
 *  Description:  
 *	  Arguments:  
 *      Returns:  0 if successful, 1 if something went wrong
 * =====================================================================================
 */
int start_simulation(char strategy, int pagesize, int memsize);