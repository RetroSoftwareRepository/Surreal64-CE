@ECHO OFF
cls
COLOR 1B
rem ---------------------------------------------
TITLE Surreal64CE Build Script
ECHO Preparing the build.
ECHO ------------------------------
rem	CONFIG START

	set MODE=debug
	set DIST=0
	set ERROR=0

	IF "%VS71COMNTOOLS%"=="" (
	  set NET="%ProgramFiles%\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.com"
	) ELSE (
	  set NET="%VS71COMNTOOLS%\..\IDE\devenv.com"
	)
	IF NOT EXIST %NET% (
	  set DIETEXT=Visual Studio .NET 2003 was not found.
	  goto DIE
	)
	
	:: using all-in-one solution instead to cut down on compile time
	:: set SOL_LIST=("Surreal64 CE.sln" Project510.sln Project531.sln Project560.sln Project611.sln Project612.sln)
	set SOL_LIST=(Build.sln)

	set XBE_1964x085_LIST=(1964x085-510.xbe 1964x085-510M.xbe 1964x085-531.xbe 1964x085-531M.xbe 1964x085-560.xbe 1964x085-560M.xbe 1964x085-611.xbe 1964x085-611M.xbe 1964x085-612.xbe 1964x085-612M.xbe)
	set XBE_1964x11_LIST=(1964x11-510.xbe 1964x11-510M.xbe 1964x11-531.xbe 1964x11-531M.xbe 1964x11-560.xbe 1964x11-560M.xbe 1964x11-611.xbe 1964x11-611M.xbe 1964x11-612.xbe 1964x11-612M.xbe)
	set XBE_PJ64x14_LIST=(PJ64x14-510.xbe PJ64x14-510M.xbe PJ64x14-531.xbe PJ64x14-531M.xbe PJ64x14-560.xbe PJ64x14-560M.xbe PJ64x14-611.xbe PJ64x14-611M.xbe PJ64x14-612.xbe PJ64x14-612M.xbe)
	set XBE_PJ64x16_LIST=(PJ64x16-510.xbe PJ64x16-510M.xbe PJ64x16-531.xbe PJ64x16-531M.xbe PJ64x16-560.xbe PJ64x16-560M.xbe PJ64x16-611.xbe PJ64x16-611M.xbe PJ64x16-612.xbe PJ64x16-612M.xbe)
	set XBE_LNCH_LIST=(default.xbe UltraHLE.xbe)
	
	goto PROMPT_MODE

rem	CONFIG END
rem ---------------------------------------------
:PROMPT_MODE
	ECHO Select a build type:
	ECHO [D] Debug
	ECHO [P] Profile
	ECHO [F] Profile FastCap
	ECHO [R] Release
	ECHO [L] Release LTCG
	ECHO ------------------------------
	set /P S64CE_COMPILE_ANSWER=Which mode should be used [d/p/f/r/l] ?
	if /i "%S64CE_COMPILE_ANSWER:~,1%" EQU "d" set MODE=debug
	if /i "%S64CE_COMPILE_ANSWER:~,1%" EQU "p" set MODE=profile
	if /i "%S64CE_COMPILE_ANSWER:~,1%" EQU "f" set MODE=profile_fastcap
	if /i "%S64CE_COMPILE_ANSWER:~,1%" EQU "r" set MODE=release
	if /i "%S64CE_COMPILE_ANSWER:~,1%" EQU "l" set MODE=release_ltcg
	
	if %MODE%==release goto PROMPT_DIST
	if %MODE%==release_ltcg goto PROMPT_DIST
	
	goto COMPILE
rem ---------------------------------------------

:PROMPT_DIST
	ECHO ------------------------------
	ECHO Distribution Build:
	ECHO [Y] Include Dist and Copyright
	ECHO [N] XBE's only
	ECHO ------------------------------
	set /P S64CE_BUILD_ANSWER=Is this build for distribution? [y/n]
	if /I %S64CE_BUILD_ANSWER% NEQ y set DIST=0
	if /I %S64CE_BUILD_ANSWER% NEQ n set DIST=1
	goto COMPILE

:COMPILE
	ECHO ------------------------------
	ECHO Compiling Solution(s) [%MODE% mode]
	ECHO ------------------------------
	
	for %%i in %SOL_LIST% do (
		ECHO Cleaning...
		%NET% %%i /clean %MODE%
	)

	for %%i in %SOL_LIST% do (
		ECHO Compiling...
		%NET% %%i /build %MODE%
	)

	for %%i in %XBE_1964x085_LIST% do IF NOT EXIST %MODE%\%%i (
		ECHO ----
		echo %%i failed to build!
		set ERROR=1
	)
	for %%i in %XBE_1964x11_LIST% do IF NOT EXIST %MODE%\%%i (
		ECHO ----
		echo %%i failed to build!
		set ERROR=1
	)
	for %%i in %XBE_PJ64x14_LIST% do IF NOT EXIST %MODE%\%%i (
		ECHO ----
		echo %%i failed to build!
		set ERROR=1
	)
	for %%i in %XBE_PJ64x16_LIST% do IF NOT EXIST %MODE%\%%i (
		ECHO ----
		echo %%i failed to build!
		set ERROR=1
	)
	for %%i in %XBE_LNCH_LIST% do IF NOT EXIST %MODE%\%%i (
		ECHO ----
		echo %%i failed to build!
		set ERROR=1
	)
	if %ERROR%==1 (
		set DIETEXT="Build Failed! (See BuildLog.htm(s) for details)"
		goto DIE
	)
	
	ECHO Done!
	ECHO ------------------------------
	GOTO MAKE_BUILD

:MAKE_BUILD
	ECHO Copying files...
	rmdir BUILD /S /Q
	md BUILD
	
	Echo .svn>exclude.txt
	Echo Thumbs.db>>exclude.txt
	Echo Desktop.ini>>exclude.txt
	Echo exclude.txt>>exclude.txt
	
	xcopy %MODE%\*.xbe BUILD /EXCLUDE:exclude.txt
	
	if %DIST%==1 (
		ECHO Copying Dist... [this can take a while]
		xcopy *.txt BUILD /EXCLUDE:exclude.txt
		xcopy Dist BUILD /E /Q /I /Y /EXCLUDE:exclude.txt
	)
	
	del exclude.txt
	
	:: copy XBE's to the XBOX in case the deployment tool failed to during compile
	IF EXIST %XDK%\xbox\bin\xbcp.exe (
	
		:: give it a few seconds to close all current connections
		ECHO Preparing to copy XBE files to the XBOX...
		ping 1.1.1.1 -n 1 -w 5000>nul
		
		xbcp /y "BUILD\*.xbe" "xe:\Surreal64 CE"
	)
	
	ECHO ------------------------------
	ECHO Build Succeeded!
	
	ECHO ------------------------------
	ECHO Cleaning...
	for %%i in %SOL_LIST% do %NET% %%i /clean %MODE%
	ECHO Done!
	ECHO ------------------------------
	
	goto VIEWPAUSE

:DIE
	ECHO !-!-!-!-!-!-!-!-!-!-!-!-!-!-!-
	set DIETEXT=ERROR: %DIETEXT%
	echo %DIETEXT%
	ECHO !-!-!-!-!-!-!-!-!-!-!-!-!-!-!-
	goto VIEWPAUSE

:VIEWPAUSE
	set S64CE_BUILD_ANSWER=
	ECHO Press any key to exit...
	pause > NUL
  
:END