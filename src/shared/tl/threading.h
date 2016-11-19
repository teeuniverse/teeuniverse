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

#ifndef __SHARED_TL_THREADING__
#define __SHARED_TL_THREADING__

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include <shared/system/threading.h>

/*
	atomic_inc - should return the value after increment
	atomic_dec - should return the value after decrement
	atomic_compswap - should return the value before the eventual swap
	sync_barrier - creates a full hardware fence
*/

#if defined(__GNUC__)

	inline unsigned atomic_inc(volatile unsigned *pValue)
	{
		return __sync_add_and_fetch(pValue, 1);
	}

	inline unsigned atomic_dec(volatile unsigned *pValue)
	{
		return __sync_add_and_fetch(pValue, -1);
	}

	inline unsigned atomic_compswap(volatile unsigned *pValue, unsigned comperand, unsigned value)
	{
		return __sync_val_compare_and_swap(pValue, comperand, value);
	}

	inline void sync_barrier()
	{
		__sync_synchronize();
	}

#elif defined(_MSC_VER)
	#include <intrin.h>

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>

	inline unsigned atomic_inc(volatile unsigned *pValue)
	{
		return _InterlockedIncrement((volatile long *)pValue);
	}
	
	inline unsigned atomic_dec(volatile unsigned *pValue)
	{
		return _InterlockedDecrement((volatile long *)pValue);
	}

	inline unsigned atomic_compswap(volatile unsigned *pValue, unsigned comperand, unsigned value)
	{
		return _InterlockedCompareExchange((volatile long *)pValue, (long)value, (long)comperand);
	}

	inline void sync_barrier()
	{
		MemoryBarrier();
	}
#else
	#error missing atomic implementation for this compiler
#endif

#if defined(CONF_PLATFORM_MACOSX)
	/*
		use semaphore provided by SDL on macosx
	*/
#else
	class semaphore
	{
		SEMAPHORE sem;
	public:
		semaphore() { semaphore_init(&sem); }
		~semaphore() { semaphore_destroy(&sem); }
		void wait() { semaphore_wait(&sem); }
		void signal() { semaphore_signal(&sem); }
	};
#endif

class lock
{
	friend class scope_lock;

	LOCK var;

	void take() { lock_wait(var); }
	void release() { lock_unlock(var); }

public:
	lock()
	{
		var = lock_create();
	}

	~lock()
	{
		lock_destroy(var);
	}
};

class scope_lock
{
	lock *var;
public:
	scope_lock(lock *l)
	{
		var = l;
		var->take();
	}

	~scope_lock()
	{
		var->release();
	}
};

/* FOREIGN CODE END: TeeWorlds ****************************************/

#endif

