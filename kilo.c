/*** includes ***/

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h> 
#include <termios.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s){
    perror(s);
    exit(1);
} 

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH ,&orig_termios) == -1){
        die("tcsetattr");
    }
}

void enableRawMode() {

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");

    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    tcgetattr(STDERR_FILENO, &raw);

    raw.c_iflag &= ~(BRKINT | ICRNL | IXON | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw) == -1 ) die("tcsetattr");
    
}

/*** init ***/

int main () {
    enableRawMode();
    
    while(1){
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        read(STDIN_FILENO, &c, 1);

        if(iscntrl(c)){
            printf("%d\r\n",c);
        }else {
            printf("%d ('%c')\r\n",c,c);
        }
        if ( c == 'q') break;
    }
    
    return 0;
}