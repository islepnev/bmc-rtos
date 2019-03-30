#ifndef ANSI_ESCAPE_CODES_H
#define ANSI_ESCAPE_CODES_H

/* Terminal control ANSI escape codes */
#define CSI "\x1B["

#define ANSI_CLEARTERM CSI "2J"
#define ANSI_GOHOME    CSI "1;1H"

#define ANSI_BOLD      CSI "1m"
#define ANSI_BLINK     CSI "5m"

#define ANSI_BLACK     CSI "30m"
#define ANSI_RED       CSI "31m"
#define ANSI_GREEN     CSI "32m"
#define ANSI_YELLOW    CSI "33m"
#define ANSI_BLUE      CSI "34m"
#define ANSI_PUR       CSI "35m"
#define ANSI_CYAN      CSI "36m"
#define ANSI_GRAY      CSI "37m"

#define ANSI_BGR_BLACK  CSI "40m"
#define ANSI_BGR_RED    CSI "41m"
#define ANSI_BGR_GREEN  CSI "42m"
#define ANSI_BGR_YELLOW CSI "43m"
#define ANSI_BGR_BLUE   CSI "44m"
#define ANSI_BGR_PUR    CSI "45m"
#define ANSI_BGR_CYAN   CSI "46m"
#define ANSI_BGR_WHITE  CSI "47m"

#define ANSI_CLEAR     CSI "0m"

#define ANSI_CLEAR_EOL CSI "K"

#define ANSI_COL20     CSI "20G"
#define ANSI_COL30     CSI "30G"
#define ANSI_COL40     CSI "40G"

#define ANSI_SHOW_CURSOR CSI"?25h"
#define ANSI_HIDE_CURSOR CSI"?25l"

#endif /* ANSI_ESCAPE_CODES_H */
