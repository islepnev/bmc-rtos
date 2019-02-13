#ifndef ANSI_ESCAPE_CODES_H
#define ANSI_ESCAPE_CODES_H

/* Terminal control ANSI escape codes */
#define ANSI_CLEARTERM "\e[2J"
#define ANSI_GOHOME "\e[1;1H"
#define ANSI_RED   "\x1B[0;31m"
#define ANSI_GREEN "\x1B[0;32m"
#define ANSI_YELLOW "\x1B[0;33m"
#define ANSI_BLUE "\x1B[0;34m"
#define ANSI_PUR   "\x1B[0;35m"
#define ANSI_CYAN  "\x1B[0;36m"
#define ANSI_GRAY  "\x1B[0;37m"
#define ANSI_NONE  "\x1B[0;1m"
#define ANSI_CLEAR "\x1B[0m"

#define ANSI_COL20 "\e[20G"
#define ANSI_COL30 "\e[30G"
#define ANSI_COL40 "\e[40G"

#endif /* ANSI_ESCAPE_CODES_H */
