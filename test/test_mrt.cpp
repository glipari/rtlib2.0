#include "catch.hpp"
#include <metasim.hpp>
#include <rttask.hpp>
#include <mrtkernel.hpp>
#include <edfsched.hpp>
#include <cbserver.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("multicore")
{
    EDFScheduler sched;
    MRTKernel kern(&sched, 2);

    PeriodicTask t1(10, 10, 0, "task 1");
    t1.insertCode("fixed(4);");
    t1.setAbort(false);
    PeriodicTask t2(15, 15, 0, "task 2");
    t2.insertCode("fixed(5);");
    t2.setAbort(false);
    PeriodicTask t3(25, 25, 0, "task 3");
    t3.insertCode("fixed(4);");
    t3.setAbort(false);

    kern.addTask(t1);
    kern.addTask(t2);
    kern.addTask(t3);
    
    SIMUL.initSingleRun();

    SIMUL.run_to(4);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 4);
    REQUIRE(t3.getExecTime() == 0);

    SIMUL.run_to(5);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t3.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 5);

    SIMUL.run_to(8);

    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t3.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);

    SIMUL.endSingleRun();
}

TEST_CASE("multicore with cbs")
{
    EDFScheduler sched;
    MRTKernel kern(&sched, 2);

    PeriodicTask t1(10, 10, 0, "task 1");
    t1.insertCode("fixed(4);");
    t1.setAbort(false);
    PeriodicTask t2(15, 15, 0, "task 2");
    t2.insertCode("fixed(5);");
    t2.setAbort(false);
    PeriodicTask t3(25, 25, 0, "task 3");
    t3.insertCode("fixed(4);");
    t3.setAbort(false);

    CBServer serv1(4, 10, 10, true,  "server1", "FIFOSched");
    CBServer serv2(5, 15, 15, true,  "server2", "FIFOSched");
    CBServer serv3(2, 12, 12, true,  "server3", "FIFOSched");

    serv1.addTask(t1);
    serv2.addTask(t2);
    serv3.addTask(t3);
    
    kern.addTask(serv1);
    kern.addTask(serv2);
    kern.addTask(serv3);

    SIMUL.initSingleRun();

    SIMUL.run_to(1);
    REQUIRE(t1.getExecTime() == 1);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 1);
    REQUIRE(serv1.get_remaining_budget() == 3);
    REQUIRE(serv1.getDeadline() == 10);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 1);
    REQUIRE(serv3.getDeadline() == 12);

    SIMUL.run_to(2);
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 2);
    REQUIRE(serv1.getDeadline() == 10);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(4);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 2);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 3);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(5);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 3);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 2);
    REQUIRE(serv2.getDeadline() == 15);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(7);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(10);
    REQUIRE(t1.getExecTime() == 0);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(12);
    REQUIRE(t1.getExecTime() == 2);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 2);
    REQUIRE(serv1.get_remaining_budget() == 2);
    REQUIRE(serv1.getDeadline() == 20);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 24);

    SIMUL.run_to(14);
    REQUIRE(t1.getExecTime() == 4);
    REQUIRE(t2.getExecTime() == 5);
    REQUIRE(t3.getExecTime() == 4);
    REQUIRE(serv1.get_remaining_budget() == 4);
    REQUIRE(serv1.getDeadline() == 30);
    REQUIRE(serv2.get_remaining_budget() == 5);
    REQUIRE(serv2.getDeadline() == 30);
    REQUIRE(serv3.get_remaining_budget() == 2);
    REQUIRE(serv3.getDeadline() == 36);
}
