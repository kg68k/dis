// ソースコードジェネレータ
// getopt
// Copyright (C) 2010-2023 TcbnErik

#ifndef GETOPT_H
#define GETOPT_H

#define optarg dis_optarg
#define optind dis_optind

extern char* optarg;
extern int optind;
extern int dis_getopt(int, char**, const char*);

#endif

// EOF
