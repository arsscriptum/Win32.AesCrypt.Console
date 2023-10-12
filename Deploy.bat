@echo off
setlocal EnableDelayedExpansion

:: ==============================================================================
:: 
::      Deploy.bat
::
::      Deploy the app after build
::
:: ==============================================================================
::   arsccriptum - made in quebec 2020 <guillaumeplante.qc@gmail.com>
:: ==============================================================================

goto :init

:init
    set "__scripts_root=%AutomationScriptsRoot%"
    call :read_script_root development\build-automation  BuildAutomation
    set "__script_file=%~0"
    set "__target=%~1"
    set "__script_path=%~dp0"
    set "__makefile=%__scripts_root%\make\make.bat"
    set "__lib_date=%__scripts_root%\batlibs\date.bat"
    set "__lib_out=%__scripts_root%\batlibs\out.bat"
    ::*** This is the important line ***
    set "__build_cfg=%__script_path%buildcfg.ini"
    set "__build_script=%__script_path%Build.bat"
    set "__binary_path=%__script_path%bin"
    set "__compiled_bin=%__script_path%bin\x64\Release\AesCrypter.exe"
    set "__build_cancelled=0"
    goto :validate


:header
    echo. %__script_name% v%__script_version%
    echo.    This script is part of Ars Scriptum build wrappers.
    echo.
    goto :eof

:header_err
    echo. ======================================================
    echo. = This script is part of Ars Scriptum build wrappers =
    echo. ======================================================
    echo.
    echo. YOU NEED TO HAVE THE BuildAutomation Scripts setup
    echo. on you system...
    echo. https://github.com/arsscriptum/BuildAutomation
    goto :eof


:read_script_root
    set regpath=%OrganizationHKCU::=%
    for /f "tokens=2,*" %%A in ('REG.exe query %regpath%\%1 /v %2') do (
            set "__scripts_root=%%B"
        )
    goto :eof

:validate
    if not defined __scripts_root          call :header_err && call :error_missing_path __scripts_root & goto :eof
    if not exist %__build_script%  call :error_missing_script "%__build_script%" & goto :eof

    goto :deploy


:: ==============================================================================
::   invoke_clean
:: ==============================================================================
:invoke_clean
    call %__lib_out% :__out_d_yel " ======================================================================="
    call %__lib_out% :__out_d_red "    DELETING %__binary_path%"
    call %__lib_out% :__out_d_yel " ======================================================================="
    rmdir /S /Q "%__binary_path%"
    goto :eof




:: ==============================================================================
::   deploy
:: ==============================================================================
:deploy
    cls
    call :invoke_clean
    call %__build_script%
    if not exist %__compiled_bin%  call :error_no_compiled_binary "%__compiled_bin%" & goto :eof
    setx AES_CRYPTER "%__compiled_bin%"
    set AES_CRYPTER="%__compiled_bin%"
    goto :finished


:error_missing_path
    echo.
    echo   Error
    echo    Missing path: %~1
    echo.
    goto :eof


:error_no_compiled_binary
    echo.
    echo    Error
    echo    NO COMPILED BINARY: %~1
    echo.
    goto :eof


:error_missing_script
    echo.
    echo    Error
    echo    Missing bat script: %~1
    echo.
    goto :eof



:finished
    call %__lib_out% :__out_d_grn "Deploy complete"
