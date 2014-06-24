#include "catch.hpp"
#include <rttask.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("CBS algorithm: period ratio")
{
    PeriodicTask t1(13, 13, 0, "TaskA");
    t1.insertCode("fixed(5);");
    t1.setAbort(false);    

    PeriodicTask t2(19, 19, 0, "TaskB");
    t2.insertCode("fixed(7);");
    t2.setAbort(false);

    EDFScheduler sched;
    RTKernel kern(&sched);
    
    CBServer serv1(3, 6, 6, true,  "server1", "FIFOSched");
    serv1.addTask(t1);
    CBServer serv2(4, 9, 9, true,  "server2", "FIFOSched");
    serv2.addTask(t2);

    kern.addTask(serv1);
    kern.addTask(serv2);

    SIMUL.initSingleRun();

    SIMUL.run_to(3);
    REQUIRE(t1.getExecTime() == 3);
    REQUIRE(t2.getExecTime() == 0);
    REQUIRE(serv1.get_remaining_budget() == 3);
    REQUIRE(serv1.getDeadline() == 12);
    REQUIRE(serv2.get_remaining_budget() == 4);
    REQUIRE(serv2.getDeadline() == 9);

    SIMUL.run_to(7);
    REQUIRE(t1.getExecTime() == 3);
    REQUIRE(t2.getExecTime() == 4);
    REQUIRE(serv1.get_remaining_budget() == 3);
    REQUIRE(serv1.getDeadline() == 12);
    REQUIRE(serv2.get_remaining_budget() == 4);
    REQUIRE(serv2.getDeadline() == 18);

    SIMUL.run_to(9);
    REQUIRE(t1.getExecTime() == 5);
    REQUIRE(t2.getExecTime() == 4);
    REQUIRE(serv1.get_remaining_budget() == 1);
    REQUIRE(serv1.getDeadline() == 12);
    REQUIRE(serv2.get_remaining_budget() == 4);
    REQUIRE(serv2.getDeadline() == 18);

    SIMUL.run_to(12);
    REQUIRE(t1.getExecTime() == 5);
    REQUIRE(t2.getExecTime() == 7);
    REQUIRE(serv1.get_remaining_budget() == 1);
    REQUIRE(serv1.getDeadline() == 12);
    REQUIRE(serv2.get_remaining_budget() == 1);
    REQUIRE(serv2.getDeadline() == 18);
    
    SIMUL.endSingleRun();
}


TEST_CASE("CBS algorithm: Original")
{
    PeriodicTask t1(8, 8, 0, "TaskA");
    t1.insertCode("fixed(2);");
    t1.setAbort(false);

    PeriodicTask t2(10, 10, 0, "TaskB");
    t2.insertCode("fixed(3);");
    t2.setAbort(false);

    EDFScheduler sched;
    RTKernel kern(&sched);
    
    CBServer serv(5, 15, 15, true,  "server1", "FIFOSched");
    serv.addTask(t2);

    kern.addTask(t1);
    kern.addTask(serv);

    SECTION("Original") {
	serv.set_policy(CBServer::ORIGINAL);
	SIMUL.initSingleRun();
	
	SIMUL.run_to(3);
	
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 1);
	REQUIRE(serv.get_remaining_budget() == 4);

	SIMUL.run_to(5);
	REQUIRE(t2.getExecTime() == 3);
	REQUIRE(serv.get_remaining_budget() == 2);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(8);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(9);
	REQUIRE(serv.getStatus() == IDLE);
	SIMUL.run_to(10);
	REQUIRE(serv.getStatus() == EXECUTING);
	REQUIRE(serv.getDeadline() == 25);

	SIMUL.endSingleRun();
    }

    SECTION("Reuse dline") {
	serv.set_policy(CBServer::REUSE_DLINE);
	SIMUL.initSingleRun();
	
	SIMUL.run_to(3);
	
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 1);
	REQUIRE(serv.get_remaining_budget() == 4);	

	SIMUL.run_to(5);
	REQUIRE(t2.getExecTime() == 3);
	REQUIRE(serv.get_remaining_budget() == 2);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(8);
	REQUIRE(serv.getStatus() == RELEASING);
	SIMUL.run_to(9);
	REQUIRE(serv.getStatus() == IDLE);
	SIMUL.run_to(10);
	REQUIRE(serv.getStatus() == EXECUTING);
	REQUIRE(serv.getDeadline() == 15);
	REQUIRE(serv.get_remaining_budget() == 1);

	SIMUL.run_to(11);
	REQUIRE(serv.getStatus() == RECHARGING);
	REQUIRE(serv.getDeadline() == 30);
	
	SIMUL.endSingleRun();
    }
}

TEST_CASE("Task with suspension")
{
    PeriodicTask t1(8, 8, 0, "TaskA");
    t1.insertCode("fixed(2); suspend(3); fixed(2);");
    t1.setAbort(false);

    PeriodicTask t2(15, 15, 0, "TaskB");
    t2.insertCode("fixed(5);");
    t2.setAbort(false);

    EDFScheduler sched;
    RTKernel kern(&sched);
    
    CBServer serv(4, 8, 8, true,  "server1", "FIFOSched");
    serv.addTask(t1);

    kern.addTask(t2);
    kern.addTask(serv);

    SECTION("Original") {
	serv.set_policy(CBServer::ORIGINAL);
	SIMUL.initSingleRun();
	
	SIMUL.run_to(2);
	
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 0);

	SIMUL.run_to(5);
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 3);
	REQUIRE(serv.getDeadline() == 13);
	
	SIMUL.endSingleRun();
    }
    SECTION("Reuse deadline") {
	serv.set_policy(CBServer::REUSE_DLINE);
	SIMUL.initSingleRun();
	
	SIMUL.run_to(2);
	
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 0);

	SIMUL.run_to(5);
	REQUIRE(t1.getExecTime() == 2);
	REQUIRE(t2.getExecTime() == 3);
	REQUIRE(serv.getDeadline() == 8);
	REQUIRE(serv.get_remaining_budget() == 1);
	
	SIMUL.endSingleRun();
    }
}
