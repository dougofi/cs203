/* 
Douglas Adjei-Frempah 
862259280
Fall 2021
*/


#ifndef PREFETCHER_H
#define PREFETCHER_H

#include <stdint.h>
#include "mem-sim.h"
#include <sys/types.h>
#include <stdlib.h> 

#define UPPER_BOUND_OF_CAPACITY 256

struct entity
{
	int difference; 
	int previous; 
};

class Prefetcher {
  private:
	bool isready; 
	Request afterRequest; 

	/* a 256-entry table*/
	struct entity *entities;
	int awaiting; 
	int previous_address;  
	int preceding_difference; 
	int mycount;

  public:
	Prefetcher();
	~Prefetcher();

	void HouseKeeping (Request req);

	void flush_function(void); 


	// should return true if a request is ready for this cycle
	bool hasRequest(u_int32_t cycle);

	
	// request a desired address be brought in
	Request getRequest(u_int32_t cycle);

	// this function is called whenever the last prefetcher request was successfully sent to the L2
	void completeRequest(u_int32_t cycle);

	/* Find the next candidate entry to prefetch */
	int get_following_request (int difference ); 

	/*
	 * This function is called whenever the CPU references memory.
	 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
	 */
	void cpuRequest(Request req);

	
	/* this fetches the immediate past entry*/
	int get_previous (int difference ); 

	
};

#endif
