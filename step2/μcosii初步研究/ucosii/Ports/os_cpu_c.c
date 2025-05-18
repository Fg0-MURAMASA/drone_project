#include "os_cpu.h"

void SystemInit(){}

void OSInitHookBegin(){}
void OSInitHookEnd(){}
void OSTCBInitHook(){}
void OSTaskCreateHook(){}
void OSTaskIdleHook(){}
void OSTaskReturnHook(){}
void OSTaskSwHook(){}
void OSTimeTickHook(){}
void OSTaskStatHook(){}
void OSTaskDelHook(){}
        

OS_STK *OSTaskStkInit(void (*task)(void *p_arg), void *p_arg, OS_STK *ptos, 
INT16U opt){
return ptos;
}


void OS_CPU_ExceptStkBase(){}

	
void OS_KA_BASEPRI_Boundary(){}
