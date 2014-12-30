__forceinline void lw(OP_PARAMS)
{
	_s32	value;

	compilerstatus.cp0Counter += 1;
	_SAFTY_CPU_(r4300i_lw) SetRdRsRt32bit(PASS_PARAMS);

#ifdef SAFE_LOADSTORE
	goto _Default;
#endif


	if(ConstMap[xRS->mips_reg].IsMapped == 1)
	{
		uint32	QuerAddr;

		QuerAddr = (_u32) ((_s32) ConstMap[xRS->mips_reg].value + (_s32) (_s16) __I);

		if(NOT_IN_KO_K1_SEG(QuerAddr))
        {        
            goto _Default;
        }

		__try
		{
			value = LOAD_SWORD_PARAM(QuerAddr);
		}
		
		__except(NULL, EXCEPTION_EXECUTE_HANDLER)
		{
			// TRACE1("DYNA LW: Addr=%08X", QuerAddr);
			goto _Default;
		}

		if(xRT->mips_reg != 0) // mandatory
		{
			ConstMap[xRT->mips_reg].IsMapped = 0;
			xRT->IsDirty = 1;
			xRT->NoNeedToLoadTheLo = 1;
			MapRegister(xRT, 99, 99);

			// added by oDD
			if (CheckIfInRom(QuerAddr))
			{
				ADD_ImmToMemory((_u32)&g_dynaHits, 1);
				
				PUSHAD();

				MOV_ImmToReg(1, Reg_ECX, QuerAddr);
				X86_CALL((_u32) ReadSWORDFromROM);

				MOV_RegToMemory(1, Reg_EAX, ModRM_disp32, (_u32)&g_tempValue);

				POPAD();

				MOV_MemoryToReg(1, xRT->x86reg, ModRM_disp32, (_u32)&g_tempValue);
			}
			else
			{
				MOV_MemoryToReg(1, xRT->x86reg, ModRM_disp32, (_u32) pLOAD_SWORD_PARAM(QuerAddr));
			}
		}
	} 
	else 
	{
_Default:

        if(ConstMap[xRS->mips_reg].IsMapped)
		{
			int		temp = ConstMap[xRS->mips_reg].value;
			uint32	vAddr = (uint32) ((_int32) temp + (_s32) (_s16) __dotI);

			xRT->IsDirty = 1;
			xRT->NoNeedToLoadTheLo = 1;

			MapRT;

			// added by oDD
			if (CheckIfInRom(vAddr))
			{
				ADD_ImmToMemory((_u32)&g_dynaHits, 1);
				
				PUSHAD();

				MOV_ImmToReg(1, Reg_ECX, vAddr);

				X86_CALL((_u32) ReadSWORDFromROM);

				MOV_RegToMemory(1, Reg_EAX, ModRM_disp32, (_u32)&g_tempValue);

				POPAD();

				MOV_MemoryToReg(1, xRT->x86reg, ModRM_disp32, (_u32)&g_tempValue);
			}
			else
			{
				if(xRT->x86reg != Reg_EAX) 
					PUSH_RegIfMapped(Reg_EAX);

				MOV_ImmToReg(1, Reg_EAX, vAddr);
				X86_CALL((uint32) memory_read_functions[vAddr >> SHIFTER2_READ]);

				//LOGGING_DYNA(LogDyna("	CALL memory_read_functions[]\n"););

				if(__RT != 0) 
					MOV_ModRMToReg(1, xRT->x86reg, ModRM_EAX);

				if(xRT->x86reg != Reg_EAX)
					POP_RegIfMapped(Reg_EAX);
			}

		} 
		else 
		{
            xRT->IsDirty = 1;

			if(xRT->mips_reg != xRS->mips_reg)
                xRT->NoNeedToLoadTheLo = 1;

           
			MapRT;

			// added by oDD
			if(xRT->x86reg != Reg_EAX)
			{
				/* we better have checked if rs is mapped above. */
				MapRS_To(xRT, 1, MOV_MemoryToReg);

				PUSH_RegIfMapped(Reg_EAX);
				ADD_ImmToReg(1, xRT->x86reg, (_s32) (_s16) __dotI);
				MOV_Reg2ToReg1(1, Reg_EAX, xRT->x86reg);

				{
					PUSHAD();

					MOV_Reg2ToReg1(1, Reg_ECX, Reg_EAX);
					X86_CALL((_u32) CheckIfInRom);

					CMP_RegWithImm(1, Reg_EAX, TRUE);
					Jcc_auto(CC_NE, 1);

					{
						POPAD();
						PUSHAD();

						MOV_Reg2ToReg1(1, Reg_ECX, Reg_EAX);

						X86_CALL((_u32) ReadSWORDFromROM);

						MOV_RegToMemory(1, Reg_EAX, ModRM_disp32, (_u32)&g_tempValue);

						POPAD();

						ADD_ImmToMemory((_u32)&g_dynaHits, 1);

						MOV_MemoryToReg(1, xRT->x86reg, ModRM_disp32, (_u32)&g_tempValue);

						JMP_Short_auto(2);
					}

					SetTarget(1);
					POPAD();
				}

				SHR_RegByImm(1, xRT->x86reg, SHIFTER2_READ);

				WC16(0x14FF);
				WC8((uint8) (0x85 | (xRT->x86reg << 3)));
				WC32((uint32) & memory_read_functions);

				if(__RT != 0)
					MOV_ModRMToReg(1, xRT->x86reg, ModRM_EAX);

				SetTarget(2);

				//if(xRT->x86reg != Reg_EAX)
					POP_RegIfMapped(Reg_EAX);
				/*else if(xRS->x86reg != xRT->x86reg)
					POP_RegFromStack(xRS->x86reg);*/
			} 
			else 
			{
                // we checked if rs is a const above already. good.
				if( (CheckWhereIsMipsReg(xRS->mips_reg) == -1)
				&&	(ConstMap[xRS->mips_reg].FinalAddressUsedAt <= gHWS_pc)) 
				{
					FetchEBP_Params(xRS->mips_reg);

					if(xRT->x86reg != Reg_EAX)
						PUSH_RegIfMapped(Reg_EAX);

                    
					if(xRT->x86reg != Reg_ECX)
						PUSH_RegIfMapped(Reg_ECX);

					MOV_MemoryToReg(1, Reg_ECX, x86params.ModRM, x86params.Address);
					
				} 
				else 
				{
					MapRS;

					if(xRT->x86reg != Reg_EAX)
						PUSH_RegIfMapped(Reg_EAX);

                    
					if(xRT->x86reg != Reg_ECX)
						PUSH_RegIfMapped(Reg_ECX);

					MOV_Reg2ToReg1(1, Reg_ECX, xRS->x86reg);	/* mov rt, rs (lo) */
				}

				ADD_ImmToReg(1, Reg_ECX, (_s32) (_s16) __dotI);

				// added by oDD
				{
					PUSHAD();

					X86_CALL((_u32) CheckIfInRom);

					CMP_RegWithImm(1, Reg_EAX, TRUE);
					Jcc_auto(CC_NE, 1);

					{
						POPAD();
						PUSHAD();

						X86_CALL((_u32) ReadSWORDFromROM);

						MOV_RegToMemory(1, Reg_EAX, ModRM_disp32, (_u32)&g_tempValue);

						POPAD();

						ADD_ImmToMemory((_u32)&g_dynaHits, 1);

						MOV_MemoryToReg(1, xRT->x86reg, ModRM_disp32, (_u32)&g_tempValue);

						JMP_Short_auto(2);
					}

					SetTarget(1);
					POPAD();
				}

				MOV_Reg2ToReg1(1, Reg_EAX, Reg_ECX);

				SHR_RegByImm(1, Reg_ECX, SHIFTER2_READ);
				WC16(0x14FF);
				WC8(0x85 | (Reg_ECX << 3));
				WC32((uint32) & memory_read_functions);

				LOGGING_DYNA(LogDyna("	CALL memory_read_functions[]\n"););

				if(__RT != 0)
					MOV_ModRMToReg(1, xRT->x86reg, ModRM_EAX);

				SetTarget(2); //oDD

				if(xRT->x86reg != Reg_ECX)
					POP_RegIfMapped(Reg_ECX);

				if(xRT->x86reg != Reg_EAX)
					POP_RegIfMapped(Reg_EAX);
			}
		}
	}
}