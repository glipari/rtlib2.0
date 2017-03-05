#include "catch.hpp"
#include <rttask.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <fpsched.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("Thresholds", "[thresholds, test1]")
{
    PeriodicTask t1(12, 12, 0, "TaskA");
    t1.insertCode("raise_thres(1);fixed(5);lower_thres()");
    t1.setAbort(false);    

    PeriodicTask t2(20, 20, 2, "TaskB");
    t2.insertCode("raise_thres(1);fixed(7);lower_thres()");
    t2.setAbort(false);

    FPScheduler sched;
    RTKernel kern(&sched);
    
    kern.addTask(t1, "2");
    kern.addTask(t2, "1");

    SIMUL.initSingleRun();

    SIMUL.run_to(2);
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 0);
    SIMUL.run_to(5);
    REQUIRE(t1.getExecTime() == 5);
    REQUIRE(t2.getExecTime() == 0);
    SIMUL.run_to(6);
    REQUIRE(t1.getExecTime() == 5);
    REQUIRE(t2.getExecTime() == 1);
    
    SIMUL.endSingleRun();
}

TEST_CASE("Thresholds 3 tasks", "[thresholds, test2]")
{
    PeriodicTask t1(4, 4, 0, "TaskA");
    t1.insertCode("raise_thres(1); fixed(1); lower_thres()");
    t1.setAbort(false);    

    PeriodicTask t2(6, 6, 0, "TaskB");
    t2.insertCode("raise_thres(1); fixed(2); lower_thres()");
    t2.setAbort(false);

    PeriodicTask t3(7, 7, 0, "TaskC");
    t3.insertCode("raise_thres(1); fixed(3); lower_thres()");
    t3.setAbort(false);

    FPScheduler sched;
    RTKernel kern(&sched);
    
    kern.addTask(t1, "1");
    kern.addTask(t2, "2");
    kern.addTask(t3, "3");

    SIMUL.dbg.enable("All");

    SIMUL.initSingleRun();

    REQUIRE(sched.getPriority(&t1) == 1);

    SIMUL.run_to(1);
    
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 0);
    REQUIRE(sched.getPriority(&t1) == 1);
    REQUIRE(sched.getPriority(&t2) == 2);
        
    SIMUL.run_to(3);
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 2);
    REQUIRE(t3.getExecTime() == 0);
    REQUIRE(sched.getPriority(&t2) == 2);    
    
    SIMUL.run_to(4);
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 2);
    REQUIRE(t3.getExecTime() == 1);
    
    SIMUL.run_to(6);
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 3);
    
    SIMUL.run_to(7);
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 0);
    
    SIMUL.run_to(8);
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 1);
    REQUIRE(t3.getExecTime() == 0);
    
    SIMUL.run_to(9);
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 2);
    REQUIRE(t3.getExecTime() == 0);
    
    SIMUL.run_to(10);
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 2);
    REQUIRE(t3.getExecTime() == 0);

    
    SIMUL.endSingleRun();
}


