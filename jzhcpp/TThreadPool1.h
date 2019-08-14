#ifndef TTHREADPOOL_H
#define TTHREADPOOL_H

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>
#include <queue>
#include <functional>
#include <future>

namespace TP{

    class TthreadPool
    {
    public:
        ~TthreadPool() {
            ShutDown();
            mCVvar.notify_all();
            
            // 等待所有线程结束
            for (auto& t : mPool) {
                if (t.joinable())
                    t.join();
            }
        }

        void ShutDown() {
            // 将atomic变量设置true， memory_order_release和memory_order_acquire一起使用
            // store memory_order_acquire后load memory_order_release一定能得到store的值
            mStop.store(true, std::memory_order_release);
        }
        
        TthreadPool(int n = 0) : mStop(false) {
            int nthreads = n;
            if (nthreads <= 0)
            {
                nthreads = std::thread::hardware_concurrency();
                nthreads = (nthreads == 0 ? 2 : nthreads);
            }

            for (int i = 0; i != nthreads; ++i) {
                mPool.emplace_back(std::thread([this](){

                    // 获取stop值， 停止donothing    注意不要写成if否则只创建nthreads多，后续不会继续创建
                    while (!mStop.load(std::memory_order_acquire)) {

                        // 和条件变量一起使用
                        std::unique_lock<std::mutex> ulk(this->mMutex);

                        // 阻塞等待直到stop为true或者任务为空
                        mCVvar.wait(ulk, [this](){return this->mStop.load(std::memory_order_acquire) ||
                            !this->mTasks.empty();
                        });

                        // 如果stop为true || 任务为空donothing
                        if (mStop.load(std::memory_order_acquire) || mTasks.empty()) 
                            return;

                        // 执行任务
                        Task task = std::move(mTasks.front());
                        mTasks.pop();
                        task();
                    }
                }));
            }
        }
        
        template<class fun, class... args>
        std::future<typename std::result_of<fun(args...)>::type> Add(fun&& f, args&&... arg) {
            // typedef 返回值类型
            typedef typename std::result_of<fun(args...)>::type returntype;
            typedef std::packaged_task<returntype()> task;

            // 使用智能指针， 获取future
            // std::forward<T>(u) 有两个参数：T 与 u。当T为左值引用类型时，u将被转换为T类型的左值，否则u将被转换为T类型右值。
            // 如此定义std::forward是为了在使用右值引用参数的函数模板中解决参数的完美转发问题
            auto t = std::make_shared<task>(std::bind(std::forward<fun>(f), std::forward<args>(arg)...));
            auto ret = t->get_future();

            // 加锁
            std::lock_guard<std::mutex> ulk(mMutex);

            // 线程池停止则抛出异常
            if (mStop.load(std::memory_order_acquire))
                throw std::runtime_error("thread pool has stopped");

            // 插入队列 捕捉t，执行t
            mTasks.emplace([t]{(*t)();});

            // 通知线程
            mCVvar.notify_one();

            return ret;
        }

    private:
        TthreadPool(const TthreadPool&) =               delete;
        TthreadPool(TthreadPool&&) =                    delete;
        TthreadPool& operator=(const TthreadPool&) =    delete;
        TthreadPool& operator=(TthreadPool&&) =         delete;

    private:
        using Task = std::function<void()>;


        std::vector<std::thread>    mPool;
        std::queue<Task>            mTasks;
        std::mutex                  mMutex;
        std::atomic<bool>           mStop;
        std::condition_variable     mCVvar;
    };
}


#endif // TTHREADPOOL_H