#pragma once
#include "Diagnostics/Log.hpp"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
namespace Protos::MT
{
using namespace Ivy;

struct Task
{

    using Function = std::move_only_function<void()>;

    Task() = default;
    template <typename F, typename... Args>
    explicit Task(F&& op, Args&&... args)
        : m_Op([op = std::forward<F>(op), ... args = std::forward<Args>(args)]() mutable { op(args...); })
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
        m_ThreadCount = threadCount;
        m_Threads.resize(threadCount);
        for (auto& thread : m_Threads)
        {
            thread = std::jthread([this] { Worker(); });
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
        if (start >= end)
        {
            return;
        }

        for (int i = start; i < end; i += chunk)
        {
            const unsigned int chunkStart = i;
            const unsigned int chunkEnd = std::min(i + chunk, end);

            Pool.Assign(Task{[chunkStart, chunkEnd, op = std::forward<F>(op)]()
                {
                    for (unsigned int j = chunkStart; j < chunkEnd; ++j)
                    {
                        op(j);
                    }
                }});
        }
    }
};

inline TaskManager Parallel{};
} // namespace Protos::MT
