	EXPORT OS_CPU_SR_Save
    EXPORT OS_CPU_SR_Restore
    EXPORT OSCtxSw
    EXPORT OSIntCtxSw
    EXPORT OSStartHighRdy
    EXPORT OS_CPU_PendSVHandler
    
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8
    
    
    ; 保存中断状态并关中断
OS_CPU_SR_Save PROC
    MRS   R0, PRIMASK      ; 读取PRIMASK/BASEPRI（中断状态寄存器）并存入R0
    CPSID I                ; 关闭中断
    BX    LR               ; 返回
    ENDP
    
   ; 恢复中断状态
OS_CPU_SR_Restore PROC
    MSR   PRIMASK, R0      ; 将R0的值写回PRIMASK/BASEPRI寄存器
    BX    LR
    ENDP
    ; 任务上下文切换，此处为空实现，仅供通过编译。以后需要实现任务上下文切换代码。
OSCtxSw PROC    
    BX LR
    ENDP
    ; 中断级任务切换，空实现
OSIntCtxSw PROC
    BX LR
    ENDP
    ; 启动最高优先级任务，空实现，实际需要初始化任务堆栈并启动第一个任务   
OSStartHighRdy PROC
    BX LR
    ENDP
    ; PendSV 中断处理，空实现，实际应实现PendSV中断处理以完成任务切换   
OS_CPU_PendSVHandler PROC
    BX LR
    ENDP
    END