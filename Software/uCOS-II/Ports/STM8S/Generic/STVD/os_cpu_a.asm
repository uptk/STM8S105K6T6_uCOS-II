;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2012, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           ARM Cortex-M0 Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.92.07.00
; By        : JJL
;             BN
;			  MCD
;
; For       : ARMv6M Cortex-M0
; Mode      : Thumb2
; Toolchain : RealView Development Suite
;             RealView Microcontroller Development Kit (MDK)
;             ARM Developer Suite (ADS)
;             Keil uVision
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    xref  _OSRunning                                           ; External references
    xref  _OSPrioCur
    xref  _OSPrioHighRdy
    xref  _OSTCBCur
    xref  _OSTCBHighRdy
    xref  _OSIntExit
    xref  _OSTaskSwHook
    xref  _OS_CPU_ExceptStkBase
		xref	_OSTimeTick
		xref	_OSIntEnter
		xref	_OSIntExit
xref	_Uart_RxPrc

	xdef	f_UART2_RX_IRQHandler
	xdef  _OS_CPU_SR_Save                                      ; Functions declared in this file
    xdef  _OS_CPU_SR_Restore
    xdef  f_OSStartHighRdy
    xdef  f_OSCtxSw
    xdef  f_OSIntCtxSw
    xdef  f_OS_CPU_SysTickHandler
OS_TASK_SW_HOOK_EN:	equ	0
OS_CPU_HOOKS_EN:	equ	0

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************


;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    switch	.text

;********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3                   /* Allocate storage for CPU status register */
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();                      /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();                       /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;********************************************************************************************************
;stack=2+1=3
_OS_CPU_SR_Save:
	PUSH CC
	POP A
	SIM
	RET
;statck = 2+1=3
_OS_CPU_SR_Restore:
		PUSH A
		POP CC
    RET
;该宏用于在切换任务时保存当前任务的运行时环境入栈
SAVE_REGS:  MACRO 
	pushw y
	pushw x        
	push  a
	push  cc        
	ENDM
;该宏用于切换任务时恢复要运行任务的堆栈环境
;借助iret指令中对PC寄存器重新赋值的操作来实现任务的切换
RESTORE_REGS:  MACRO     
	IRET
	ENDM  
	
;该宏用于保存当前任务的堆栈指针
SAVE_OLD_STK_PTR: MACRO                        
	LDW    Y,_OSTCBCur                         ;Y=OSTCBCur
	LDW    X,SP                                 ;X=SP
	LDW    (Y),X                                ;OSTCBCur->OSTCBStkPtr=SP       
	ENDM	
	
;该宏用于读取要运行任务的堆栈指针
LOAD_NEW_STK_PTR: MACRO
	LDW     Y,   _OSTCBHighRdy                 ;Y=OSTCBHighRdy     
	LDW     Y,(Y)                               ;Y= OSTCBHighRdy->OSTCBStkPtr
	LDW     SP, Y         
	ENDM
;********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set the main stack to OS_CPU_ExceptStkBase;
;              d) Set OSRunning to TRUE;
;              e) Trigger PendSV exception;
;              f) Enable interrupts (tasks will run with interrupts enabled).
;********************************************************************************************************
;stack=stack(_OSTaskSwHook)+9+3
f_OSStartHighRdy:
if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
	CALLF	_OSTaskSwHook	          ;OSTaskSwHook()
endif
	MOV		_OSRunning,#1	          ;OSRunning=1
	
	LOAD_NEW_STK_PTR	
	RESTORE_REGS      


;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;********************************************************************************************************
;stack=stack(_OSTaskSwHook)+9
f_OSCtxSw:
    	SAVE_REGS
	SAVE_OLD_STK_PTR
	if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
	CALLF	_OSTaskSwHook		            	;OSTaskSwHook()
	endif
	MOV     _OSPrioCur,_OSPrioHighRdy         ;OSPrioCur=OSPrioHighRdy
	LDW     X, _OSTCBHighRdy 
	LDW     _OSTCBCur, X                     ;OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	RESTORE_REGS
;********************************************************************************************************
;                             PERFORM A CONTEXT SWITCH (From interrupt level)
;                                         void OSIntCtxSw(void)
;
; Notes:    1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;********************************************************************************************************
;stack=stack(_OSTaskSwHook)+9
f_OSIntCtxSw:
if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN > 0)
	CALLF	_OSTaskSwHook			   			;OSTaskSwHook()
endif	
	MOV     _OSPrioCur,_OSPrioHighRdy         ;OSPrioCur=OSPrioHighRdy
	LDW		X, _OSTCBHighRdy 
	LDW     _OSTCBCur, X                       ;OSTCBCur=OSTCBHighRdy
	
	LOAD_NEW_STK_PTR
	RESTORE_REGS 
;stack=max(
;		stack(OSTaskSwHook),
;		stack(OSIntEnter),
;		stack(OSTimeTick),
;		stack(OSIntExit)
;	)+9
f_OS_CPU_SysTickHandler:    
	SIM;有什么作用
	
	;INT_SAVE_REGS
	
	SAVE_OLD_STK_PTR
	
	BRES	0x5255, #0	
	BRES	0x5256, #0	;清除中断标志位
	
	CALL	_OSIntEnter
	CALL	_OSTimeTick
	CALL	_OSIntExit
	
	RESTORE_REGS
	
f_UART2_RX_IRQHandler:    
	SIM;有什么作用
	
	;INT_SAVE_REGS
	
	SAVE_OLD_STK_PTR
	
	
	CALL	_OSIntEnter
	CALL	_Uart_RxPrc
	CALL	_OSIntExit
	
	RESTORE_REGS	
;********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                     void OS_CPU_PendSVHandler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M0.  This is because the Cortex-M0 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R7 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs R4-R7 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R7 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************

    END
