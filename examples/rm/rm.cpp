/*
 In this example, a simple system is simulated, consisting of two
 real-time tasks scheduled by RM on a single processor.
 */
#include <kernel.hpp>
#include <fpsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>

using namespace MetaSim;
using namespace RTSim;

int main()
{
    TextTrace ttrace("trace.txt");
    SIMUL.dbg.enable("All");
    SIMUL.dbg.setStream("debug.txt");
    
    // create the scheduler and the kernel
    FPScheduler fpsched;
    RTKernel kern(&fpsched);
    
    // creates the two tasks.
    /* t1 has a period of 15, a relative deadline of 15, starts at time 0,
       and is called "taskA" */
    PeriodicTask t1(100, 100, 0, "task0");
    // Creates the pseudoinstructions for the two tasks
    t1.insertCode("fixed(101);");
    t1.killOnMiss(true);

    /* t2 has a period of 20, a relative deadline of 20, starts at time 0,
       and is called "taskB" */
    PeriodicTask t2(20, 20, 0, "task1");
    t2.insertCode("fixed(5);fixed(6);fixed(4);");
    
//    
//    PeriodicTask t3(50, 50, 0, "task2");
//    t3.insertCode("fixed(4);");
//    t3.setTrace(&jtrace);
//    
//    PeriodicTask t4(25, 25, 0, "task3");
//    t4.insertCode("fixed(4);");
//    t4.setTrace(&jtrace);
//    
    ttrace.attachToTask(t1);
    ttrace.attachToTask(t2);
//    ttrace.attachToTask(&t3);
//    ttrace.attachToTask(&t4);
    
    kern.addTask(t1,"1");
    kern.addTask(t2,"5");
//    kern.addTask(t3,"15");
//    kern.addTask(t4,"10");
    
    //    kern.setContextSwitchDelay(2);
    
    try {
        // run the simulation for 500 units of time
        SIMUL.run(500);
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}
