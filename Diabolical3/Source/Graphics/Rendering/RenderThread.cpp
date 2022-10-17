#include "RenderThread.h"
#include "Logging/Logging.h"

DRenderThread::DRenderThread()
	: RenderThread(std::make_unique<DThread>([=]() { RunRender(); }))
{

}

DRenderThread::~DRenderThread()
{
	bMustClose = true;
	RenderThread->Join();
}

void DRenderThread::RunRender()
{
	while (!bMustClose)
	{
		{
			DScopedMutex ScopedMutex(Mutex);
			for (DSharedPtr<DActionBase>& Task : ImmediateRenderTasks)
			{
				Task->Run();
			}
			ImmediateRenderTasks.Clear();
		}

		{
			DVector<DSharedPtr<DActionBase>> MovedTasks;
			{
				DScopedMutex ScopedMutex(Mutex);

				// Steal RenderTasks and reinitialize it. Faster than copying.
				MovedTasks = std::move(RenderTasks);
				RenderTasks = DVector<DSharedPtr<DActionBase>>();
			}

			for (DSharedPtr<DActionBase>& Task : MovedTasks)
			{
				Task->Run();
			}
			MovedTasks.Clear();
		}

		DThread::Sleep(1);
	}
}