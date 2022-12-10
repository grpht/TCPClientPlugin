// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPThreadPool.h"

TCPThreadPool::TCPThreadPool(size_t threadCount)
    : ThreadNum(threadCount)
    , bStopAllThread(false)
{
    for (size_t i = 0; i < ThreadNum; ++i)
    {
        Threads.emplace_back([this]() { this->WorkerThread(); });
    }
}

TCPThreadPool::~TCPThreadPool()
{

    bStopAllThread = true;
    CondVar.notify_all();

    for (auto& thd : Threads)
    {
        thd.join();
    }
}

void TCPThreadPool::EnqueueJob(std::function<void()> job)
{
    if (bStopAllThread)
    {
        throw std::runtime_error("ThreadPool closed");
    }
    {
        std::lock_guard<std::mutex> lock(MtxJobQ);
        JobQ.push(std::move(job));
    }
    CondVar.notify_one();
}

void TCPThreadPool::WorkerThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(MtxJobQ);
        // if false, lock -> unlock and sleep, if true, achieve lock an awake
        CondVar.wait(lock, [this]() { return !JobQ.empty() || bStopAllThread; });
        if (bStopAllThread && JobQ.empty())
        {
            return;
        }

        std::function<void()> job = std::move(JobQ.front());
        JobQ.pop();
        lock.unlock();
        job();
    }
}
