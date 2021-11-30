/* 
Douglas Adjei-Frempah 
862259280
Fall 2021
*/


#include "prefetcher.h"
#include <stdio.h>


void Prefetcher::HouseKeeping (Request req ){
		isready = true;
		mycount = 0;
}


Prefetcher::~Prefetcher() {
}

bool Prefetcher::hasRequest(u_int32_t cycle)
{
	mycount++;
	if (mycount > 2)
		isready = false;
	else
		isready = true;

	return isready;
}


void Prefetcher::completeRequest(u_int32_t cycle)
{
	return;
}

void Prefetcher::flush_function(void)
{
	//flushing
	awaiting++;
	if (awaiting <= UPPER_BOUND_OF_CAPACITY)
		awaiting = 0;
}


int Prefetcher::get_previous(int difference)
{
	int waiting_pool = awaiting;
	int i;
	bool isPresent = false;

	for (i = 0; i < UPPER_BOUND_OF_CAPACITY - 1; i++) {
		if (waiting_pool == 0)
			waiting_pool = UPPER_BOUND_OF_CAPACITY - 1;
		else
			waiting_pool = waiting_pool - 1;

		if (entities[waiting_pool].difference == difference) {
			 isPresent = true;
			break;
		}
	}

	if (isPresent)
		return waiting_pool;
	else
		return -1;
}


int Prefetcher::get_following_request(int difference)
{
	int waiting_pool = awaiting;
	int candidate;
	int i;
	bool isPresent = false;
	struct entity container[10];
	

	while(i < 10 ){ 
		container[i].difference = 0 ; 
		container[i].previous = 0 ;
		i = i + 1 ; 
	}


	int num = 0;
	int flip = 0;
	int max = 0;
	int index = 0;
	int next_pos = 0;
	int temp_diff = 0;
	bool first_cycle = true;


	while(1) {
		if (!first_cycle) {
			candidate = waiting_pool + 1;
			if (candidate == UPPER_BOUND_OF_CAPACITY)
				candidate = 0;

			temp_diff = entities[candidate].difference;
			if (temp_diff) {
				for (i = 0; i < num; i++) {
					if (container[i].difference == temp_diff) {
						container[i].previous++;
						break;
					}
				}

				if (i == num && num < 10) { 
					container[num].difference= temp_diff;
					container[num].previous++;
					num++;
				}
			}
		} else {
			first_cycle = false;
		}

		next_pos = entities[waiting_pool].previous;
		if (next_pos == -1)
			break;
		if (next_pos >= waiting_pool && flip)
			break;
		if (next_pos >= waiting_pool)
			flip = 1;
		if (entities[next_pos].difference != difference)
			break;

		waiting_pool = next_pos;
	}

	for (i = 0; i < num; i++) {
		if (container[i].previous > max) {
			max = container[i].previous;
			index = i;
		}
	}

	if (max && container[index].difference)
		return container[index].difference;
	else
		return 0;
}


Request Prefetcher::getRequest(u_int32_t cycle)
{
	int next_diff;

	if (mycount == 1)
		next_diff = preceding_difference;
	else if (mycount == 2)
		next_diff = preceding_difference - 32;
	else
		next_diff = preceding_difference + 32;

	afterRequest.addr = previous_address + next_diff;
	return afterRequest;
}



void Prefetcher::cpuRequest(Request req)
{
	int difference;
	int next_diff;

	if(!req.HitL1 ) {
		if(!isready){
		difference = req.addr - previous_address;

		entities[awaiting].difference = difference;
		entities[awaiting].previous = get_previous(difference);
		flush_function();
		next_diff = get_following_request(difference);

		if (next_diff >= 0 && next_diff < 64) {
			next_diff = 64;
		} else if (next_diff < 0 && next_diff > -64){
			next_diff = -64;
		}


		isready = true;
	
		previous_address = req.addr;
	
		mycount = 0;
		
		}
		
	}
}






Prefetcher::Prefetcher() {
	int i;
	previous_address = 0;
	preceding_difference = 0;
	mycount = 0;
	isready = false;
	afterRequest.addr = 0;
	awaiting = 0;
	
	entities = (struct entity *)malloc(sizeof(struct entity) * UPPER_BOUND_OF_CAPACITY);
	
	int entity_manager = UPPER_BOUND_OF_CAPACITY; 

	while(entity_manager > 0){ 
		entities[i].previous = -1 ; 
		entities[i].difference = 0 ;
		entity_manager = entity_manager - 1; 
	}

}
