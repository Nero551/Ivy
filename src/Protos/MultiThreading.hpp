#pragma once
#include "Diagnostics/Log.hpp"
#include "Utilities/stl.hpp"
#include <algorithm>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
namespace Protos::MT
{
using namespace Ivy;

enum class Execution
{
    Sequential,
    Parallel
};

struct Task
{

    using Function = std::move_only_function<void()>;

    Task() = default;
    template <typename F, typename... Args>
    explicit Task(F&& op, Args&&... args)
        : m_Op([op = std::forward<F>(op), ... args = std::forward<Args>(args)]() mutable
              { std::invoke(op, args...); })
    {
    }

    void Run()
    {
        m_Op();
    }

  private:
    Function m_Op;
};

struct ThreadPool
{
    ThreadPool() = default;
    ThreadPool(unsigned int threadCount)
    {
        Initialize(threadCount);
    }

    ~ThreadPool()
    {
        {
            std::scoped_lock lock(m_Mutex);
            m_Stopping = true;
        }

        m_CV.notify_all();

        for (auto& thread : m_Threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) noexcept = delete;
    ThreadPool& operator=(ThreadPool&&) noexcept = delete;

    unsigned int GetThreadCount() const
    {
        return m_ThreadCount;
    }

    void Initialize(unsigned int threadCount)
    {
        if (!m_Threads.empty())
        {
            D::Log::Error("ThreadPool Already Initialized");
        }

        if (threadCount == 0)
        {
            D::Log::Error("ThreadPool Requires At Least One Worker");
        }

        m_ThreadCount = threadCount;
        m_Threads.reserve(threadCount);

        for (unsigned int i = 0; i < threadCount; ++i)
        {
            m_Threads.emplace_back([this] { Worker(); });
        }
    }

    void Assign(Task&& task)
    {
        {
            std::scoped_lock<std::mutex> lock{m_Mutex};

            if (m_Stopping)
            {
                D::Log::Error("ThreadPool Is Stopping");
            }

            m_Tasks.emplace(std::move(task));
        }
        m_CV.notify_one();
    }

    template <typename... Tasks>
    void Assign(Execution Mode, Tasks&&... tasks) requires(std::same_as<Tasks, Task> && ...)
    {
        {
            std::scoped_lock<std::mutex> lock{m_Mutex};

            if (m_Stopping)
            {
                D::Log::Error("ThreadPool Is Stopping");
            }

            if (Mode == Execution::Sequential)
            {
                m_Tasks.emplace([... tasks = std::move(tasks)]() mutable { (tasks.Run(), ...); });
            }
            else
            {
                (m_Tasks.emplace(std::move(tasks)), ...);
            }
        }

        m_CV.notify_all();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock{m_Mutex};
        m_WaitCV.wait(lock, [this]() { return m_Tasks.empty() && m_ActiveTasks == 0; });
    }

  private:
    unsigned int m_ThreadCount = 0;
    std::vector<std::jthread> m_Threads;
    std::queue<Task> m_Tasks;
    std::mutex m_Mutex;
    std::condition_variable m_CV;
    std::condition_variable m_WaitCV;
    bool m_Stopping = false;
    unsigned int m_ActiveTasks = 0;

    void Worker()
    {
        while (true)
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_Mutex);

                m_CV.wait(lock, [this]() { return m_Stopping || !m_Tasks.empty(); });

                if (m_Stopping && m_Tasks.empty())
                {
                    return;
                }

                task = std::move(m_Tasks.front());
                m_Tasks.pop();
                ++m_ActiveTasks;
            }
            task.Run();

            {
                std::scoped_lock<std::mutex> lock{m_Mutex};
                --m_ActiveTasks;
            }
            m_WaitCV.notify_all();
        }
    }
};

struct TaskManager
{
    ThreadPool Pool{};

    void Initialize(unsigned int threadCount)
    {
        Pool.Initialize(threadCount);
    }

    template <typename F> void For(int start, int end, F&& op, int chunk = 1024)
    {
        if (start >= end || chunk <= 0)
        {
            return;
        }

        std::decay_t<F> operation = std::forward<F>(op);

        for (int i = start; i < end; i += chunk)
        {
            const unsigned int chunkStart = i;
            const unsigned int chunkEnd = std::min(i + chunk, end);

            Pool.Assign(Task{[chunkStart, chunkEnd, operation]()
                {
                    for (unsigned int j = chunkStart; j < chunkEnd; ++j)
                    {
                        operation(j);
                    }
                }});
        }
    }
};

inline TaskManager Parallel{};

void Test()
{
    auto start = std::chrono::high_resolution_clock::now();
    int s = 0;
    int e = 2000;
    int counter = 0;
    std::condition_variable cv;
    std::mutex mutex;

    MT::Parallel.Initialize(8);

    stl::function<void(int, int)> op = [&mutex, &counter](int s, int e)
    {
        for (int i = s; i < e; ++i)
        {
            std::scoped_lock<std::mutex> lock{mutex};
            ++counter;
        }
    };

    // while (true)
    // {
    MT::Parallel.Pool.Assign(MT::Execution::Sequential, MT::Task{op, s, e}, MT::Task{op, s, e},
        MT::Task{op, s, e}, MT::Task{op, s, e});

    // MT::Parallel.For(s, e, [](int j) { j++; });
    // }

    MT::Parallel.Pool.Wait();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    D::Log::Info(duration.count() / 1000000.0f);
    D::Log::Info(counter);
}
} // namespace Protos::MT
