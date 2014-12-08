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


TEST_CASE("Task suspension, one task")
{
    PeriodicTask t1(10, 10, 0);
    t1.insertCode("fixed(2); suspend(4); fixed(2);");
    t1.setAbort(false);

    FPScheduler sched;
    RTKernel kern(&sched);

    kern.addTask(t1, "10");

    SIMUL.initSingleRun();
    SIMUL.run_to(0);

    REQUIRE(t1.getExecTime()==0);
    REQUIRE(t1.getDeadline()==10);
    
    SIMUL.run_to(2);
    
    REQUIRE(t1.getExecTime()==2);

    SIMUL.run_to(6);

    REQUIRE(t1.getExecTime()==2);
    REQUIRE(t1.getDeadline()==10);
    
    SIMUL.run_to(8);
    
    REQUIRE(t1.getExecTime()==4);
    REQUIRE(t1.getDeadline()==10);    

    SIMUL.endSingleRun();
}

TEST_CASE("Task suspension, two tasks")
{
    PeriodicTask t1(10, 10, 0);
    t1.insertCode("fixed(2); suspend(2); fixed(2);");
    t1.setAbort(false);

    PeriodicTask t2(8, 8, 0);
    t2.insertCode("fixed(2); suspend(2); fixed(3);");
    t2.setAbort(false);

    FPScheduler sched;
    RTKernel kern(&sched);

    kern.addTask(t1, "15");
    kern.addTask(t2, "10");

    SIMUL.initSingleRun();
    SIMUL.run_to(0);

    REQUIRE(t1.getExecTime()==0);
    REQUIRE(t1.getDeadline()==10); 
    REQUIRE(t2.getExecTime()==0);
    REQUIRE(t2.getDeadline()==8);
   
    SIMUL.run_to(2);
    
    REQUIRE(t1.getExecTime()==0);
    REQUIRE(t2.getExecTime()==2);

    SIMUL.run_to(4);
    
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 2);

    SIMUL.run_to(6);
    
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 4); 

    SIMUL.run_to(7);
    
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 5); 
    REQUIRE(t1.getDeadline() == 10); 
    REQUIRE(t2.getDeadline() == 8);

    SIMUL.run_to(8);
    
    REQUIRE(t1.getExecTime() == 3);
    REQUIRE(t2.getExecTime() == 0); 
    REQUIRE(t1.getDeadline() == 10); 
    REQUIRE(t2.getDeadline() == 16);

    SIMUL.run_to(10);
    
    REQUIRE(t1.getExecTime() == 3);
    REQUIRE(t2.getExecTime() == 2); 
    REQUIRE(t1.getDeadline() == 10); 
    REQUIRE(t2.getDeadline() == 16);

    SIMUL.run_to(11);
    
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 2); 
    REQUIRE(t1.getDeadline() == 20); 
    REQUIRE(t2.getDeadline() == 16);

    SIMUL.run_to(12);
    
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 2); 
    REQUIRE(t1.getDeadline() == 20); 
    REQUIRE(t2.getDeadline() == 16);

    SIMUL.run_to(15);
    
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 5); 
    REQUIRE(t1.getDeadline() == 20); 
    REQUIRE(t2.getDeadline() == 16);

    SIMUL.endSingleRun();
}
