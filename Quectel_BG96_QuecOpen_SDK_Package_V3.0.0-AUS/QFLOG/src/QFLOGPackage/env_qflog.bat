@echo off

REM Cusomter need modify their python-3.6.1 install path
set QFLOG_PYTHON_PATH=c:\compile_tools\Python36

set QFLOGPackage=%~dp0
cd %~dp0..
set QFLOGSRC_PATH=%cd%
cd %~dp0..\..
set QFLOG_PATH=%cd%
cd %~dp0

set PYTHONPATH=%QFLOG_PYTHON_PATH%;%QFLOGPackage%;%QFLOGSRC_PATH%;%QFLOG_PATH%;
set PATH=.;%PYTHONPATH%;%PATH%
