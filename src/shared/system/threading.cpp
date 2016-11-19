/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverses.
 * 
 * TeeUniverses is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverses is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverses.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include "threading.h"

#if defined(CONF_FAMILY_UNIX)	
	#include <cstdlib>
	#include <pthread.h>
	#include <unistd.h>
	
	//I don't know if theses includes are needed
	#if defined(CONF_PLATFORM_MACOSX)
		#include <Carbon/Carbon.h>
	#endif
	
#elif defined(CONF_FAMILY_WINDOWS)
	//I don't know if theses includes are needed
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <fcntl.h>
	#include <direct.h>
	#include <errno.h>
	#include <wincrypt.h>
	
#else
	#error NOT IMPLEMENTED
#endif

void *thread_init(void (*threadfunc)(void *), void *u)
{
#if defined(CONF_FAMILY_UNIX)
	pthread_t id;
	pthread_create(&id, NULL, (void *(*)(void*))threadfunc, u);
	return (void*)id;
#elif defined(CONF_FAMILY_WINDOWS)
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadfunc, u, 0, NULL);
#else
	#error not implemented
#endif
}

void thread_wait(void *thread)
{
#if defined(CONF_FAMILY_UNIX)
	pthread_join((pthread_t)thread, NULL);
#elif defined(CONF_FAMILY_WINDOWS)
	WaitForSingleObject((HANDLE)thread, INFINITE);
#else
	#error not implemented
#endif
}

void thread_destroy(void *thread)
{
#if defined(CONF_FAMILY_UNIX)
	void *r = 0;
	pthread_join((pthread_t)thread, &r);
#else
	/*#error not implemented*/
#endif
}

void thread_yield()
{
#if defined(CONF_FAMILY_UNIX)
	sched_yield();
#elif defined(CONF_FAMILY_WINDOWS)
	Sleep(0);
#else
	#error not implemented
#endif
}

void thread_sleep(int milliseconds)
{
#if defined(CONF_FAMILY_UNIX)
	usleep(milliseconds*1000);
#elif defined(CONF_FAMILY_WINDOWS)
	Sleep(milliseconds);
#else
	#error not implemented
#endif
}

void thread_detach(void *thread)
{
#if defined(CONF_FAMILY_UNIX)
	pthread_detach((pthread_t)(thread));
#elif defined(CONF_FAMILY_WINDOWS)
	CloseHandle(thread);
#else
	#error not implemented
#endif
}




#if defined(CONF_FAMILY_UNIX)
typedef pthread_mutex_t LOCKINTERNAL;
#elif defined(CONF_FAMILY_WINDOWS)
typedef CRITICAL_SECTION LOCKINTERNAL;
#else
	#error not implemented on this platform
#endif

LOCK lock_create()
{
	LOCKINTERNAL *lock = new LOCKINTERNAL;

#if defined(CONF_FAMILY_UNIX)
	pthread_mutex_init(lock, 0x0);
#elif defined(CONF_FAMILY_WINDOWS)
	InitializeCriticalSection((LPCRITICAL_SECTION)lock);
#else
	#error not implemented on this platform
#endif
	return (LOCK)lock;
}

void lock_destroy(LOCK lock)
{
#if defined(CONF_FAMILY_UNIX)
	pthread_mutex_destroy((LOCKINTERNAL *)lock);
#elif defined(CONF_FAMILY_WINDOWS)
	DeleteCriticalSection((LPCRITICAL_SECTION)lock);
#else
	#error not implemented on this platform
#endif
	delete ((LOCKINTERNAL*) lock);
}

int lock_trylock(LOCK lock)
{
#if defined(CONF_FAMILY_UNIX)
	return pthread_mutex_trylock((LOCKINTERNAL *)lock);
#elif defined(CONF_FAMILY_WINDOWS)
	return !TryEnterCriticalSection((LPCRITICAL_SECTION)lock);
#else
	#error not implemented on this platform
#endif
}

void lock_wait(LOCK lock)
{
#if defined(CONF_FAMILY_UNIX)
	pthread_mutex_lock((LOCKINTERNAL *)lock);
#elif defined(CONF_FAMILY_WINDOWS)
	EnterCriticalSection((LPCRITICAL_SECTION)lock);
#else
	#error not implemented on this platform
#endif
}

void lock_unlock(LOCK lock)
{
#if defined(CONF_FAMILY_UNIX)
	pthread_mutex_unlock((LOCKINTERNAL *)lock);
#elif defined(CONF_FAMILY_WINDOWS)
	LeaveCriticalSection((LPCRITICAL_SECTION)lock);
#else
	#error not implemented on this platform
#endif
}

#if !defined(CONF_PLATFORM_MACOSX)
	#if defined(CONF_FAMILY_UNIX)
	void semaphore_init(SEMAPHORE *sem) { sem_init(sem, 0, 0); }
	void semaphore_wait(SEMAPHORE *sem) { sem_wait(sem); }
	void semaphore_signal(SEMAPHORE *sem) { sem_post(sem); }
	void semaphore_destroy(SEMAPHORE *sem) { sem_destroy(sem); }
	#elif defined(CONF_FAMILY_WINDOWS)
	void semaphore_init(SEMAPHORE *sem) { *sem = CreateSemaphore(0, 0, 10000, 0); }
	void semaphore_wait(SEMAPHORE *sem) { WaitForSingleObject((HANDLE)*sem, INFINITE); }
	void semaphore_signal(SEMAPHORE *sem) { ReleaseSemaphore((HANDLE)*sem, 1, NULL); }
	void semaphore_destroy(SEMAPHORE *sem) { CloseHandle((HANDLE)*sem); }
	#else
		#error not implemented on this platform
	#endif
#endif

/* FOREIGN CODE END: TeeWorlds ****************************************/

