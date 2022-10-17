#pragma once
#include "Thread/Thread.h"
#include "Types/Action.h"
#include "Types/DMemory.h"

class DRenderThread
{
public:
	DRenderThread();
	virtual ~DRenderThread();

	template<typename... Args>
	void Invoke(DAction<Args...> Action)
	{
		DScopedMutex ScopedMutex(Mutex);
		DSharedPtr<DActionBase> HeapAction = DSharedPtr<DAction<Args...>>(std::move(std::make_shared<DAction<Args...>>(std::move(Action))));
		RenderTasks.Add(HeapAction);
	}

	// Will execute this task before returning. Will not stop execution of tasks if they are already started, but will do this before any other tasks are started. 
	template<typename... Args>
	void InvokeImmediately(DAction<Args...> Action)
	{
		{
			DScopedMutex ScopedMutex(Mutex);
			DSharedPtr<DActionBase> HeapAction = DSharedPtr<DAction<Args...>>(std::move(std::make_shared<DAction<Args...>>(std::move(Action))));
			ImmediateRenderTasks.Add(HeapAction);
		}
		while (ImmediateRenderTasks.Size() > 0)
		{
			DThread::Sleep(1);
		}
	}

	void SyncThreads()
	{
		int WaitFor = ThreadSync + 1;
		Invoke(DAction<int>([=](int a) { ThreadSync++; }));
		while (WaitFor > ThreadSync)
		{
			DThread::Sleep(1);
		}
	}

protected:
	void RunRender();

	bool bMustClose = false;

	DMutexHandle Mutex;
	DUniquePtr<DThread> RenderThread;
	DVector<DSharedPtr<DActionBase>> RenderTasks;
	DVector<DSharedPtr<DActionBase>> ImmediateRenderTasks;
	int ThreadSync = 0;
};