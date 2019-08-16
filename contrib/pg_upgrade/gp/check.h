#ifndef GP_CHECK_H
#define GP_CHECK_H

extern void
gp_check_failure(const char *restrict fmt,...)
__attribute__((format(printf, 1, 2)));

#endif

