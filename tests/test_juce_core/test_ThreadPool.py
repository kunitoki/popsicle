import os

import popsicle as juce

#==================================================================================================

class ThreadPoolJob(juce.ThreadPoolJob):
    runCount = 0

    def runJob(self):
        self.runCount += 1
        return juce.ThreadPoolJob.JobStatus.jobHasFinished

#==================================================================================================

def test_default_constructor():
    pool = juce.ThreadPool()
    assert pool.getNumThreads() == os.cpu_count()

#==================================================================================================

def test_threads_constructor():
    pool = juce.ThreadPool(4)
    assert pool.getNumThreads() == 4

    job1 = ThreadPoolJob("abc")
    pool.addJob(job1)

    job2 = ThreadPoolJob("123")
    pool.addJob(job2)

    assert pool.getNamesOfAllJobs(False) == ["abc", "123"]

    pool.moveJobToFront(job2)
    assert pool.getNamesOfAllJobs(False) == ["123", "abc"]

    pool.removeAllJobs(False, 1000)

    assert job1.runCount == 1
    assert job2.runCount == 1
