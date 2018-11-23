#ifndef __OS_STACKTRACE_H__
#define __OS_STACKTRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*! Initialize backtrace handler, once "Segmentation fault" occured,
 * the backtrace info will be show like "(gdb) bt"
 *
 */
int debug_backtrace_init(void);

#ifdef __cplusplus
}
#endif
#endif
