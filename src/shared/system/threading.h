/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverse.
 * 
 * TeeUniverse is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
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

#ifndef __SHARED_SYSTEM_THREADING__
#define __SHARED_SYSTEM_THREADING__

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

/*
	Function: thread_sleep
		Suspends the current thread for a given period.

	Parameters:
		milliseconds - Number of milliseconds to sleep.
*/
void thread_sleep(int milliseconds);

/*
	Function: thread_init
		Creates a new thread.

	Parameters:
		threadfunc - Entry point for the new thread.
		user - Pointer to pass to the thread.

*/
void *thread_init(void (*threadfunc)(void *), void *user);

/*
	Function: thread_wait
		Waits for a thread to be done or destroyed.

	Parameters:
		thread - Thread to wait for.
*/
void thread_wait(void *thread);

/*
	Function: thread_destroy
		Destroys a thread.

	Parameters:
		thread - Thread to destroy.
*/
void thread_destroy(void *thread);

/*
	Function: thread_yeild
		Yeild the current threads execution slice.
*/
void thread_yield();

/*
	Function: thread_detach
		Puts the thread in the detached thread, guaranteeing that
		resources of the thread will be freed immediately when the
		thread terminates.

	Parameters:
		thread - Thread to detach
*/
void thread_detach(void *thread);

/* Group: Locks */
typedef void* LOCK;

LOCK lock_create();
void lock_destroy(LOCK lock);

int lock_trylock(LOCK lock);
void lock_wait(LOCK lock);
void lock_unlock(LOCK lock);


/* Group: Semaphores */

#if !defined(CONF_PLATFORM_MACOSX)
	#if defined(CONF_FAMILY_UNIX)
		#include <semaphore.h>
		typedef sem_t SEMAPHORE;
	#elif defined(CONF_FAMILY_WINDOWS)
		typedef void* SEMAPHORE;
	#else
		#error missing sempahore implementation
	#endif

	void semaphore_init(SEMAPHORE *sem);
	void semaphore_wait(SEMAPHORE *sem);
	void semaphore_signal(SEMAPHORE *sem);
	void semaphore_destroy(SEMAPHORE *sem);
#endif

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif
