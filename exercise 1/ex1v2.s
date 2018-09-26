        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset:
	//enable GPIO clock
	ldr r1, =CMU_BASE
	ldr r2, =CMU_HFPERCLKEN0
	ldr r3, [r1, r2]
	mov r4, 1
	ldr r5, =CMU_HFPERCLKEN0_GPIO
	lsl r4, r4, r5
	orr r3, r3, r4
	str r3, [r1, r2]
	
	//Enable LEDS for port A
	ldr r1, =GPIO_PA_BASE
	//Set high drive strenght
	mov r2, 0x2
	str r2, [r1]
	//Set pins 8-15 to output
	ldr r2, =GPIO_MODEH
	mov r3, 0x55555555
	str r3, [r1, r2]
	//Turn off LEDS
	ldr r2, =GPIO_DOUT
	mov r3, 0xffffffff
	str r3, [r1, r2]
	
	//Enable port c buttons for input on pins 0-7
	ldr r1, =GPIO_PC_BASE
	ldr r2, =GPIO_MODEL
	mov r3, 0x33333333
	str r3, [r1, r2]
	ldr r2, =GPIO_DOUT
	mov r3, 0xff
	str r3, [r1, r2]
	
	//Configure GPIO control registers to allow for interrupts
	ldr r1, =GPIO_BASE
	ldr r2, =GPIO_EXTIPSELL
	mov r3, 0x22222222
	str r3, [r1, r2]
	mov r3, 0xff
	ldr r2, =GPIO_EXTIRISE
	str r3, [r1, r2]
	ldr r2, =GPIO_EXTIFALL
	str r3, [r1, r2]
	ldr r2, =GPIO_IEN
	str r3, [r1, r2]
	
	//Enable NVIC
	ldr r1, =ISER0
	ldr r2, =0x802
	str r2, [r1]
	
	//Enable deep sleep and exit to sleep
	ldr r1, =SCR
	mov r2, 0b110
	str r2, [r1]
	wfi

	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////

        .thumb_func
gpio_handler:
	ldr r1, =GPIO_BASE
	ldr r2, =GPIO_IF
	ldr r3, =GPIO_IFC
	ldr r4, [r1, r2]
	str r4, [r1, r3]
	
	ldr r1, =GPIO_PC_BASE
	ldr r2, =GPIO_DIN
	ldr r3, [r1, r2]
	mov r4, 8
	lsl r3, r3, r4
	ldr r1, =GPIO_PA_BASE
	ldr r2, =GPIO_DOUT
	str r3, [r1, r2]
	bx lr	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

