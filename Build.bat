@echo off
setlocal EnableDelayedExpansion

:: ==============================================================================
:: 
::      Build.bat
::
::      Build different configuration of the app
::
:: ==============================================================================
::   Guilaume Plante <guillaume.plante@luminator.com>
::   Copyright (C) Luminator Technology Group
:: ==============================================================================

goto :init

:init
    set "__scripts_root=%AutomationScriptsRoot%"
    set "__executable_name=AesCrypter.exe"
    call :read_script_data development\build-automation  BuildAutomation
    set "__script_file=%~0"
    set "__target=%~1"
    set "__script_path=%~dp0"
    set "__makefile=%__scripts_root%\make\make.bat"
    set "__lib_date=%__scripts_root%\batlibs\date.bat"
    set "__lib_out=%__scripts_root%\batlibs\out.bat"
    ::*** This is the important line ***
    set "__build_cfg=%__script_path%buildcfg.ini"
    set "__build_cancelled=0"
    goto :validate


:header
    echo. %__script_name% v%__script_version%
    echo.    This script is part of my custom LTG build wrappers.
    echo.
    goto :eof

:header_err
    echo. =======================================================
    echo. = This script is part of my custom LTG build wrappers =
    echo. =======================================================
    echo.
    echo. YOU NEED TO HAVE THE BuildAutomation Scripts setup
    echo. on you system...
    echo. http://vsvr-gitlab-01/guillaume.plante/buildautomation
    goto :eof


:read_script_data
    if not defined OrganizationHKCU          call :header_err && call :error_missing_path OrganizationHKCU::= & goto :eof
    set regpath=%OrganizationHKCU::=%
    ::echo "OrganizationHKCU => %regpath%"
    for /f "tokens=2,*" %%A in ('REG.exe query %regpath%\%1 /v %2') do (
            set "__scripts_root=%%B"
        )
    goto :eof

:validate
    if not defined __scripts_root          call :header_err && call :error_missing_path __scripts_root & goto :eof
    if not exist %__makefile%  call :error_missing_script "%__makefile%" & goto :eof
    if not exist %__lib_date%  call :error_missing_script "%__lib_date%" & goto :eof
    if not exist %__lib_out%  call :error_missing_script "%__lib_out%" & goto :eof
    if not exist %__build_cfg%  call :error_missing_script "%__build_cfg%" & goto :eof

    goto :prebuild_header


:prebuild_header
    call %__lib_date% :getbuilddate
    call %__lib_out% :__out_d_yel "======================================================="
    call %__lib_out% :__out_d_red "=        AESCRYPTER - STARTING COMPILATION            ="
    call %__lib_out% :__out_d_yel "======================================================="
    call :build
    goto :eof


:: ==============================================================================
::   call make
:: ==============================================================================
:call_make_build
    set config=%1
    set platform=%2
    call %__makefile% /i %__build_cfg% /t Build /c %config% /p %platform%
    goto :end

:call_make_build_export
    set config=%1
    set platform=%2
    call %__makefile% /i %__build_cfg% /t Build /c %config% /p %platform% /x
    goto :end

:: ==============================================================================
::   Build static
:: ==============================================================================
:build_x86_debug
    call :call_make_build Debug x86
    goto :end

:: ==============================================================================
::   Build x64
:: ==============================================================================
:build_Release
    call :call_make_build_export Release x64
    goto :end

:: ==============================================================================
::   clean all
:: ==============================================================================
:clean
    call %__makefile% /i %__build_cfg% /t Clean /c Debug /p x86
    call %__makefile% /i %__build_cfg% /t Clean /c Release /p x86
    call %__makefile% /i %__build_cfg% /t Clean /c Debug /p x64
    call %__makefile% /i %__build_cfg% /t Clean /c Release /p x64
    goto :end


:protek
	set APP_PATH=%cd%\bin\x64\Release
	call %__lib_out% :__out_n_l_yel "ENCRYPTION"
	"%AXPROTECTOR_SDK%\bin\AxProtector.exe" -x -kcm -f6000010 -p101001 -cf0 -d:6.20 -fw:3.00 -slw -nn -cav -cas100 -wu1000 -we100 -eac -eec -eusc1 -emc -v -cag23 -caa7 -o:"%APP_PATH%\%__executable_name%" "%APP_PATH%\%__executable_name%"
	call %__lib_out% :__out_d_grn " SUCCESS"
	goto :end


:: ==============================================================================
::   Build
:: ==============================================================================
:build
	if "%__target%" == "clean" (
		call :clean
		)
    if "%__target%" == "rebuild" (
		call :clean
		)
    if "%__target%" == "debug" (
        call :build_x86_debug
        )
    ::call :build_x86

    call :build_Release
    ::call :protek
    goto :finished


:error_missing_path
    echo.
    echo   Error
    echo    Missing path: %~1
    echo.
    goto :eof



:error_missing_script
    echo.
    echo    Error
    echo    Missing bat script: %~1
    echo.
    goto :eof



:finished
    call %__lib_out% :__out_log_script_success %__script_file%
    exit /B

:end
    exit /B


 