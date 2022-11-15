/*
 * sysreg.h
 *
 *  Created on: May 18, 2021
 *      Author: Morgoth
 */

#ifndef SRC_SYSREG_H_
#define SRC_SYSREG_H_

#define REG_MSTATUS					(0x00)	//Machine status register.
#define REG_MIE						(0x04)	//Machine interrupt-enable register.
#define REG_RIP						(0x08) //Return from interupt address.
#define REG_MSCRATCH				(0x0c)	//Scratch register for machine trap handlers.
#define REG_MEPC					(0x10)	//Machine exception program counter.
#define REG_MCAUSE					(0x14)	//Machine trap cause.
#define REG_MBADADDR				(0x18)	//Machine bad address.

#define REG_MSTATUS_MIE_bp			3
#define REG_MSTATUS_HIE_bp			2
#define REG_MSTATUS_SIE_bp			1
#define REG_MSTATUS_UIE_bp			0



#endif /* SRC_SYSREG_H_ */
