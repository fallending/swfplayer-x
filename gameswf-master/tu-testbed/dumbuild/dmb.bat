@echo off
rem dumbuild build shell; bootstraps if necessary
if exist %~dp0dmb-out\bootstrap\dmb.exe goto skip_bootstrap
  echo bootstrapping dmb.exe
  %~dp0bootstrap.bat

:skip_bootstrap
%~dp0dmb-out\bootstrap\dmb.exe %*
