#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t n = std::thread::hardware_concurrency())
        : stop_(false) {
        for (size_t i = 0; i < n; ++i)
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> job; {
                        // lock scope
                        std::unique_lock lk(mutex_);
                        cv_.wait(lk, [this] { return stop_ || !waiting_q_.empty(); });
                        if (stop_ && waiting_q_.empty()) return;
                        job = std::move(waiting_q_.front());
                        waiting_q_.pop();
                    }
                    job();
                }
            });
    }

    ~ThreadPool() { {
            std::lock_guard lk(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto &w: workers_) w.join();
    }

    template<typename F>
    void enqueue(F &&f) { {
            std::lock_guard lk(mutex_);
            waiting_q_.emplace(std::forward<F>(f));
        }
        cv_.notify_one();
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()> > waiting_q_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_;
};

#endif //THREAD_POOL_H
