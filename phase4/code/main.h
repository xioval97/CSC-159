// main.h, 159

#ifndef _MAIN_H_
#define _MAIN_H_

<<<<<<< HEAD
void main();
void InitData();
void Scheduler();
void Kernel();
=======
void SetIDTEntry(int, func_ptr_t);
void main();
void InitControl();
void InitData();
void Scheduler();
void Kernel(tf_t *);
>>>>>>> b8a9e70220cc217dedb00b209013c8d21b381054

#endif
