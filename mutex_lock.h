/*
 * Adapted from https://blog.csdn.net/u011857683/article/details/53046295
 */
#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include <mutex>

#define EPRINT printf	// error print
#define DPRINT printf	// debug print

#define LOCK_SUCCESS 0
#define LOCK_FAILED -1

class MutexLock
{
public:
	MutexLock();
	~MutexLock();

	int lock();
	int unlock();

private:
	void mutexInit();
	void mutexDestroy();

private:
	std::mutex m_mutex;
};

#endif
