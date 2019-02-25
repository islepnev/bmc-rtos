#ifndef ANSI_ESCAPE_CODES_H
#define ANSI_ESCAPE_CODES_H

/* Terminal control ANSI escape codes */
#define ESCAPE "\x1B"
#define ANSI_CLEARTERM ESCAPE "[2J"
#define ANSI_GOHOME    ESCAPE "[1;1H"

#define ANSI_BOLD      ESCAPE "[1m"

#define ANSI_BLACK     ESCAPE "[30m"
#define ANSI_RED       ESCAPE "[31m"
#define ANSI_GREEN     ESCAPE "[32m"
#define ANSI_YELLOW    ESCAPE "[33m"
#define ANSI_BLUE      ESCAPE "[34m"
#define ANSI_PUR       ESCAPE "[35m"
#define ANSI_CYAN      ESCAPE "[36m"
#define ANSI_GRAY      ESCAPE "[37m"

#define ANSI_BGR_BLACK  ESCAPE "[40m"
#define ANSI_BGR_RED    ESCAPE "[41m"
#define ANSI_BGR_GREEN  ESCAPE "[42m"
#define ANSI_BGR_YELLOW ESCAPE "[43m"
#define ANSI_BGR_BLUE   ESCAPE "[44m"
#define ANSI_BGR_PUR    ESCAPE "[45m"
#define ANSI_BGR_CYAN   ESCAPE "[46m"
#define ANSI_BGR_WHITE  ESCAPE "[47m"

#define ANSI_CLEAR     ESCAPE "[0m"

#define ANSI_CLEAR_EOL ESCAPE "[K"

#define ANSI_COL20     ESCAPE "[20G"
#define ANSI_COL30     ESCAPE "[30G"
#define ANSI_COL40     ESCAPE "[40G"

#endif /* ANSI_ESCAPE_CODES_H */
