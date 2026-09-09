#ifndef _ThreadUtil_h
#define _ThreadUtil_h



#include<mutex>


// 同步屏障：让主线程等待渲染线程执行到某个点
class SyncLock {
    std::mutex mtx;
    std::condition_variable cv;   

    bool suspended = true;
    bool isSuspended = false;

public:
    // 渲染线程调用：暂停自己
    void DoSuspend() {
        std::unique_lock<std::mutex> lock(mtx);

        if (suspended) {
            isSuspended = true;
            cv.notify_all();  // 通知逻辑线程：我已暂停
            cv.wait(lock, [this] { return !suspended; });  // 阻塞等待恢复
            isSuspended = false;
        }
    }

    // 逻辑线程调用：等待渲染线程暂停
    void WaitRemoteSuspend() {
        // 等待渲染线程确认已暂停
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return isSuspended; });
        //渲染线程已暂停，安全执行操作
    }

    // 逻辑线程调用：恢复渲染线程
    void Resume() {
        {
            std::lock_guard lock(mtx);
            suspended = false;
        }
        cv.notify_all();  // 唤醒渲染线程
    }
};


#endif