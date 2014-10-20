/*
 * 1964 Copyright (C) 1999-2004 Joel Middendorf, <schibo@emulation64.com> This
 * program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details. You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. To contact the
 * authors: email: schibo@emulation64.com, rice1964@yahoo.com ? * This file contains newer memory access functions and definitions. The problem
 * of using __try__except blocks in the older functions is speed. - Need to use
 * __try__except blocks to catch memory access exception, will need to setup
 * except service routine vector at the beginning of the protected block, and need
 * to set the vector back at the end of block, which affects speed very much. Need
 * to use __try__except block to detect access to the protected memory address to
 * detect self-mod code - Need to check if the address is in K0/K1 range, or
 * otherwise need to translate the address via TLB - Cannot be used in
 * dyna-compiler. Dyna-compiler has to interpret all store/load opcodes because we
 * cannot do exception handling in dyna. The newer methods are actually trying to
 * use existing methods in other emus, which to use a function array to access
 * memory. by Rice -- 11/09/2001
 */
#include "stdafx.h"

/*#ifdef _XBOX
#include "rompaging.h" // needed?
#endif*/

/*
 * The reason I have to define memory block size as 4KB for write is for protected
 * memory methods ? * protected memory methods are using 4KB memory blocks ? * Memory access functions The memory read function are returning the pointer to
 * the memory, not the actual value ? * at the memory location, the calling function is responsible to read the value
 * from the ? * real memory location. ? * The reason I do this is because we don't know what memory access type is, in
 * BYTE, or WORD, DWORD ..
 */
#define _SAFETY_READ_MEM_(x)	{ __asm push ecx __asm mov ecx, eax __asm call x __asm pop ecx }

//Hack for games to write into ROM
BOOL write_to_rom_flag = FALSE;
uint32 write_to_rom_value[2] = {0,0};
uint32* prom_value = &write_to_rom_value[0];
extern BOOL NeedToApplyRomWriteHack;
HardwareState		gHardwareState;
MemoryState			gMemoryState;
/*
 =======================================================================================================================
 =======================================================================================================================
 */




__declspec (naked)
uint32 *mem_read_eax_only_helper(uint32 addr)
{
	__asm
	{
		mov eax, ecx
		shr ecx, SHIFTER2_READ
		call gHardwareState.memory_read_functions[ecx * 4]
        ret 0
	}
}


__declspec (naked)
uint32 *mem_write_eax_only_helper(uint32 addr)
{
	__asm
	{
		mov eax, ecx
		shr ecx, SHIFTER2_READ
		call gHardwareState.memory_write_fun_eax_only[ecx * 4]
        ret 0
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline __declspec (naked)
uint32 *read_mem_rdram_k0seg_eax_only(void)
{
	__asm
	{
		sub eax, 0x60000000 /* rdram is at 0x20000000 */
		ret 0
	}
}
__forceinline static __declspec (naked)
uint32 *write_mem_rdram_k0seg_eax_only(void)
{
    __asm
	{
		sub eax, 0x60000000 /* rdram is at 0x20000000 */
		ret 0
	}
}




/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline  __declspec (naked)
uint32 *read_mem_rdram_k1seg_eax_only(void)
{
	__asm
	{
		xor eax, 0x80000000 /* rdram is at 0x20000000 */
		ret 0
	}
}
__forceinline static __declspec (naked)
uint32 *write_mem_rdram_k1seg_eax_only(void)
{
	__asm
	{
		xor eax, 0x80000000 /* rdram is at 0x20000000 */
		ret 0
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_rdram_not_at_0x20000000_eax_only(void)
{
	/* Access by pointer */
	__asm
	{
		push ecx
		and eax, 0x007fffff
		mov ecx, gMemoryState
		add eax, [ecx + 56] /* rdram */
		pop ecx
	}
}
uint32 *write_mem_rdram_not_at_0x20000000_eax_only(void)
{
	/* Access by pointer */
	__asm
	{
		push ecx
		and eax, 0x007fffff
		mov ecx, gMemoryState
		add eax, [ecx + 56] /* rdram */
		pop ecx
    }
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_rdram_not_at_0x20000000_eax_only__Opcode_Debugger_is_off(void)
{
	/* Doesn't acces by pointer */
	__asm
	{
		and eax, 0x007fffff
		add eax, gMemoryState.RDRAM

		/* ret 0 //must not ret 0 if this function is not naked */
	}
}
uint32 *write_mem_rdram_not_at_0x20000000_eax_only__Opcode_Debugger_is_off(void)
{
	/* Doesn't acces by pointer */
	__asm
	{
		and eax, 0x007fffff
		add eax, gMemoryState.RDRAM

		/* ret 0 //must not ret 0 if this function is not naked */
	}
}



uint32	dummyWord[4];

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline __declspec (naked)
uint32 *read_mem_cart_eax_only(void)
{
	/* Access by pointer */
	__asm
	{
		cmp	write_to_rom_flag, TRUE
		jne l1
		mov write_to_rom_flag, FALSE;

#ifdef _DEBUG
		mov dummyWord[0], eax
		pushad
	}
	TRACE2("Read from ROM, addr = %08X, PC=%08X", dummyWord[0], r.r_.pc);

	__asm{
		popad
#endif
		mov eax, prom_value
		ret 0
l1:
		push ecx
		and eax, 0x07ffffff /* Rice: please confirm this mask ? */
		mov ecx, gMemoryState
		add eax, [ecx + 76] /* ROM_Image */
		pop ecx
		ret 0
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline __declspec (naked) 
uint32 *read_mem_cart_eax_only__Opcode_Debugger_is_off(void)
{
	/* doesn't access by pointer */
	__asm
	{
		cmp	write_to_rom_flag, TRUE
		jne l1
		mov write_to_rom_flag, FALSE;

#ifdef _DEBUG
		mov dummyWord[0], eax
		pushad
	}
	TRACE2("Read from ROM, addr = %08X, PC=%08X", dummyWord[0], r.r_.pc);

	__asm{
		popad
#endif
		mov eax, prom_value//dword ptr write_to_rom_value
		ret 0
l1:
		and eax, 0x07ffffff
		add eax, gMemoryState.ROM_Image
		ret 0
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__int32 lAddr;
__declspec(naked) uint32 *read_mem_io_eax_only(void)
{
	__asm mov lAddr, eax
	__asm push edx
	__asm push ecx
	Check_LW(lAddr);
	lAddr = (uint32) ((uint8 *) sDWORD_R[((uint16) ((lAddr) >> 16))] + ((uint16) (lAddr)));
	__asm mov eax, lAddr
	__asm mov ecx, [esp]
	__asm mov edx, [esp+4]
	__asm add esp, 8
	__asm ret 0
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_flashram(uint32 addr)
{
	if(currentromoptions.Save_Type == SRAM_SAVETYPE || gamesave.firstusedsavemedia == SRAM_SAVETYPE)
	{
		return gMS_C2A2+(addr&0x0001FFFFF);
	}
	else
	{
		__asm push edx;
		dummyWord[0] = Check_LW(addr);
		__asm pop edx;
		return dummyWord;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
__declspec(naked) 
uint32 *read_mem_flashram_eax_only(void)
{
	_SAFETY_READ_MEM_(read_mem_flashram);
	__asm ret 0
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_sram(uint32 addr)
{
	__asm push edx

	addr &= 0x00007FFF;
	__asm pop edx
	return(uint32 *) (gamesave.SRam + addr);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__declspec(naked)
uint32 *read_mem_sram_eax_only(void)
{
	_SAFETY_READ_MEM_(read_mem_sram) 
     __asm ret 0
}


__declspec(naked)
uint32 *read_mem_sp_eax_only(void)
{
    __asm
    {
        and         eax,1FFFh 
        add         eax,dword ptr gMemoryState.SP_MEM 
        ret
    }
}
__declspec(naked)
uint32 *write_mem_sp_eax_only(void)
{
    __asm
    {
        and         eax,1FFFh 
        add         eax,dword ptr gMemoryState.SP_MEM 
        ret
    }
}

__declspec(naked)
uint32 *write_mem_sp(uint32 addr)
{
    __asm
    {
        mov         eax,ecx 
        mov         ecx,dword ptr gMemoryState.SP_MEM 
        and         eax,1FFFh 
        add         eax,ecx
        ret
    }
}

/* This function is used when currentromoptions.UseTLB = No */

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void VIDEO_FrameBufferRead(DWORD addr);
void VIDEO_FrameBufferWrite(DWORD addr, DWORD size);
void init_rdram_region_func_array(uint32 startAddress, uint32 size);
void SetFrameBufferHasBeenRead(uint32 addr);
void SetFrameBufferHasBeenWritten(uint32 addr);

uint32 *write_mem_framebuffer(uint32 addr)
{
	__asm push edx
	__asm push eax
	__asm push ecx
	SetFrameBufferHasBeenWritten(addr);

	__asm pop ecx
	__asm pop eax
	addr = (uint32)(gMemoryState.RDRAM + (addr&0x7FFFFF));
	__asm pop edx
	return(uint32 *) addr;
}
uint32 *write_mem_framebuffer_eax_only()
{
	uint32 addr;

    __asm mov addr, eax
    __asm push edx
	__asm push eax
	__asm push ecx
	SetFrameBufferHasBeenWritten(addr);

	__asm pop ecx
	__asm pop eax
	addr = (uint32)(gMemoryState.RDRAM + (addr&0x7FFFFF));
	__asm pop edx
	return(uint32 *) addr;
}



uint32 *read_mem_framebuffer(uint32 addr)
{
	__asm push edx;

	// This checking could eliminate many unnecessary calls to the function SetFrameBufferHasBeenRead
	//if( (uint32) gHardwareState.memory_write_fun_eax_only[(addr / 0x1000) >> SHIFTER1_WRITE] == (uint32) write_mem_framebuffer_eax_only )
	//{
		// Need to call Video Plugin function to inform the memory read
		SetFrameBufferHasBeenRead(addr);
	//}


	addr = (uint32)(gMemoryState.RDRAM + (addr&0x7FFFFF));
	__asm pop edx
	return(uint32 *) addr;
}

__declspec(naked)
uint32 *read_mem_framebuffer_eax_only(void)
{
	_SAFETY_READ_MEM_(read_mem_framebuffer) 
    __asm ret 0
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__declspec(naked)
uint32 *read_mem_tlb_eax_only__UseTLB_No(void)
{
	__asm
	{
		push edx

		/* TranslateTLBAddressForLoad */
		and eax, 0x1fffffff
		or eax, 0x80000000

		/* memory_read_functions */
		mov edx, eax
		shr edx, SHIFTER2_READ
		call dword ptr gHardwareState.memory_read_functions[edx * 4]
		pop edx
		ret 0
	}
}

/* This function is used when currentromoptions.UseTLB = Yes */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__declspec(naked)
uint32 *read_mem_tlb_eax_only(void)
{
	__asm
	{
		push edx
		push ecx
		mov ecx, eax
		call TranslateTLBAddressForLoad
		mov edx, eax
		shr edx, SHIFTER2_READ
		call dword ptr gHardwareState.memory_read_functions[edx * 4]
		mov ecx, [esp]
		mov edx, [esp+4]
		add esp, 8
		ret 0
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_unmapped(uint32 addr)
{
	return(uint32 *) (gMemoryState.dummyReadWrite);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_unmapped_eax_only(void)
{
	/*
	 * SAFETY_READ_MEM_(read_mem_unmapped) //what's wrong with this line in DEBUG mode
	 * !?
	 */
	return(uint32 *) (gMemoryState.dummyReadWrite);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *read_mem_others(uint32 addr)
{
	__asm push edx
	addr &= 0x1FFFFFFF;
    
    if(addr >= MEMORY_START_PIF && addr < MEMORY_START_PIF + MEMORY_SIZE_PIF)
	{
		addr = (uint32) gMS_PIF + (addr & (MEMORY_SIZE_PIF - 1));
	}
	else if(addr >= MEMORY_START_GIO && addr < MEMORY_START_GIO + MEMORY_SIZE_GIO_REG)
	{
		addr = (uint32) gMS_GIO_REG + (addr & 0xFFF);
	}
	else if(addr >= MEMORY_START_RAMREGS0 && addr < MEMORY_START_RAMREGS0 + MEMORY_SIZE_RAMREGS0)
	{
		addr = (uint32) gMS_ramRegs0 + (addr & 0x3F);
	}
	else if(addr >= MEMORY_START_RAMREGS8 && addr < MEMORY_START_RAMREGS8 + MEMORY_SIZE_RAMREGS8)
	{
		addr = (uint32) gMS_ramRegs8 + (addr & 0x3F);
	}
	else
	{
		addr = (uint32) (gMemoryState.dummyReadWrite);
	}

	__asm pop edx;
	return (uint32 *) addr;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
__declspec(naked)
uint32 *read_mem_others_eax_only(void)
{
	__asm
	{
		push ecx
		mov ecx, eax
		call read_mem_others
		pop ecx
		ret
	}
}

/*
 * The memory write functions return the pointer to memory, not the actual value ? * of the memory location(only write actual value in function write_mem_io()), the
 * calling ? * function is responsible for writing the actual value to the real memory
 * location. ? * The reason I do this is because we don't know what memory access type is, in
 * BYTE, or WORD, DWORD .. ? * I probably need to come back here, try to write these functions in ASM code ? * but I still need to parameter val. in most functions, I don't care the real
 * value of this parameter ? * because I am not going to write the value into memory location, I do care about
 * the real value only ? * in function write_mem_io(), so in LW/SW opcode, you need to pass me the real
 * value so I can write the ? * real value into the IO register and I can trigger correct hardware functions ? * Remember, in SW/LW opcode, you still need to write the real value to the memory
 * locate ? * because the SW/LW may be accessing non io registers (at most time), I will not
 * write real value ? * for non-io-registers, any problem here? maybe, because the value maybe not the
 * value that actually ? * write into the io register, like the MI control registers. How to deal with
 * this? well, I will return ? * a dummy memory pointer to you, so SW opcode will write value into a dummy
 * memory location, and I have ? * already write the correct value before return from write_mem_io function. ? * You are not passing me the real value, but pass the GPR number rt ? * This is revised, we are not passing rt in function any more, but you should
 * store ? * the rt value in parameter r.r_.write_mem_rt;
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline uint32 *write_mem_rdram_k0seg(uint32 addr)
{
    __asm lea eax, [ecx + 0xA0000000]
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline uint32 *write_mem_rdram_k1seg(uint32 addr)
{
	__asm lea eax, [ecx + 0x80000000]	/* rdram is at 0x20000000 */
}

uint32	tempaddr;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *write_mem_rdram_not_at_0x20000000(uint32 addr)
{

#ifdef _DEBUG
	__asm push edx
	tempaddr = (uint32) (gMS_RDRAM + (addr & 0x007FFFFF));
	__asm pop edx
	return (uint32 *) tempaddr;
#else
	return(uint32 *) (gMS_RDRAM + (addr & 0x007FFFFF));
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__forceinline uint32 *write_mem_cart(uint32 addr)
{

#ifdef _DEBUG
	__asm push edx
    __asm push ecx
	TRACE2("Write to ROM, addr = %08X, PC=%08X", addr, r.r_.pc);
	__asm pop ecx
    __asm pop edx
#endif

	if( NeedToApplyRomWriteHack )	//Do we need to apply ?
	{
		write_to_rom_flag = TRUE;
		return  (uint32*)(&write_to_rom_value[0]);
	}
	else
	{
		return(uint32 *) (gMS_ROM_Image + (addr & 0x0FFFFFFF));
	}
}

__forceinline uint32 *write_mem_cart_eax_only()
{

    uint32 addr;

    __asm mov addr, eax

#ifdef _DEBUG
	__asm push edx
    __asm push ecx
	TRACE2("Write to ROM, addr = %08X, PC=%08X", addr, r.r_.pc);
	__asm pop ecx
    __asm pop edx
#endif

	if( NeedToApplyRomWriteHack )	//Do we need to apply ?
	{
		write_to_rom_flag = TRUE;
		return  (uint32*)(&write_to_rom_value[0]);
	}
	else
	{
		return(uint32 *) (gMS_ROM_Image + (addr & 0x0FFFFFFF));
	}
}

//TODO: Optimize. Don't access Rom Image by pointer in this function.
__forceinline uint32 *write_mem_cart_eax_only__Opcode_Debugger_is_off()
{
    uint32 addr;

    __asm mov addr, eax

#ifdef _DEBUG
	__asm push edx
    __asm push ecx
	TRACE2("Write to ROM, addr = %08X, PC=%08X", addr, r.r_.pc);
	__asm pop ecx
    __asm pop edx
#endif

	if( NeedToApplyRomWriteHack )	//Do we need to apply ?
	{
		write_to_rom_flag = TRUE;
		return  (uint32*)(&write_to_rom_value[0]);
	}
	else
	{
		return(uint32 *) (gMS_ROM_Image + (addr & 0x0FFFFFFF));
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *write_mem_io(uint32 addr)
{
	__asm push edx

	/* I will do this later */
	Check_SW(addr, r.r_.write_mem_rt);

	/* Check_SW(addr, rt); */
	__asm pop edx
	return(uint32 *) (gMemoryState.dummyReadWrite);
}
uint32 *write_mem_io_eax_only()
{
	uint32 addr;

    __asm mov addr, eax
    __asm push ecx
    __asm push edx

	/* I will do this later */
	Check_SW(addr, r.r_.write_mem_rt);

	/* Check_SW(addr, rt); */
	__asm pop edx
    __asm pop ecx
	return(uint32 *) (gMemoryState.dummyReadWrite);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *write_mem_flashram(uint32 addr)
{
	__asm push edx
	Check_SW(addr, r.r_.write_mem_rt);
	__asm pop edx
	return (uint32 *) gMS_dummyReadWrite;	/* gMS_C2A2+(addr&0x0001FFFFF); */
}

uint32 *write_mem_flashram_eax_only()
{
	uint32 addr;

    __asm mov addr, eax
    __asm push ecx
    __asm push edx
	Check_SW(addr, r.r_.write_mem_rt);
	__asm pop edx
    __asm pop ecx
	return (uint32 *) gMS_dummyReadWrite;	/* gMS_C2A2+(addr&0x0001FFFFF); */
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *write_mem_sram(uint32 addr)
{
	__asm push edx

	addr &= 0x00007FFF;
	__asm pop edx
	return(uint32 *) (gamesave.SRam + addr);
}
uint32 *write_mem_sram_eax_only(void)
{
	uint32 addr;

    __asm push edx
    __asm mov addr, eax

	addr &= 0x00007FFF;
	__asm pop edx
	return(uint32 *) (gamesave.SRam + addr);
}


uint32 *write_mem_tlb_eax_only()
{
	uint32 addr;
    
    __asm push edx
    __asm push ecx
    __asm mov addr, eax
	addr = TranslateTLBAddressForStore(addr);
    __asm mov eax, addr
	//addr = (uint32) ((*gHardwareState.memory_write_fun_eax_only[addr >> SHIFTER2_WRITE]) ());
	__asm mov ecx, addr
	__asm  shr         ecx,0Ch 
	__asm  call        dword ptr gHardwareState.memory_write_fun_eax_only[ecx*4]
	__asm  mov         addr,eax 
	__asm pop ecx
    __asm pop edx
	return (uint32 *) addr;
}


uint32 *write_mem_unmapped_eax_only()
{

    return(uint32 *) (gMemoryState.dummyReadWrite);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
uint32 *write_mem_others(uint32 addr)
{
	/*
	 * I those memory regions are seldomly accessed, so I don't care the memory access
	 * speed ?	 * very much to these memory regions
	 */
	return read_mem_others(addr);
}

__forceinline __declspec (naked)
uint32 *write_mem_others_eax_only()
{
    __asm
	{
		push ecx
		mov ecx, eax
		call read_mem_others
		pop ecx
		ret
	}
}


/*
 =======================================================================================================================
    in this function, edx should be saved without change.
 =======================================================================================================================
 */
uint32 *write_mem_protected(uint32 addr)
{
	/*
	 * write into a protected memory location, need to invalidate the 4KB block ?	 * I those memory regions are seldomly accessed, so I don't care the memory access
	 * speed ?	 * very much to these memory regions ?	 * addr must be within RDRAM range
	 */
	__asm push edx
	addr = addr & 0xDFFFFFFF;

	if(addr < 0x80000000 + current_rdram_size)
	{
		if(*(uint32 *) (RDRAM_Copy + addr - 0x80000000) != DUMMYOPCODE)
		{
			*(uint32 *) (RDRAM_Copy + addr - 0x80000000) = DUMMYOPCODE;
			InvalidateOneBlock(addr);
			unprotect_memory_set_func_array(addr);
			CODE_DETECT_TRACE(TRACE1("Protect Memory found self-mod code at %08X, invalidate the block", addr));
		}
	    addr = (uint32) (gMS_RDRAM + (addr & 0x007FFFFF));
    }
    else
        addr = (uint32)gMemoryState.dummyReadWrite;



	__asm pop edx
	return (uint32 *) addr;
}

uint32 *write_mem_protected_eax_only()
{
    uint32 addr;

    /*
	 * write into a protected memory location, need to invalidate the 4KB block ?	 * I those memory regions are seldomly accessed, so I don't care the memory access
	 * speed ?	 * very much to these memory regions ?	 * addr must be within RDRAM range
	 */
	__asm push edx
    __asm push ecx
    __asm mov addr, eax
	addr = addr & 0xDFFFFFFF;

	if(addr < 0x80000000 + current_rdram_size)
	{
		if(*(uint32 *) (RDRAM_Copy + addr - 0x80000000) != DUMMYOPCODE)
		{
			*(uint32 *) (RDRAM_Copy + addr - 0x80000000) = DUMMYOPCODE;
			InvalidateOneBlock(addr);
			unprotect_memory_set_func_array(addr);
			CODE_DETECT_TRACE(TRACE1("Protect Memory found self-mod code at %08X, invalidate the block", addr));
		}
    	addr = (uint32) (gMS_RDRAM + (addr & 0x007FFFFF));
	}

     else
        addr = (uint32)gMemoryState.dummyReadWrite;


	__asm pop ecx
    __asm pop edx
	return (uint32 *) addr;
}



/*
 =======================================================================================================================
 Write 4KB block which has cheat code enabled.
 - will not write active cheat code bytes
 - Cheat code memory address must be within RDRAM 4MB/8MB range
 =======================================================================================================================
 */
#ifdef CHEATCODE_LOCK_MEMORY

uint32 *read_mem_cheatcode_block(uint32 addr)
{
	uint32 block;
	uint32* retval;
	__asm push edx;
	block = (addr&0x1FFFFFFF)/0x1000;
	if( block >= current_rdram_size/0x1000 || cheatCodeBlockMap[block] == NULL || (uint8)(cheatCodeBlockMap[block][addr&0xFFF]) == 0 )
	{
		//This block or this byte is not locked/protected by cheatcodes
		__asm pop edx;
		__asm mov eax, addr;
		__asm call read_mem_rdram_not_at_0x20000000_eax_only;
		__asm mov retval, eax;
	}
	else
	{
#ifdef _DEBUG
		__asm pushad;
		if( cheatCodeBlockMap[block][addr&0xFFF] == BYTE_AFFECTED_BY_CHEAT_CODES )
		{
			TRACE1("Byte is affected by Cheat code protect memory write at: %08X", addr);
		}
		else
		{
			TRACE1("Cheat code protect memory write at: %08X", addr);
		}
		__asm popad;
#endif
		//Reapply the cheat code
		//CodeList_ApplyCode_At_Address(cheatCodeBlockMap[block][addr&0xFFF], addr);		
		__asm mov eax, addr;
		__asm call read_mem_rdram_not_at_0x20000000_eax_only;
		__asm mov retval, eax;
		{
			int i;
			uint8 val;
			for( i=0; i<4; i++)
			{
				if( (uint8)(cheatCodeBlockMap[block][(addr&0xFFC)+(i^0x3)]) != BYTE_AFFECTED_BY_CHEAT_CODES )
				{
					val = (uint8)(cheatCodeBlockMap[block][(addr&0xFFC)+(i^0x3)]>>8);
					*(uint8*)(((uint32)retval&0xFFFFFFFC)+(i^0x3)) = val;
				}
			}
		}

		__asm pop edx;
		//return dummyWord;	//return dummy word, or any write operation will be written into this dummy word
	}
	return (uint32*)retval;
}

uint32 *read_mem_cheatcode_block_eax_only_rdram_not_at_20000000()
{
	/*
	__asm push ecx;
	__asm mov ecx, eax;
	__asm call read_mem_cheatcode_block;
	__asm pop ecx;
	*/
	__asm {
		push	eax;
		and		eax, 0x1FFFF000;
		shr		eax, 10;
		mov		eax, cheatCodeBlockMap[eax];
		cmp		eax, 0;
		jnz		l1;
		pop		eax;
		call	read_mem_rdram_not_at_0x20000000_eax_only
		ret		0
l1:
		pop		eax;
		push	ecx;
		mov		ecx, eax;
		call	read_mem_cheatcode_block;
		pop		ecx;
		ret		0
	}

}

__forceinline static __declspec (naked)
uint32 *read_mem_cheatcode_block_k0seg_eax_only_rdram_at_0x20000000()
{
	__asm {
		push	eax;
		and		eax, 0x1FFFF000;
		shr		eax, 10;
		mov		eax, cheatCodeBlockMap[eax];
		cmp		eax, 0;
		jnz		l1;
		pop		eax;
		add		eax, 0xA0000000 /* rdram is at 0x20000000 */
		ret		0
l1:
		pop		eax;
		push	ecx;
		mov		ecx, eax;
		call	read_mem_cheatcode_block;
		pop		ecx;
		ret		0
	}
}

__forceinline static __declspec (naked)
uint32 *read_mem_cheatcode_block_k1seg_eax_only_rdram_at_0x20000000()
{
	__asm {
		push	eax;
		and		eax, 0x1FFFF000;
		shr		eax, 10;
		mov		eax, cheatCodeBlockMap[eax];
		cmp		eax, 0;
		jnz		l1;
		pop		eax;
		add		eax, 0x80000000 /* rdram is at 0x20000000 */
		ret		0
l1:
		pop		eax;
		push	ecx;
		mov		ecx, eax;
		call	read_mem_cheatcode_block;
		pop		ecx;
		ret		0
	}
}



void enable_cheat_code_lock_block(uint32 addr)
{
	uint32 block = (addr&0x1FFFFFFF)/0x1000;
	if( block >= current_rdram_size/0x1000 )
	{
		return;
	}
	else
	{
		TRACE1("Enable cheat code protection at block=%08X", block);
		if( rdram_is_at_0x20000000 )
		{
			(uint32) gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (uint32) read_mem_cheatcode_block_k0seg_eax_only_rdram_at_0x20000000;
			(uint32) gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (uint32) read_mem_cheatcode_block_k1seg_eax_only_rdram_at_0x20000000;
		}
		else
		{
			(uint32) gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (uint32) read_mem_cheatcode_block_eax_only_rdram_not_at_20000000;
			(uint32) gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (uint32) read_mem_cheatcode_block_eax_only_rdram_not_at_20000000;
		}
	}
}

void init_rdram_region_func_array(uint32 startAddress, uint32 size);

void disable_cheat_code_lock_block(uint32 addr)
{
	uint32 block = (addr&0x1FFFFFFF)/0x1000;
	if( block >= current_rdram_size/0x1000 )
	{
		return;
	}
	else
	{
		TRACE1("Disable cheat code protection at block=%08X", block);
		init_rdram_region_func_array(addr&0x007FC0000, 0x40000);
	}
}

#endif

/*
 =======================================================================================================================
    Function array initializations
 =======================================================================================================================
 */
void init_mem_region_func_array_eax_only(uint32 startAddress, uint32 size, uint32 readfunc, uint32 writefunceaxonly)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	uint32	curSegment = ((startAddress) >> SHIFTER2_READ);
	uint32	endSegment = ((startAddress + size - 1) >> SHIFTER2_READ);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(curSegment <= endSegment)
	{
		gHardwareState.memory_read_functions[curSegment | (0x8000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)())readfunc;
		gHardwareState.memory_read_functions[curSegment | (0xA000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)())readfunc;
		curSegment++;
	}

	curSegment = ((startAddress) >> SHIFTER2_WRITE);
	endSegment = ((startAddress + size - 1) >> SHIFTER2_WRITE);
	while(curSegment <= endSegment)
	{
		gHardwareState.memory_write_fun_eax_only[curSegment | (0x80000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)())writefunceaxonly;
		gHardwareState.memory_write_fun_eax_only[curSegment | (0xA0000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)())writefunceaxonly;
		curSegment++;
	}
}

/*
 =======================================================================================================================
    Function array initializations
 =======================================================================================================================
 */
void init_rdram_region_func_array(uint32 startAddress, uint32 size)
{
	if(rdram_is_at_0x20000000)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		uint32	curSegment = ((startAddress) >> SHIFTER2_READ);
		uint32	endSegment = ((startAddress + size - 1) >> SHIFTER2_READ);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        while(curSegment <= endSegment)
		{
			gHardwareState.memory_read_functions[curSegment | (0x8000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)()) read_mem_rdram_k0seg_eax_only;
			gHardwareState.memory_read_functions[curSegment | (0xA000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)()) read_mem_rdram_k1seg_eax_only;
			curSegment++;
		}

		curSegment = ((startAddress) >> SHIFTER2_WRITE);
		endSegment = ((startAddress + 0x400000 - 1) >> SHIFTER2_WRITE);
		while(curSegment <= endSegment)
		{
			gHardwareState.memory_write_fun_eax_only[curSegment | (0x80000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)()) write_mem_rdram_k0seg_eax_only;
			gHardwareState.memory_write_fun_eax_only[curSegment | (0xA0000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)()) write_mem_rdram_k1seg_eax_only;
			curSegment++;
		}
    }
	else
	{
		uint32	curSegment = ((startAddress) >> SHIFTER2_READ);
		uint32	endSegment = ((startAddress + 0x400000 - 1) >> SHIFTER2_READ);

		if(debug_opcode!=0)
		{
			while(curSegment <= endSegment)
			{
				gHardwareState.memory_read_functions[curSegment | (0x8000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)()) read_mem_rdram_not_at_0x20000000_eax_only;
				gHardwareState.memory_read_functions[curSegment | (0xA000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)()) read_mem_rdram_not_at_0x20000000_eax_only;
				curSegment++;
			}

    		curSegment = ((startAddress) >> SHIFTER2_WRITE);
	    	endSegment = ((startAddress + 0x400000 - 1) >> SHIFTER2_WRITE);
		    while(curSegment <= endSegment)
    		{
    			gHardwareState.memory_write_fun_eax_only[curSegment | (0x80000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)()) write_mem_rdram_not_at_0x20000000_eax_only;
    			gHardwareState.memory_write_fun_eax_only[curSegment | (0xA0000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)()) write_mem_rdram_not_at_0x20000000_eax_only;
    			curSegment++;
    		}

		}
		else
		{
			while(curSegment <= endSegment)
			{
                gHardwareState.memory_read_functions[curSegment | (0x8000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)()) read_mem_rdram_not_at_0x20000000_eax_only__Opcode_Debugger_is_off;
				gHardwareState.memory_read_functions[curSegment | (0xA000 >> SHIFTER1_READ)] = (unsigned int *(__fastcall *)()) read_mem_rdram_not_at_0x20000000_eax_only__Opcode_Debugger_is_off;
				curSegment++;
			}


    		curSegment = ((startAddress) >> SHIFTER2_WRITE);
	    	endSegment = ((startAddress + 0x400000 - 1) >> SHIFTER2_WRITE);
		    while(curSegment <= endSegment)
    		{
    			gHardwareState.memory_write_fun_eax_only[curSegment | (0x80000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)()) write_mem_rdram_not_at_0x20000000_eax_only__Opcode_Debugger_is_off;
    			gHardwareState.memory_write_fun_eax_only[curSegment | (0xA0000 >> SHIFTER1_WRITE)] = (unsigned int *(__fastcall *)()) write_mem_rdram_not_at_0x20000000_eax_only__Opcode_Debugger_is_off;
    			curSegment++;
    		}
        }
    }
}




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void init_whole_mem_func_array(void)
{
	int i;

	for(i = 0; i < (0x8000 >> SHIFTER1_READ); i++)
	{
		if(currentromoptions.Use_TLB != USETLB_YES)
			gHardwareState.memory_read_functions[i] = (unsigned int *(__fastcall *)()) read_mem_tlb_eax_only__UseTLB_No;
		else
			gHardwareState.memory_read_functions[i] = (unsigned int *(__fastcall *)()) read_mem_tlb_eax_only;
	}

	for(i = (0x8000 >> SHIFTER1_READ); i < (0xC000 >> SHIFTER1_READ); i++)
		(unsigned int *(__fastcall *)()) gHardwareState.memory_read_functions[i] = (unsigned int *(__fastcall *)()) read_mem_unmapped_eax_only;

	for(i = (0xC000 >> SHIFTER1_READ); i < (0x10000 >> SHIFTER1_READ); i++)
	{
		if(currentromoptions.Use_TLB != USETLB_YES)
			gHardwareState.memory_read_functions[i] = (unsigned int *(__fastcall *)()) read_mem_tlb_eax_only__UseTLB_No;
		else
			gHardwareState.memory_read_functions[i] = (unsigned int *(__fastcall *)()) read_mem_tlb_eax_only;
	}

	for(i = 0; i < (0x80000 >> SHIFTER1_WRITE); i++)
    {
        gHardwareState.memory_write_fun_eax_only[i] = (unsigned int *(__fastcall *)()) write_mem_tlb_eax_only;
    }

    for(i = (0x80000 >> SHIFTER1_WRITE); i < (0xC0000 >> SHIFTER1_WRITE); i++)
    {
        gHardwareState.memory_write_fun_eax_only[i] = (unsigned int *(__fastcall *)()) write_mem_unmapped_eax_only;
    }
    
    for(i = (0xC0000 >> SHIFTER1_WRITE); i < (0x100000 >> SHIFTER1_WRITE); i++)
    {
    	gHardwareState.memory_write_fun_eax_only[i] = (unsigned int *(__fastcall *)()) write_mem_tlb_eax_only;
    }

	init_rdram_region_func_array(MEMORY_START_RDRAM, 0x400000);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_RAMREGS0,
		MEMORY_SIZE_RAMREGS0,
		(uint32) read_mem_others_eax_only,
        (uint32) write_mem_others_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_RAMREGS8,
		MEMORY_SIZE_RAMREGS8,
		(uint32) read_mem_others_eax_only,
        (uint32) write_mem_others_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_SPMEM,
		MEMORY_SIZE_SPMEM,
		(uint32) read_mem_sp_eax_only,
        (uint32) write_mem_sp_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_SPREG_1,
		MEMORY_SIZE_SPREG_1,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_SPREG_2,
		MEMORY_SIZE_SPREG_2,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_DPC,
		MEMORY_SIZE_DPC,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_DPS,
		MEMORY_SIZE_DPS,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_MI,
		MEMORY_SIZE_MI,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_VI,
		MEMORY_SIZE_VI,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_AI,
		MEMORY_SIZE_AI,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_PI,
		MEMORY_SIZE_PI,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_RI,
		MEMORY_SIZE_RI,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_SI,
		MEMORY_SIZE_SI,
		(uint32) read_mem_io_eax_only,
        (uint32) write_mem_io_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_C2A1,
		MEMORY_SIZE_C2A1,
		(uint32) read_mem_sram_eax_only,
        (uint32) write_mem_sram_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_C1A1,
		MEMORY_SIZE_C1A1,
		(uint32) read_mem_sram_eax_only,
        (uint32) write_mem_sram_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_GIO,
		MEMORY_SIZE_GIO_REG,
		(uint32) read_mem_others_eax_only,
        (uint32) write_mem_others_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_PIF,
		MEMORY_SIZE_PIF,
		(uint32) read_mem_others_eax_only,
        (uint32) write_mem_others_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_C1A3,
		MEMORY_SIZE_C1A3,
		(uint32) read_mem_sram_eax_only,
        (uint32) write_mem_sram_eax_only
	);
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_C2A2,
		MEMORY_SIZE_C2A2,
		(uint32) read_mem_flashram_eax_only,
        (uint32) write_mem_flashram_eax_only
	);

#ifndef _USE_ROM_PAGING // works ok when commented. commented by oDD -Paging?
	if(debug_opcode!=0)
	{
		init_mem_region_func_array_eax_only
		(
			MEMORY_START_ROM_IMAGE,
			gAllocationLength,
			(uint32) read_mem_cart_eax_only,
            (uint32) write_mem_cart_eax_only
		);
	}
	else

		init_mem_region_func_array_eax_only
		(
			MEMORY_START_ROM_IMAGE,
			gAllocationLength,
			(uint32) read_mem_cart_eax_only__Opcode_Debugger_is_off,
            (uint32) write_mem_cart_eax_only__Opcode_Debugger_is_off
		);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void enable_exrdram_func_array(void)
{
	init_rdram_region_func_array(MEMORY_START_EXRDRAM, 0x400000);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void disable_exrdram_func_array(void)
{
	init_mem_region_func_array_eax_only
	(
		MEMORY_START_EXRDRAM,
		MEMORY_SIZE_EXRDRAM,
		(uint32) read_mem_unmapped_eax_only,
        (uint32) write_mem_unmapped_eax_only
	);
}

/*
 =======================================================================================================================
    input parameter pc must be in valid RDRAM memory region
 =======================================================================================================================
 */
void protect_memory_set_func_array(uint32 pc)
{
    gHardwareState.memory_write_fun_eax_only[(pc / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_protected_eax_only;
	gHardwareState.memory_write_fun_eax_only[((pc | 0x20000000) / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_protected_eax_only;
}

/*
 =======================================================================================================================
    input parameter pc must be in valid RDRAM memory region
 =======================================================================================================================
 */
void unprotect_memory_set_func_array(uint32 pc)
{
	if(rdram_is_at_0x20000000)
	{
        gHardwareState.memory_write_fun_eax_only[(pc / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_rdram_k0seg_eax_only;
		gHardwareState.memory_write_fun_eax_only[((pc | 0x20000000) / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_rdram_k1seg_eax_only;
    }
	else
	{
        gHardwareState.memory_write_fun_eax_only[(pc / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_rdram_not_at_0x20000000_eax_only;
		gHardwareState.memory_write_fun_eax_only[((pc | 0x20000000) / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_rdram_not_at_0x20000000_eax_only;
    
    }
}


void protect_framebuffer_memory_set_func_array(uint32 pc)
{
	uint32 addr = pc | 0x80000000;

	gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_framebuffer_eax_only;
	gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_framebuffer_eax_only;
	
	if(rdram_is_at_0x20000000)
	{
		if( (uint32) gHardwareState.memory_write_fun_eax_only[(addr / 0x1000) >> SHIFTER1_WRITE] == (uint32) write_mem_rdram_k0seg_eax_only )
		{
            // Only protect the frame buffer memory if the 4KB block is not protected by other purpose
			gHardwareState.memory_write_fun_eax_only[(addr / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_framebuffer_eax_only;
			gHardwareState.memory_write_fun_eax_only[((addr | 0x20000000) / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_framebuffer_eax_only;
		}
	}
	else
	{
		//hmm..do we need to check write_mem_rdram_eax_only_Opcode_Debugger_is_off????
        if( (uint32) gHardwareState.memory_write_fun_eax_only[(addr / 0x1000) >> SHIFTER1_WRITE] == (uint32) write_mem_rdram_not_at_0x20000000_eax_only )
		{
            gHardwareState.memory_write_fun_eax_only[(addr / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_framebuffer_eax_only;
			gHardwareState.memory_write_fun_eax_only[((addr | 0x20000000) / 0x1000) >> SHIFTER1_WRITE] = (unsigned int *(__fastcall *)()) write_mem_framebuffer_eax_only;
        }
	}
}

void unprotect_framebuffer_memory_set_func_array(uint32 pc)
{
	uint32 addr = pc | 0x80000000;

	unprotect_memory_set_func_array(addr);

	if(rdram_is_at_0x20000000)
	{
		gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_k0seg_eax_only;
		gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_k1seg_eax_only;
	}
	else
	{
		gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_not_at_0x20000000_eax_only;
		gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_not_at_0x20000000_eax_only;
	}
}

void unprotect_framebuffer_memory_from_READ_set_func_array(uint32 pc)
{
	uint32 addr = pc | 0x80000000;

	if(rdram_is_at_0x20000000)
	{
		gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_k0seg_eax_only;
		gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_k1seg_eax_only;
	}
	else
	{
		gHardwareState.memory_read_functions[( addr				/ 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_k0seg_eax_only;
		gHardwareState.memory_read_functions[((addr | 0x20000000) / 0x10000) >> SHIFTER1_READ] = (unsigned int *(__fastcall *)()) read_mem_rdram_k1seg_eax_only;
	}
}


