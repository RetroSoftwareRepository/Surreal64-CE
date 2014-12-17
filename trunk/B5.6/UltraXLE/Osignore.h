char *disablepatches[]={
#if 0
"osDpGetStatus",
"osDpSetStatus",
"osDpSetNextBuffer",
"__osDpDeviceBusy",
"osDpGetCounters",
#endif
#if 1
"osPiGetDeviceType",
"osPiGetStatus",
"osPiRawReadIo",
"osPiRawWriteIo",
"osPiRawStartDma",
"osPiGetCmdQueue",
"__osPiDevMgr",
"__osPiTable",
"__osPiAccessQueueEnabled",
"__osPiCreateAccessQueue",
"__osPiGetAccess",
"__osPiRelAccess",
"osPiWriteIo",
"osPiReadIo",
//"osPiStartDma",
#endif
#if 1
"__osPfsPifRam",
"osPfsReFormat",
"__osPfsRWInode",
"__osPfsSelectBank",
"osPfsInit",
"__osPfsGetStatus",
"osPfsAllocateFile",
"__osPfsDeclearPage",
"osPfsDeleteFile",
"__osPfsReleasePages",
"osPfsReadWriteFile",
"osPfsFileState",
"osPfsFindFile",
"osPfsSetLabel",
"osPfsGetLabel",
"osPfsIsPlug",
"__osPfsRequestData",
"__osPfsGetInitData",
"osPfsFreeBlocks",
"osPfsNumFiles",
"osPfsReSizeFile",
#endif
#if 1
"osContInit",
"osContStartQuery",
"osContGetQuery",
"osContStartReadData",
"osContGetReadData",
"osContReset",
"__osContGetInitData",
"__osContRamWrite",
"__osContAddressCrc",
"__osContDataCrc",
"__osContRamRead",
"__osContPifRam",
"__osContLastCmd",
"__osContinitialized",
"__osSiGetStatus",
"__osSiRawReadIo",
"__osSiDeviceBusy",
"__osSiRawWriteIo",
"__osSiRawStartDma",
"__osSiCreateAccessQueue",
"__osSiGetAccess",
"__osSiRelAccess",
"__osSiAccessQueue",
"__osSiAccessQueueEnabled",
#endif
#ifdef NOSPATCH
"__osSpGetStatus",
"__osSpSetStatus",
"__osSpSetPc",
"__osSpRawReadIo",
"__osSpDeviceBusy",
"__osSpRawWriteIo",
"__osSpRawStartDma",
"osSpTaskLoad",
"osSpTaskStartGo",
"osSpTaskYield",
"osSpTaskYielded",
#endif
"__osGetCause",
"__osGetCompare",
"__osGetConfig",
//"osGetCount",
"__osGetFpcCsr",
"__osGetSR",
//"osGetIntMask",
"__osGetTLBASID",
"__osGetTLBHi",
"__osGetTLBLo0",
"__osGetTLBLo1",
"__osGetTLBPageMask",
//"osInvalDCache",
//"osInvalICache",
//"__osDisableInt",
//"__osRestoreInt",
//"osMapTLB",
//"osSetIntMask",
"__osSetCause",
"__osSetCompare",
"__osSetConfig",
"__osSetCount",
"__osSetFpcCsr",
"__osSetSR",
//"osSetTLBASID",
//"osUnmapTLB",
//"osUnmapTLBAll",
//"osWritebackDCache",
//"osWritebackDCacheAll",
//"osMapTLBRdb",
//"__osAtomicDec",
//"osCreateMesgQueue",
//"osCreateThread",
//"__osExceptionPreamble",
//"__osException",
//"send_mesg",
//"handle_CpU",
//"__osEnqueueAndYield",
//"__osEnqueueThread",
//"__osPopThread",
//"__osDispatchThread",
//"__osCleanupThread",
//"osDestroyThread",
//"__osGetActiveQueue",
//"osGetThreadId",
//"osGetThreadPri",
//"osGetTime",
//"osInitialize",
//"osJamMesg",
//"osPhysicalToVirtual",
//"osRecvMesg",
//"__osResetGlobalIntMask",
//"osSendMesg",
//"osSetEventMesg",
//"__osSetGlobalIntMask",
//"__osSetHWIntrRoutine",
//"osSetThreadPri",
//"osSetTime",
//"osSetTimer",
//"osStartThread",
//"osStopThread",
//"osStopTimer",
//"__osDequeueThread",
//"__osTimerServicesInit",
//"__osTimerInterrupt",
//"__osSetTimerIntr",
//"__osInsertTimer",
//"osVirtualToPhysical",
//"__osProbeTLB",
//"osYieldThread",
//"__osGetCurrFaultedThread",
//"__osGetNextFaultedThread",
//"osAiGetLength",
//"osAiGetStatus",
//"osAiSetFrequency",
//"osAiSetNextBuffer",
//"__osAiDeviceBusy",
//"__osViInit",
//"osViGetCurrentField",
//"osViGetCurrentFramebuffer",
//"osViGetNextFramebuffer",
//"osViGetCurrentLine",
//"osViGetCurrentMode",
//"osViGetStatus",
//"osCreateViManager",
//"__osViGetCurrentContext",
//"osViSetEvent",
//"osViSetMode",
//"osViSetSpecialFeatures",
//"osViSetXScale",
//"osViSetYScale",
//"osViSwapBuffer",
//"__osViSwapContext",
//"osViBlack",
//"osViFade",
//"__osPackResetData",
//"__osPackRequestData",
//"__osSumcalc",
//"__osIdCheckSum",
//"__osRepairPackId",
//"__osCheckPackId",
//"__osGetId",
//"__osCheckId",
//"osPfsChecker",
//"corrupted_init",
//"corrupted",
//"__osBlockSum",
//"osEepromRead",
//"osEepromWrite",
//"__osEepStatus",
//"osEepromProbe",
//"osEepromLongWrite",
//"osEepromLongRead",

//"osCreatePiManager",
//"osEPiRawReadIo",
//"osEPiRawWriteIo",
//"osEPiRawStartDma",
//"osEPiWriteIo",
//"osEPiReadIo",
//"osEPiStartDma",
//"osCartRomInit",
//"osLeoDiskInit",
//"__osLeoInterrupt",
//"__osDevMgrMain",

//"bcmp",
//"bcopy",
//"bzero",
//"__ull_rshift",
//"__ull_rem",
//"__ull_div",
//"__ll_lshift",
//"__ll_rem",
//"__ll_div",
//"__ll_mul",
//"__ull_divremi",
//"__ll_mod",
//"__ll_rshift",
//"sprintf",
//"memcpy",
//"strlen",
//"strchr",
//"_Printf",
//"_Litob",
//"lldiv",
//"ldiv",
//"_Ldtob",
//"osSyncPrintf",
//"osClockRate",
//"__osShutdown",
//"__OSGlobalIntMask",
//"osDiskExist",
//"__osThreadTail",
//"__osRunQueue",
//"__osActiveQueue",
//"__osRunningThread",
//"__osFaultedThread",
//"__osTimerList",
//"__osViCurr",
//"__osViNext",
//"osViClock",
//"__osViDevMgr",
//"osViModeNtscLan1",
//"osViModePalLan1",
//"osViModeMpalLan1",
//"__osThreadSave",
//"__osEventStateTab",
//"__osBaseTimer",
//"__osCurrentTime",
//"__osBaseCounter",
//"__osViIntrCount",
//"__osTimerCounter",
//"__osMaxControllers",
"__osEepromTimer",
"__osEepromTimerQ",
"__osEepromTimerMsg",
"__osEepPifRam",
NULL};

