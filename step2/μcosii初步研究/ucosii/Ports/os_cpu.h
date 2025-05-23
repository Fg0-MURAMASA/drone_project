#ifndef OS_CPU_H
#define OS_CPU_H


typedef unsigned char BOOLEAN;
typedef unsigned char INT8U;
typedef signed char INT8S;
typedef unsigned short INT16U;
typedef signed short INT16S;
typedef unsigned int INT32U;
typedef signed int INT32S;
typedef float FP32;
typedef double FP64;
typedef unsigned int OS_STK;
typedef unsigned int OS_CPU_SR;

#define         OS_CRITICAL_METHOD 3u
#if                 OS_CRITICAL_METHOD ==3u

OS_CPU_SR OS_CPU_SR_Save(void);
void OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);

#define         OS_ENTER_CRITICAL()    { cpu_sr = OS_CPU_SR_Save(); } 
#define         OS_EXIT_CRITICAL()     { OS_CPU_SR_Restore(cpu_sr); }
#endif

#define                OS_TASK_SW() OSCtxSw()

#define                OS_STK_GROWTH        1u

void                OSCtxSw(void);
void                OSIntCtxSw(void);
void                OSStartHighRdy(void);
#endif
