#include "pthread_impl.h"
#include "syscall.h"

static volatile int check_pi_result = -1;

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *a, int protocol)
{
	int r;
	switch (protocol) {
	case PTHREAD_PRIO_NONE:
		a->__attr &= ~8;
		return 0;
	case PTHREAD_PRIO_INHERIT:
		r = check_pi_result;
		if (r < 0) {
			volatile int lk = 0;
#ifdef __wasilibc_unmodified_upstream
			r = -__syscall(SYS_futex, &lk, FUTEX_LOCK_PI, 0, 0);
#else
			r = __wasilibc_futex_wait(&lk, FUTEX_LOCK_PI, 0, 0);
#endif
			a_store(&check_pi_result, r);
		}
		if (r) return r;
		a->__attr |= 8;
		return 0;
	case PTHREAD_PRIO_PROTECT:
		return ENOTSUP;
	default:
		return EINVAL;
	}
}
