// ソースコードジェネレータ
// バージョン番号
// Copyright (C) 1989,1990 K.Abe, 1994 R.ShimiZu
// All rights reserved.
// Copyright (C) 1997-2023 TcbnErik

// This file is part of dis (source code generator).
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program. If not, see <https://www.gnu.org/licenses/>.

#include "version.h"

#define VERSION "4.02"
#define DATE "2023-07-11"

const char ProgramAndVersion[] = "dis version " VERSION;

const char Title[] =
    "ソースコードジェネレータ for X680x0"
#ifdef __linux__
    " (Linux cross)"
#endif
#ifdef __FreeBSD__
    " (BSD cross)"
#endif
#ifdef __CYGWIN__
    " (Cygwin cross)"
#endif
#ifdef __MINGW32__
    " (MinGW cross)"
#endif
    " version " VERSION
    "\n"
    "Copyright (C)1989-1992 K.Abe, 1994-1997 R.ShimiZu, " DATE
    " TcbnErik.\n"

#ifdef OSKDIS
    "OS-9/68000 version by TEMPLE, 1994\n"
#endif
    "";

// EOF
