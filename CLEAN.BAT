@ECHO OFF
SETLOCAL
SET BATDIR=%~dp0

IF EXIST	.vs\			RMDIR /S /Q	.vs\

IF EXIST	x64\			RMDIR /S /Q	x64\
IF EXIST	Debug\			RMDIR /S /Q	Debug\
IF EXIST	Release\		RMDIR /S /Q	Release\

IF EXIST	TinyJS\x64\		RMDIR /S /Q	TinyJS\x64\
IF EXIST	TinyJS\Debug\		RMDIR /S /Q	TinyJS\Debug\
IF EXIST	TinyJS\Release\		RMDIR /S /Q	TinyJS\Release\

IF EXIST	Script\x64\		RMDIR /S /Q	Script\x64\
IF EXIST	Script\Debug\		RMDIR /S /Q	Script\Debug\
IF EXIST	Script\Release\		RMDIR /S /Q	Script\Release\

IF EXIST	run_tests\x64\		RMDIR /S /Q	run_tests\x64\
IF EXIST	run_tests\Debug\	RMDIR /S /Q	run_tests\Debug\
IF EXIST	run_tests\Release\	RMDIR /S /Q	run_tests\Release\
