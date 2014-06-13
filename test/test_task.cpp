#include "catch.hpp"

#include <metasim.hpp>
#include <rttask.hpp>
#include <kernel.hpp>
#include <fpsched.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("Task arrival")
{
    FPScheduler sched;
    RTKernel kern(&sched);

    PeriodicTask t1(10, 10, 0, "task 1");
    t1.insertCode("fixed(4);");
    t1.setAbort(false);
    PeriodicTask t2(15, 15, 0, "task 2");
    t2.insertCode("fixed(5);");
    t2.setAbort(false);
    PeriodicTask t3(25, 25, 0, "task 3");
    t3.insertCode("fixed(4);");
    t3.setAbort(false);

    kern.addTask(t1, "10");
    kern.addTask(t2, "11");
    kern.addTask(t3, "12");
    
    SIMUL.initSingleRun();
    SIMUL.run_to(4);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 0);    

    SIMUL.run_to(9);
    
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 0);    

    SIMUL.run_to(10);
    
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 1);    

    SIMUL.run_to(14);
    
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 1);    

    SIMUL.run_to(15);
    
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 2);    

    SIMUL.run_to(20);
    
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);    

    SIMUL.run_to(24);
    
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);    

    SIMUL.run_to(25);
    
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 3);
    REQUIRE(t3.getDeadline() == 25);

    SIMUL.run_to(26);
    
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 0);    
    REQUIRE(t3.getDeadline() == 50);    

    SIMUL.run_to(30);
    
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 4);
    REQUIRE(t1.getDeadline() == 40);
    REQUIRE(t2.getDeadline() == 45);
    REQUIRE(t3.getDeadline() == 50);    

    SIMUL.endSingleRun(); 
}
