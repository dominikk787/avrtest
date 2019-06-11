/*
 * scheduler.c
 *
 * Created: 2019-06-06 17:51:43
 *  Author: programowanie
 */ 
#include <string.h>
#include "scheduler.h"

void initSem(Semaphores *s) {
	memset((*s).semdata, SEMAPHORE_NONE, FUNCS_MAX_COUNTER);
}

void initFSch(FuncScheduler *fsch){
	(*fsch).sn = 0;
	memset((*fsch).semn, SEMAPHORE_NONE, FUNCS_MAX_COUNTER);
	(*fsch).func = NULL;
}

void initSch(Scheduler *sch) {
	initSem(&((*sch).s));
	for(uint8_t i = 0; i < FUNCS_MAX_COUNTER; i++) {
		initFSch(&((*sch).fsch[i]));
	}
	(*sch).c = 0;
}

uint8_t registerFSch(Scheduler *sch, void (*func)(void)) {
	uint8_t n = (*sch).c;
	if(n < FUNCS_MAX_COUNTER) {
		(*sch).fsch[n].func = func;
		(*sch).s.semdata[n] = 0;
		(*sch).fsch[n].sn = (*sch).c;
		(*sch).c++;
	}
	return n;
}

void setSem(Scheduler *sch, uint8_t n, uint8_t state) {
	(*sch).s.semdata[n] = state;
}

uint8_t getSem(Scheduler *sch, uint8_t n) {
	return (*sch).s.semdata[n];
}

void connSem(Scheduler *sch, uint8_t fn, uint8_t sn, uint8_t sstate) {
	(*sch).fsch[fn].semn[sn] = sstate;
}

uint8_t isFActive(Scheduler *sch, uint8_t fn) {
	for(uint8_t i = 0; i < FUNCS_MAX_COUNTER; i++) {
		if((*sch).fsch[fn].semn[i] != SEMAPHORE_NONE) {
			if((*sch).fsch[fn].semn[i] != (*sch).s.semdata[i]) {
				return 0;
			}
		}
	}
	return 1;
}

void runFunc(Scheduler *sch, uint8_t fn) {
	(*sch).fsch[fn].func();
}

void run(Scheduler *sch) {
	while(1) {
		for(uint8_t i = 0; i < (*sch).c; i++) {
			if(isFActive(sch, i)) runFunc(sch, i);
		}
	}
}