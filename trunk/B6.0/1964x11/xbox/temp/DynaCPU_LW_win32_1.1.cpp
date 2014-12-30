__forceinline void lw(OP_PARAMS)
{
	_s32	value;

    OpcodePrologue(1, &r4300i_lw, 1, PASS_PARAMS);

#ifdef SAFE_LOADSTORE
    INTERPRET(r4300i_lw);
    return;
#endif


	if(ConstMap[xRS->mips_reg].IsMapped == 1)
	{
		uint32	QuerAddr;

		QuerAddr = (_u32) ((_s32) ConstMap[xRS->mips_reg].value + (_s32) (_s16) __I);

		if(!(IN_K0_SEG( QuerAddr)))
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
			MapRegisterNew(FLUSH_TO_MEMORY, xRT, xRT, 99, 99);

			MOV_MemoryToReg(xRT->x86reg, ModRM_disp32, (_u32) pLOAD_SWORD_PARAM(QuerAddr));
		}
	} 
	else 
	{
_Default:

        if(ConstMap[xRS->mips_reg].IsMapped)
		{
			int		temp = ConstMap[xRS->mips_reg].value;
			uint32	vAddr = (uint32) ((_int32) temp + (_s32) (_s16) __dotI);

            if (xRT->mips_reg != 0)
            {
                xRT->IsDirty = 1;
			    xRT->NoNeedToLoadTheLo = 1;
    			MapRT;
            }
            else
                xRT->x86reg = Reg_EDI; //a little trick. rt isn't used when rt is 0.

			if(xRT->x86reg != Reg_EAX)
				PUSH_RegIfMapped(Reg_EAX, 1, Reg_EAX);

			MOV_ImmToReg(Reg_EAX, vAddr);
			X86_CALL((uint32) gHardwareState.memory_read_functions[vAddr >> SHIFTER2_READ]);

			LOGGING_DYNA(LogDyna("	CALL memory_read_functions[]\n"););
			if(xRT->mips_reg != 0)
				POP_RegIfMapped(Reg_EAX);

			if(xRT->x86reg != Reg_EAX)
				POP_RegIfMapped(Reg_EAX);
		} 
		else 
		{
            {            
            xRT->IsDirty = 1;

			if(xRT->mips_reg != xRS->mips_reg)
                xRT->NoNeedToLoadTheLo = 1;

           
            MapRegisterNew(FLUSH_TO_MEMORY, xRT, xRT, xRS->mips_reg, -1);

			if(xRT->x86reg != Reg_EAX)
			{

				/* we'd better have checked if rs is mapped above. */
				MapRS_To__WithLoadOnlyFunction(xRT, 1, MOV_MemoryToReg);

		        PUSH_RegIfMapped(Reg_EAX, 1, Reg_EAX);
			    Lea(Reg_EAX, xRT->x86reg, (_s32) (_s16) __dotI);
				ADD_ImmToReg(xRT->x86reg, (_s32) (_s16) __dotI, 0);

				SHR_RegByImm(xRT->x86reg, SHIFTER2_READ);

				WC16(0x14FF);
				WC8((uint8) (0x85 | (xRT->x86reg << 3)));
				WC32((uint32) & gHardwareState.memory_read_functions);

				if(xRT->mips_reg != 0)
					MOV_ModRMToReg(xRT->x86reg, ModRM_EAX);
                else
                    XOR_Reg2ToReg1(xRT->x86reg, xRT->x86reg);

				POP_RegIfMapped( Reg_EAX);
			} 
			else 
			{
				int ExtraRegister;

                
                // we checked if rs is a const above already. good.
				if( (CheckWhereIsMipsReg(xRS->mips_reg) == -1)
				&&	(ConstMap[xRS->mips_reg].FinalAddressUsedAt <= gHWS_pc)) 
				{

					if( xRT->x86reg != Reg_EAX)
						PUSH_RegIfMapped(Reg_EAX, 1, Reg_EAX);

                    
					//feasible for loads only
					if ((xRT->x86reg != Reg_EAX) && ( xRT->mips_reg != 0)) 
						ExtraRegister = xRT->x86reg;
					else
					{
						ExtraRegister = FindFreeRegister();
	    		        if (ExtraRegister < Reg_ECX)
	                        ExtraRegister = Reg_EBP;
					}

					FetchEBP_Params( xRS->mips_reg);
					
                    MOV_MemoryToReg( ExtraRegister, x86params.ModRM, x86params.Address);
					
                    //eax is NEVER equal to ExtraRegister. No check necessary.
                    Lea( Reg_EAX, ExtraRegister, __dotI);
                    ADD_ImmToReg(ExtraRegister, __dotI, 0);
				} 
				else 
				{
					MapRS;

					if(xRT->x86reg != Reg_EAX)
						PUSH_RegIfMapped(Reg_EAX, 1, Reg_EAX);

                    
                    //feasible for loads only.
	                //To improve pairing for this op, addded check for (xRT->mips_reg) != (xRS->mips_reg)
	                if ((xRT->x86reg != Reg_EAX) && ( xRT->mips_reg != 0) && (xRT->mips_reg) != (xRS->mips_reg)) 
	                    ExtraRegister = xRT->x86reg;
	                else
	                {
	                    ExtraRegister = FindFreeRegister();
	    		        if (ExtraRegister < Reg_ECX)
	                    {
	            			if ((ConstMap[xRS->mips_reg].FinalAddressUsedAt <= gHWS_pc) && !x86reg[xRS->x86reg].IsDirty && (xRS->x86reg != Reg_EAX))
	                        {
	                            ExtraRegister = xRS->x86reg;
	                        	Lea( Reg_EAX, xRS->x86reg, (_s32) (_s16) __dotI);
	                            Lea( ExtraRegister, xRS->x86reg, (_s32) (_s16) __dotI);
	                            goto _next;

	                        }
	                        else
	                        ExtraRegister = Reg_EBP;
		                }

	                }

					
                    //From the check above for (xRT->mips_reg) != (xRS->mips_reg),
                    //ExtraRegister is NEVER equal to RS. This makes the order of the next 2 LEA's 
                    //arbitrary. The order is reversed for better pairing with the following SHR op.
                    Lea( ExtraRegister, xRS->x86reg, (_s32) (_s16) __dotI);
                	Lea( Reg_EAX, xRS->x86reg, (_s32) (_s16) __dotI);
				}
_next:

				SHR_RegByImm(ExtraRegister, SHIFTER2_READ);
				WC16(0x14FF);
				WC8(0x85 | (ExtraRegister << 3));
				WC32((uint32)&gHardwareState.memory_read_functions);

				LOGGING_DYNA(LogDyna("	CALL memory_read_functions[]\n"););

				if(__RT != 0)
					MOV_ModRMToReg(xRT->x86reg, ModRM_EAX);
                else
                    XOR_Reg2ToReg1(xRT->x86reg, xRT->x86reg);

				if(ExtraRegister == Reg_EBP)
					MOV_ImmToReg(Reg_EBP, HardwareStart);

				if(xRT->x86reg != Reg_EAX)
					POP_RegIfMapped(Reg_EAX);
			}
            }
		}
	}
}