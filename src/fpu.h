// ソースコードジェネレータ
// 浮動小数点命令逆アセンブル ヘッダ
// Copyright (C) 2023 TcbnErik

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

#ifndef FPU_H
#define FPU_H

#include "disasm.h"
#include "estruct.h"

extern boolean disfp(codeptr ptr, disasm* code);

#endif

// EOF
