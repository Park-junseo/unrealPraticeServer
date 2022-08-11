#include "pch.h"
#include "JobQueue.h"

/*--------------
	JobQueue
--------------*/

void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	// 첫번째 job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		Execute();
	}
}

// 1) 일감이 너무 몰리면, Execute 루프에 걸림
// 렉이 걸릴 거면, 공평하게 렉이 걸리는 편이 나음
// 2) DoAsync 타고 타고 가서 절대 끝나지 않는 상황 발생 (일감이 한 쓰레드한테 몰림)
// 이왕이면 일감이 적당히 배분되게 유도하는 게 나음

void JobQueue::Execute()
{
	while (true)
	{
		xVector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}
	}

}
