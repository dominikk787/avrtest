/*
 * scheduler.h
 *
 * Created: 2019-06-06 17:52:00
 *  Author: programowanie
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#include <avr/io.h>

#define FUNCS_MAX_COUNTER 8
#define SEMAPHORE_POSITIVE 1
#define SEMAPHORE_NEGATIVE 0
#define SEMAPHORE_NONE 2

typedef struct {
	uint8_t semdata[FUNCS_MAX_COUNTER];
} Semaphores;

typedef struct {
	uint8_t sn;
	uint8_t semn[FUNCS_MAX_COUNTER];
	void (*func)(void);
} FuncScheduler;

typedef struct {
	Semaphores s;
	FuncScheduler fsch[FUNCS_MAX_COUNTER];
	uint8_t c;
} Scheduler;

void initSem(Semaphores *s);
void initFSch(FuncScheduler *fsch);
void initSch(Scheduler *sch);
uint8_t registerFSch(Scheduler *sch, void (*func)(void));
void setSem(Scheduler *sch, uint8_t n, uint8_t state);
uint8_t getSem(Scheduler *sch, uint8_t n);
void connSem(Scheduler *sch, uint8_t fn, uint8_t sn, uint8_t sstate);
uint8_t isFActive(Scheduler *sch, uint8_t fn);
void runFunc(Scheduler *sch, uint8_t fn);
void __attribute__((noreturn)) run(Scheduler *sch);
#endif /* SCHEDULER_H_ */