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
	      b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  

	      b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

/*This section is for the setup of the GIOP*/
//Enable CMU clock
ldr r1, =CMU_BASE
ldr r2, =CMU_HFPERCLKEN0
mov r3, 0b10000000000000
str r3, [r2, r1]
//Setup for port C
ldr r1, =GPIO_PC_BASE
ldr r2, =GPIO_MODEL
mov r0, 0x33333333
str r0, [r2, r1]//Configure gpio 0 to 7 to be inputs

//Setup for port a
ldr r1, =GPIO_PC_BASE
mov r0, 0x2
str r0, [r1]
ldr r2, =GPIO_MODEH
mov r0, 0x55555555
str r0, [r2, r1]//Configure gpio 8 to 15 to be outputs

//Save some values that will be needed in the loop
ldr r1, =GPIO_PA_BASE
ldr r0, =GPIO_DOUT
add r1, r1, r0//Port A DOUT register
ldr r2, =GPIO_PC_BASE
ldr r0, =GPIO_DIN
add r2, r2, r0//Port C DIN register
//Infinite loop that polls gpio for input each iteration
loop:
	//Check if pin 0 is pushed
	and r3, r2, 0b00000001
	cmp r3, 1
	//If pin 0 is pushed, jump to pin0pushed
	beq pin0pushed
	//Turn off light 0
	ldr r4, [r1]
	and r4, r4, 0
	b cont0
	pin0pushed:
		//Turn on light 0
		ldr r4, [r1]
		orr r4, r4, 0b00000001
		str r1, [r4]
	cont0:
	b loop
