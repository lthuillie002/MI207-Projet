#ifndef _UTILS_H
#define _UTILS_H

#include <stddef.h> // To mitigate effect of std=c99
#include <signal.h> // for sigset_t

/**
 * exit printing error prefixed by `prefix` if `condition` is true (non zero)
 */
void exit_if(int condition, const char *prefix);

/**
 * Compute fibonnaci of `n` (takes time around 40 on my computer)
 */
unsigned long long fibo(int n);

/**
 * Return a signle character that represent the file type from a `st_mode`
 */
char file_type(int st_mode);

/**
 * splits `argv` between `args1` or `args2`
 * if they are before or after the separator "--".
 * Returns -1 on error
 */
int split_args(char *argv[], char *args1[], char *args2[]);

/**
 * Spawn a child process that will wait `time` seconds
 * before exiting
 */
int create_waiting_child(int time);

/**
 * This function does nothing.
 * It is intented to be used with `sigaction` or `signal`
 */
void do_nothing(int sig);

/**
 * Return a pretty name for `signal`, if `signal` is invalid,
 * returns "SIG???"
 */
const char *signame(int signal);

/**
 * Pretty print on `fd` informations about signals
 */
void print_signals(int fd);

/**
 * Pretty print on `fd` the content of `sigset`.
 * This may be useful to print signals pending or masked
 */
void print_sigset(int fd, const sigset_t *sigset);

#endif
