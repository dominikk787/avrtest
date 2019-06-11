/*
 * scheduler.cpp
 *
 * Created: 2019-06-06 12:27:53
 *  Author: programowanie
 */ 

#include "scheduler.h"
#include <string.h>
#include <stdio.h>

Semaphores::Semaphores(SemaphoresData *semd) {
	sd = semd;
	memset((*sd).semdata, 0, SEMAPHORES_MAX_COUNTER);
	(*sd).semcounter = 0;
}

uint8_t Semaphores::add() {
	if((*sd).semcounter < SEMAPHORES_MAX_COUNTER) {
		(*sd).semcounter++;
		reset((*sd).semcounter);
		return (*sd).semcounter;
	}
	return 0;
}

void Semaphores::set(uint8_t n) {
	(*sd).semdata[n - 1] = 1;
}

void Semaphores::reset(uint8_t n) {
	(*sd).semdata[n - 1] = 0;
}

uint8_t Semaphores::get(uint8_t n) {
	return (*sd).semdata[n - 1];
}

FunctionScheduler::FunctionScheduler(SemaphoresData *semd) {
	sd = semd;
	s = Semaphores(sd);
	sn = s.add();
	memset(semn, 0, SEMAPHORES_MAX_COUNTER);
}

void FunctionScheduler::set() {
	s.set(sn);
}

void FunctionScheduler::reset() {
	s.reset(sn);
}

uint8_t FunctionScheduler::get() {
	return s.get(sn);
}

uint8_t FunctionScheduler::getn() {
	return sn;
}

void FunctionScheduler::add(FunctionScheduler funcs, uint8_t sstate) {
	uint8_t n = funcs.getn();
	semn[n - 1] = sstate;
}

uint8_t FunctionScheduler::isready() {
	uint8_t state = 1;
	for(uint8_t i = 0; i < SEMAPHORES_MAX_COUNTER; i++) {
		if(semn[i] != SEMAPHORE_NONE) {
			if(semn[i] == SEMAPHORE_POSITIVE) {
				state = s.get(i) ? state : 0;
			} else {
				state = s.get(i) ? 0 : state;
			}
		}
	}
	return state;
}