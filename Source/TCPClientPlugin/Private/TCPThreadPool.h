// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
/**
 *
 */
class TCPThreadPool
{
public:
public:
    TCPThreadPool(size_t threadCount);
    ~TCPThreadPool();

    void EnqueueJob(std::function<void()> job);

private:
    void WorkerThread();

private:
    size_t ThreadNum;
    std::vector<std::thread> Threads;
    std::condition_variable CondVar;

    std::queue<std::function<void()>> JobQ;
    std::mutex MtxJobQ;

    bool bStopAllThread;
};
