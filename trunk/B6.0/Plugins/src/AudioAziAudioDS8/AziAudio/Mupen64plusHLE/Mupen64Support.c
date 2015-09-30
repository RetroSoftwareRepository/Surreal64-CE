#include <stdarg.h>
#include <stdio.h>

#include "common.h"

#include "hle_external.h"
#include "hle_internal.h"
#include "../../Audio.h"
#pragma warning(disable : 4100)
void HleWarnMessage(void* UNUSED(user_defined), const char *message, ...)
{
	va_list args;
	va_start(args, message);
	//DebugMessage(M64MSG_WARNING, message, args);
	va_end(args);
}


void HleVerboseMessage(void* UNUSED(user_defined), const char *message, ...)
{
	va_list args;
	va_start(args, message);
	//DebugMessage(M64MSG_VERBOSE, message, args);
	va_end(args);
}
#pragma warning(default : 4100)

static struct hle_t _hle;

void SetupMusyX()
{
	struct hle_t *hle = &_hle;

	hle->dram = AudioInfo.RDRAM;
	hle->dmem = AudioInfo.DMEM;
	hle->imem = AudioInfo.IMEM;
	/*hle->mi_intr = NULL;
	hle->sp_mem_addr = NULL;
	hle->sp_dram_addr = NULL;
	hle->sp_rd_length = NULL;
	hle->sp_wr_length = NULL;
	hle->sp_status = NULL;
	hle->sp_dma_full = NULL;
	hle->sp_dma_busy = NULL;
	hle->sp_pc = NULL;
	hle->sp_semaphore = NULL;
	hle->dpc_start = NULL;
	hle->dpc_end = NULL;
	hle->dpc_current = NULL;
	hle->dpc_status = NULL;
	hle->dpc_clock = NULL;
	hle->dpc_bufbusy = NULL;
	hle->dpc_pipebusy = NULL;
	hle->dpc_tmem = NULL;
	hle->user_defined = NULL;*/
}

void ProcessMusyX_v1()
{
	SetupMusyX();
	musyx_v1_task(&_hle);
}

void ProcessMusyX_v2()
{
	SetupMusyX();
	musyx_v2_task(&_hle);
}
