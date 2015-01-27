/***************************************************************************
begin                : Mon Nov 3 15:54:58 CEST 2014
copyright            : (C) 2014 Simoncelli Stefano
email                : simoncelli.stefano@hotmail.it
***************************************************************************/

#include "catch.hpp"
#include <metasim.hpp>
#include <factory.hpp>
#include <AVRTask.hpp>
#include <kernel.hpp>
#include <fpsched.hpp>
#include <edfsched.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("AVRTask activate using FP")
{
    	FPScheduler sched;
    	RTKernel kern(&sched);

	AVRTask t1(	M_PI,	0,	M_PI / 4, 
		vector<string>{string("fixed(10);"), string("fixed(6);"), string("fixed(3);") }, 
		vector<double>{2000,4000,6000},
		vector<double>{500,1500,3500},
		"AVRtask1");
 
	AVRTask t2(M_PI, 0, M_PI / 4,
		vector<string>{string("fixed(12);"), string("fixed(6);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask2");

	AVRTask t3(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(13);suspend(1);"), string("fixed(6);"), string("fixed(4);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask3");

    	kern.addTask(t1, "10");
    	kern.addTask(t2, "11");
    	kern.addTask(t3, "12");

    	SIMUL.initSingleRun();

	t1.activate(0, 15);
	
	SIMUL.run_to(9);
	
	t2.activate(0, 19);
	t3.activate(0, 25);

	SIMUL.run_to(10);
	REQUIRE(t1.getExecTime() == 10);
    	REQUIRE(t2.getExecTime() == 0);
    	REQUIRE(t3.getExecTime() == 0);     	

    	SIMUL.run_to(22);
    	REQUIRE(t1.getExecTime() == 10);
    	REQUIRE(t2.getExecTime() == 12);
    	REQUIRE(t3.getExecTime() == 0);    	

    	SIMUL.run_to(35);    
    	REQUIRE(t1.getExecTime() == 10);
    	REQUIRE(t2.getExecTime() == 12);
    	REQUIRE(t3.getExecTime() == 13);       
  
	REQUIRE(t1.getDeadline() == 15);
	REQUIRE(t2.getDeadline() == 28);
	REQUIRE(t3.getDeadline() == 34);  
   
    	SIMUL.endSingleRun(); 
}

TEST_CASE("AVRTask activate using EDF")
{
	EDFScheduler sched;
	RTKernel kern(&sched);

	AVRTask t1(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(10);"), string("fixed(6);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask1");

	AVRTask t2(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(12);"), string("fixed(6);delay(1);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask2");

	AVRTask t3(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(13);"), string("fixed(6);"), string("fixed(4);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask3");

	kern.addTask(t1);
	kern.addTask(t2);
	kern.addTask(t3);

	SIMUL.initSingleRun();

	t1.activate(0, 15);
	t2.activate(1, 19);
	t3.activate(2, 5);

	SIMUL.run_to(4);

	REQUIRE(t1.getExecTime() == 0);
	REQUIRE(t2.getExecTime() == 0);
	REQUIRE(t3.getExecTime() == 4);

	SIMUL.run_to(14);
	REQUIRE(t1.getExecTime() == 10);
	REQUIRE(t2.getExecTime() == 0);
	REQUIRE(t3.getExecTime() == 4);

	SIMUL.run_to(20);
	REQUIRE(t1.getExecTime() == 10);
	REQUIRE(t2.getExecTime() == 6);
	REQUIRE(t3.getExecTime() == 4);

	REQUIRE(t1.getDeadline() == 15);
	REQUIRE(t2.getDeadline() == 19);
	REQUIRE(t3.getDeadline() == 5);

	SIMUL.endSingleRun();
}

TEST_CASE("AVRTask getWCET")
{
	FPScheduler sched;
	RTKernel kern(&sched);

	AVRTask t1(M_PI, 0, M_PI / 4,
		vector<string>{string("fixed(10);"), string("fixed(6);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask1");

	AVRTask t2(M_PI, 0, M_PI / 4,
		vector<string>{string("fixed(12);"), string("fixed(7);"), string("fixed(2);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask2");

	REQUIRE(t1.getWCET(0) == 10);
	REQUIRE(t1.getWCET(1) == 6);
	REQUIRE(t1.getWCET(2) == 3);
	REQUIRE(t2.getWCET(0) == 12);
	REQUIRE(t2.getWCET(1) == 7);
	REQUIRE(t2.getWCET(2) == 2);

    	SIMUL.endSingleRun();
}

TEST_CASE("AVRTask changeStatus()")
{
    	FPScheduler sched;
    	RTKernel kern(&sched);

	AVRTask t1(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(10);"), string("fixed(6);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask1");

	AVRTask t2(M_PI, 0, M_PI / 4,
		vector<string>{string("fixed(12);"), string("fixed(7);"), string("fixed(2);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask2");

	AVRTask t3(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(11);"), string("fixed(8);"), string("fixed(4);delay(1);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask3");

    	kern.addTask(t1, "15");
    	kern.addTask(t2, "10");
	kern.addTask(t3, "18");

    	SIMUL.initSingleRun();

	t1.activate(0, 25);
	t2.activate(1, 8);
	t3.activate(2, 9);

    	SIMUL.run_to(0);

	REQUIRE(t1.getExecTime()==0);
    	REQUIRE(t1.getDeadline()==25); 
    	REQUIRE(t2.getExecTime()==0);
    	REQUIRE(t2.getDeadline()==8);
   
    	SIMUL.run_to(7);
    
    	REQUIRE(t1.getExecTime()==0);
    	REQUIRE(t2.getExecTime()==7);

	t2.changeStatus(M_PI_4, 0, M_PI_2,
		vector < string > {string("fixed(10);"), string("fixed(6);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500});
	
	t2.activate(0,9);
	
	SIMUL.run_to(17);
	REQUIRE(t2.getExecTime() == 10);

	SIMUL.run_to(27);
	REQUIRE(t1.getExecTime() == 10);

	t1.changeStatus(M_PI_4, 0, M_PI_2, 
		vector < string > {string("fixed(12);"), string("fixed(10);delay(1);"), string("fixed(9);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500});

	t1.activate(0, 9);

    	SIMUL.run_to(39);
    	REQUIRE(t1.getExecTime() == 12);
	REQUIRE(t1.getDeadline() == 36);
	REQUIRE(t3.getExecTime() == 0);
	
	SIMUL.run_to(43);
	REQUIRE(t3.getExecTime() == 4);

	SIMUL.endSingleRun();

    
}

TEST_CASE("AVRTask createInstance from factory")
{
	FPScheduler sched;
	RTKernel kern(&sched);

	vector<string> params{ to_string(M_PI), to_string(0), to_string(M_PI_4),
				string("fixed(20);"), string("fixed(12);"), string("fixed(7);delay(1);"),
				string("2000,4000,6000"),
				string("500,1500,3500")};

	auto_ptr<Task> curr = genericFactory<Task>::instance().create("AVRTask",params);
	AVRTask *t1 = (AVRTask*)curr.release();

	REQUIRE(t1->getAngularPhase() == 0);
	REQUIRE(t1->getWCET(0) == 20);
	REQUIRE(t1->getWCET(1) == 12);
	REQUIRE(t1->getWCET(2) == 8);

	AVRTask t2(M_PI, 0, M_PI / 4,
		vector < string > {string("fixed(10);"), string("fixed(6);"), string("fixed(3);") },
		vector<double>{2000, 4000, 6000},
		vector<double>{500, 1500, 3500},
		"AVRtask1");


	kern.addTask(*t1, "15");
	kern.addTask(t2, "10");


	SIMUL.initSingleRun();

	t1->activate(2, 25);
	t2.activate(1, 8);

	SIMUL.run_to(6);
	REQUIRE(t1->getExecTime() == 0);
	REQUIRE(t2.getExecTime() == 6);	

	SIMUL.run_to(13);
	REQUIRE(t1->getExecTime() == 7);
	REQUIRE(t2.getExecTime() == 6);

	SIMUL.endSingleRun();


}
