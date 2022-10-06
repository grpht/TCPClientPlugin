// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount)
    : ThreadNum(threadCount)
    , bStopAllThread(false)
{
    for (size_t i = 0; i < ThreadNum; ++i)
    {
        Threads.emplace_back([this]() { this->WorkerThread(); });
    }
}

ThreadPool::~ThreadPool()
{

    bStopAllThread = true;
    CondVar.notify_all();

    for (auto& thd : Threads)
    {
        thd.join();
    }
}

void ThreadPool::EnqueueJob(std::function<void()> job)
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

void ThreadPool::WorkerThread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(MtxJobQ);
        //���ǽ��� false �϶� lock�� unlock �ϰ� sleep, true�� �� lock�� ��� �����. 
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
