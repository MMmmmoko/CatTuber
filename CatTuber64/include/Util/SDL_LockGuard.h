#ifndef _SDL_LockGuard_h
#define _SDL_LockGuard_h

#include<SDL3/SDL.h>
//对SDL的Lock添加一层封装以适应C++的异常机制


class SDL_LockGuard {
public:
    explicit SDL_LockGuard(SDL_Mutex* m) : m_mutex(m) {
        SDL_LockMutex(m_mutex);
    }
    ~SDL_LockGuard() {
        SDL_UnlockMutex(m_mutex);
    }
    SDL_LockGuard(const SDL_LockGuard&) = delete;
    SDL_LockGuard& operator=(const SDL_LockGuard&) = delete;

private:
    SDL_Mutex* m_mutex;
};


#endif // !_SDL_LockGuard_h
