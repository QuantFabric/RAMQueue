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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string>
#include <stdio.h>
#include <mutex>
#include <unordered_map>

template <typename T>
class ConcurrentHashMap
{
    typedef struct
    {
        int m_mutex;
        int lock;
        int unlock;
        inline void spinlock_init()
        {
            m_mutex = 0;
            lock = 0;
            unlock = 1;
        }
        inline void spinlock_lock()
        {
            while (!__sync_bool_compare_and_swap(&m_mutex, lock, 1))
            {
                usleep(100);
            }
        }
        inline void spinlock_unlock()
        {
            __sync_bool_compare_and_swap(&m_mutex, unlock, 0);
        }
    } spinlock_t;

public:
    ConcurrentHashMap(unsigned int size, unsigned int key,
                      const std::unordered_map<std::string, int> &keyIndex)
    {
        m_size = size;
        m_key = key;
        m_keyIndexMap = keyIndex;
        int shmid = shmget(m_key, sizeof(int) + size * sizeof(T), 0666 | IPC_CREAT);
        unsigned char *ptr = reinterpret_cast<unsigned char *>(shmat(shmid, 0, 0));
        m_writeIndex = reinterpret_cast<int *>(ptr);
        *m_writeIndex = -1;
        m_data = reinterpret_cast<T *>(ptr + sizeof(int));

        m_spinLock.spinlock_init();
    }

    void insert(const std::string &key, const T &value)
    {
        int index = m_keyIndexMap[key];
        m_spinLock.spinlock_lock();
        *m_writeIndex = index;
        memcpy(m_data + index, &value, sizeof(T));
        *m_writeIndex = -1;
        m_spinLock.spinlock_unlock();
    }

    bool get(std::string &key, T &value)
    {
        bool ret = false;
        int index = m_keyIndexMap[key];
        if (*m_writeIndex != index)
        {
            memcpy(&value, m_data + index, sizeof(T));
            ret = true;
        }
        return ret;
    }

private:
    unsigned int m_key;
    unsigned int m_size;
    T *m_data;
    int *m_writeIndex;
    std::unordered_map<std::string, int> m_keyIndexMap;
    spinlock_t m_spinLock;
};