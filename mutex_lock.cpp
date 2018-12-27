#include "mutex_lock.h"

MutexLock::MutexLock()
{
	mutexInit();
}

MutexLock::~MutexLock()
{
	mutexDestroy();
}

void MutexLock::mutexInit()
{
	// do nothing
}

void MutexLock::mutexDestroy()
{
	// do nothing
}

int MutexLock::lock()
{
	if (m_mutex.try_lock())
		return LOCK_SUCCESS;
	else
		return LOCK_FAILED;
}

int MutexLock::unlock()
{
	m_mutex.unlock();
	return 0;
}
