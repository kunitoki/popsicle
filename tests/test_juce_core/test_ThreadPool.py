import os

import popsicle as juce

#==================================================================================================

class ThreadPoolJob(juce.ThreadPoolJob):
    runCount = 0
    maxRuns = 1

    def __init__(self, name, maxRuns):
        super().__init__(name)
        self.maxRuns = maxRuns

    def runJob(self):
        self.runCount += 1

        if self.runCount >= self.maxRuns:
            return juce.ThreadPoolJob.JobStatus.jobHasFinished
        else:
            return juce.ThreadPoolJob.JobStatus.jobNeedsRunningAgain

#==================================================================================================

def test_default_constructor():
    pool = juce.ThreadPool()
    assert pool.getNumThreads() == os.cpu_count()

#==================================================================================================

def test_threads_constructor():
    pool = juce.ThreadPool(4)
    assert pool.getNumThreads() == 4

    job1 = ThreadPoolJob("abc", 1)
    pool.addJob(job1)

    job2 = ThreadPoolJob("123", 5)
    pool.addJob(job2)

    assert pool.getNamesOfAllJobs(False) == ["abc", "123"]

    pool.moveJobToFront(job2)
    pool.moveJobToFront(job2)
    pool.moveJobToFront(job1)
    pool.removeAllJobs(False, 1000)

    #assert job1.runCount == 1
    #assert job2.runCount == 5
