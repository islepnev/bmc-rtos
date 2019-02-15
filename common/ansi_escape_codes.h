#ifndef ANSI_ESCAPE_CODES_H
#define ANSI_ESCAPE_CODES_H

/* Terminal control ANSI escape codes */
#define ESCAPE "\x1B"
#define ANSI_CLEARTERM ESCAPE "[2J"
#define ANSI_GOHOME    ESCAPE "[1;1H"
#define ANSI_RED       ESCAPE "[0;31m"
#define ANSI_GREEN     ESCAPE "[0;32m"
#define ANSI_YELLOW    ESCAPE "[0;33m"
#define ANSI_BLUE      ESCAPE "[0;34m"
#define ANSI_PUR       ESCAPE "[0;35m"
#define ANSI_CYAN      ESCAPE "[0;36m"
#define ANSI_GRAY      ESCAPE "[0;37m"
#define ANSI_NONE      ESCAPE "[0;1m"
#define ANSI_CLEAR     ESCAPE "[0m"

#define ANSI_COL20     ESCAPE "[20G"
#define ANSI_COL30     ESCAPE "[30G"
#define ANSI_COL40     ESCAPE "[40G"

#endif /* ANSI_ESCAPE_CODES_H */
