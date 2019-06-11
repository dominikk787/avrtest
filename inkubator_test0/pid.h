/*
 * pid.h
 *
 * Created: 2019-06-09 16:04:33
 *  Author: programowanie
 */ 


#ifndef PID_H_
#define PID_H_

#define PID_Kp 16
#define PID_Ki 1
#define PID_Kd 16

uint16_t pid(uint16_t input, uint16_t setpoint);

#endif /* PID_H_ */