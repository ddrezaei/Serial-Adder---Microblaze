/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include "seraddcore.h"

void print(char *str);
int usleep(unsigned int useconds);

int main()
{
    int a = 0, b = 0;
	int c = 0;

	init_platform();

    print("Hello World: \n");
    print("Start the program ... \n");

   // set input a=8
    SERADDCORE_mWriteReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG0_OFFSET, 0x46);

    // set input b=5
    SERADDCORE_mWriteReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG1_OFFSET, 0x12);

    // set start=1 and then start=0
    SERADDCORE_mWriteReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG2_OFFSET, 0x1);
    usleep(1);
    SERADDCORE_mWriteReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG2_OFFSET, 0x0);


    // get output c:
    c = SERADDCORE_mReadReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG3_OFFSET);

    a = SERADDCORE_mReadReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG0_OFFSET);
    b = SERADDCORE_mReadReg(XPAR_SERADDCORE_0_BASEADDR, SERADDCORE_SLV_REG1_OFFSET);

    // print out the result
    xil_printf("Output = %d \n", c);
    xil_printf("a = %d , b = %d \n\n\r", a, b);

    cleanup_platform();

    return 0;
}


/*
 * based on 100MHz Processor System Clock
 * Using the timer in an inlined polled way.
 */
int usleep(unsigned int useconds) {
	u32 Value1;
	u32 Value2;
	u32 ControlStatus;
	u8 TmrCtrNumber = 0;

	/*
	 * Clear the Control Status Register
	 */
	XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber,0x0);

	/*
	 * Set the value that is loaded into the timer counter and cause it to
	 * be loaded into the timer counter, it will be a count down.
	 * Assume 200/3MHz clock (15ns) so to get to uSeconds we need to multiple by 1000/15=66.666
	 */
	XTmrCtr_SetLoadReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber, (u32) (useconds * 67));
	XTmrCtr_LoadTimerCounterReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber);

	/*
	 * Clear the Load Timer bit in the Control Status Register
	 */
	ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR,
			TmrCtrNumber);
	XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber,
			(ControlStatus & (~XTC_CSR_LOAD_MASK)));

	/*
	 * Set the counter to a down counter.
	 */
	ControlStatus = XTmrCtr_GetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR,
			TmrCtrNumber);
	XTmrCtr_SetControlStatusReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber,
			(ControlStatus | (XTC_CSR_DOWN_COUNT_MASK)));

	/*
	 * Get a snapshot of the timer counter value before it's started
	 * to compare against later
	 */
	Value1 = XTmrCtr_GetTimerCounterReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber);

	/*
	 * Start the timer counter such that it's incrementing by default
	 */
	XTmrCtr_Enable(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber);

	/*
	 * Read the value of the timer counter and wait for it to change,
	 * since it's incrementing it should change, if the hardware is not
	 * working for some reason, this loop could be infinite such that the
	 * function does not return
	 */
	//xil_printf("--Debug; Timer Initial Value: 0x%8h \r\n", Value1);
	while (1) {
		Value2 = XTmrCtr_GetTimerCounterReg(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber);

		if (Value2 == 0xFFFFFFFF) {
			/* Because of the timer overflow it stops after it passes zero. */
			break;
		}
	}
	//xil_printf("--Debug; Timer End Value: 0x%8h \r\n", Value2);

	/*
	 * Disable the timer counter such that it stops incrementing
	 */
	XTmrCtr_Disable(XPAR_XPS_TIMER_0_BASEADDR, TmrCtrNumber);

	return XST_SUCCESS;
}
