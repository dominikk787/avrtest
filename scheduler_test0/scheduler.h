/*
 * scheduler.h
 *
 * Created: 2019-06-06 12:28:11
 *  Author: programowanie
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#include <avr/io.h>

#define SEMAPHORES_MAX_COUNTER 16
#define SEMAPHORE_POSITIVE 1
#define SEMAPHORE_NEGATIVE 2
#define SEMAPHORE_NONE 0

typedef struct {
	uint8_t semdata[SEMAPHORES_MAX_COUNTER];
	uint8_t semcounter;
} SemaphoresData;

class Semaphores {
	public:
		Semaphores(SemaphoresData * semd);
		uint8_t add();
		void set(uint8_t n);
		void reset(uint8_t n);
		uint8_t get(uint8_t n);
	private:
		SemaphoresData *sd;
};

class FunctionScheduler {
	public:
		FunctionScheduler(SemaphoresData * semd);
		uint8_t get();
		uint8_t getn();
		void set();
		void reset();
		void add(FunctionScheduler funcs, uint8_t sstate);
		uint8_t isready();
	private:
		Semaphores s;
		SemaphoresData *sd;
		uint8_t sn;
		uint8_t semn[SEMAPHORES_MAX_COUNTER];
};

#endif /* SCHEDULER_H_ */