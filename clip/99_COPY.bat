@ECHO OFF
SETLOCAL
SET BATDIR=%~dp0
IF "%CLIP%" NEQ "" (
  SET SRCDIR=%CLIP%\
) ELSE (
  SET SRCDIR=C:\Home\Share\Piece\clip\
)

COPY	%SRCDIR%clipdlst.c
COPY	%SRCDIR%clipdlst.h

COPY	%SRCDIR%cliphash.c
COPY	%SRCDIR%cliphash.h

COPY	%SRCDIR%clipsort.c
COPY	%SRCDIR%clipsort.h

COPY	%SRCDIR%clipstl.c
COPY	%SRCDIR%clipstl.h

COPY	%SRCDIR%clipstr.c
COPY	%SRCDIR%clipstr.h

COPY	%SRCDIR%garray.c
COPY	%SRCDIR%garray.h

COPY	%SRCDIR%ght_hash_table.c
COPY	%SRCDIR%ght_hash_table.h

COPY	%SRCDIR%gslist.c
COPY	%SRCDIR%gslist.h

COPY	%SRCDIR%gstring.c
COPY	%SRCDIR%gstring.h

COPY	%SRCDIR%strv.c
COPY	%SRCDIR%strv.h

TIMEOUT 3
