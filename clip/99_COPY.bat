@ECHO OFF
SETLOCAL
SET BATDIR=%~dp0
IF "%CLIP%" NEQ "" (
  SET SRCDIR=%CLIP%\
) ELSE (
  REM SET SRCDIR=C:\Home\Share\Piece\clip\
      SET SRCDIR=..\..\clip\
)

REM //--------------------------------------------------------------------------
REM Unfortunately GOTO can not be used in FOR, so it seems to be the only way to handle it as follows.
REM Reference URL: http://fpcu.on.coocan.jp/dosvcmd/bbs/log/cat3/for_in_do/4-1324.html
FOR %%i IN (*.c *.h) DO (
	IF "%%i" NEQ "clip.h" (
		IF "%%i" NEQ "extclip.h" (
			IF "%%i" NEQ "extclip.c" (
				ECHO %SRCDIR%\%%i
				COPY %SRCDIR%\%%i
				IF ERRORLEVEL 1 GOTO :L_ERR
			)
		)
	)
)

REM //--------------------------------------------------------------------------
EXIT /B

REM //--------------------------------------------------------------------------
:L_ERR
ECHO ###An error occurred.
EXIT /B 1
