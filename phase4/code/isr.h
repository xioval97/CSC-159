// isr.h, 159

#ifndef _ISR_H_
#define _ISR_H_

<<<<<<< HEAD
void SpawnISR(int, func_ptr_t);
void KillISR();
void TimerISR();
void ShowStatusISR();
=======
void MyBzero(void *, int);
void SpawnISR(int, func_ptr_t);
void KillISR();
void ShowStatusISR();
void TimerISR();
>>>>>>> b8a9e70220cc217dedb00b209013c8d21b381054
void SleepISR(int);

int SemInitISR(int);
void SemWaitISR(int);
void SemPostISR(int);

void MsgSndISR();
void MsgRcvISR();

#endif
