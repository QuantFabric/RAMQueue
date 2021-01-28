#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>

#define SHM_NAME_LEN 128
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define IS_POT(x) ((x) && !((x) & ((x)-1)))
#define MEMORY_BARRIER __sync_synchronize()

template <class T>
class LockFreeQueue
{
protected:
    typedef struct
    {
	int m_lock;
	inline void spinlock_init()
	{
	    m_lock = 0;
	}
	inline void spinlock_lock()
	{
	    while(!__sync_bool_compare_and_swap(&m_lock, 0, 1)) {}
	}
	inline void spinlock_unlock()
	{
	    __sync_lock_release(&m_lock);
	}
    } spinlock_t;
public:
    // size:队列大小
    // name:共享内存key的路径名称，默认为NULL，使用数组作为底层缓冲区。
    LockFreeQueue(unsigned int size, const char* name = NULL)
    {
	memset(shm_name, 0, sizeof(shm_name));
	createQueue(name, size);
    }
    ~LockFreeQueue()
    {
	if(shm_name[0] == 0)
	{
	    delete [] m_buffer;
	    m_buffer = NULL;
	}
	else
	{
	    if (munmap(m_buffer, m_size * sizeof(T)) == -1) {
		perror("munmap");
	    }
	    if (shm_unlink(shm_name) == -1) {
		perror("shm_unlink");
	    }
	}
    }
    
    bool isFull()const
    {
#ifdef USE_POT
        return m_head == (m_tail + 1) & (m_size - 1);
#else
	return m_head == (m_tail + 1) % m_size;
#endif
    }

    bool isEmpty()const
    {
	return m_head == m_tail;
    }
    unsigned int front()const
    {
	return m_head;
    }

    unsigned int tail()const
    {
	return m_tail;
    }

    bool push(const T& value)
    {
#ifdef USE_LOCK
        m_spinLock.spinlock_lock();
#endif
        if(isFull())
        {
#ifdef USE_LOCK
            m_spinLock.spinlock_unlock();
#endif
            return false;
        }
        memcpy(m_buffer + m_tail, &value, sizeof(T));
#ifdef USE_MB
        MEMORY_BARRIER;
#endif

#ifdef USE_POT
        m_tail = (m_tail + 1) & (m_size - 1);
#else
        m_tail = (m_tail + 1) % m_size;
#endif

#ifdef USE_LOCK
        m_spinLock.spinlock_unlock();
#endif
        return true;
    }

    bool pop(T& value)
    {
#ifdef USE_LOCK
        m_spinLock.spinlock_lock();
#endif
        if (isEmpty())
        {
#ifdef USE_LOCK
            m_spinLock.spinlock_unlock();
#endif
            return false;
        }
        memcpy(&value, m_buffer + m_head, sizeof(T));
#ifdef USE_MB
        MEMORY_BARRIER;
#endif

#ifdef USE_POT
        m_head = (m_head + 1) & (m_size - 1);
#else
        m_head = (m_head + 1) % m_size;
#endif

#ifdef USE_LOCK
        m_spinLock.spinlock_unlock();
#endif
        return true;
    }
protected:
    virtual void createQueue(const char* name, unsigned int size)
    {
#ifdef USE_POT
        if (!IS_POT(size))
        {
            size = roundup_pow_of_two(size);
        }
#endif
        m_size = size;
	m_head = m_tail = 0;
	if(name == NULL)
	{
	    m_buffer = new T[m_size];
	}
        else
        {
		int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
		if (shm_fd < 0)
		{
		    perror("shm_open");
		}
		if (ftruncate(shm_fd, m_size * sizeof(T)) < 0)
		{
		    perror("ftruncate");
		    close(shm_fd);
		}
		void *addr = mmap(NULL, m_size * sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (addr == MAP_FAILED)
		{
		    perror("mmap");
		    close(shm_fd);
		}
		if (close(shm_fd) == -1)
		{
		    perror("close");
		    exit(1);
		}
		m_buffer = static_cast<T*>(addr);
		memcpy(shm_name, name, SHM_NAME_LEN - 1);
        }
#ifdef USE_LOCK
	spinlock_init(m_lock);
#endif
    }
    inline unsigned int roundup_pow_of_two(size_t size)
    {
	size |= size >> 1;
	size |= size >> 2;
	size |= size >> 4;
	size |= size >> 8;
	size |= size >> 16;
	size |= size >> 32;
	return size + 1;
    }

protected:
    char shm_name[SHM_NAME_LEN];
    volatile unsigned int m_head;
    volatile unsigned int m_tail;
    unsigned int m_size;
#ifdef USE_LOCK
    spinlock_t m_spinLock;
#endif
    T* m_buffer;
};




