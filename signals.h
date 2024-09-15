#ifndef SIGNALS_H_
#define SIGNALS_H_

void handle_sigchld();
void setup_signal_handlers(void);
void handle_sigint();
void sigtstp_handler();

#endif