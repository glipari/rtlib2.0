/*
  In this example, a simple system is simulated, consisting of two
  real-time tasks scheduled by EDF on a single processor.
*/
#include <kernel.hpp>
#include <edfsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <instr.hpp>
#include <taskstat.hpp>

using namespace MetaSim;
using namespace RTSim;

#define NRUNS  10000

int main()
{
    try {
        SIMUL.dbg.enable(_TASK_DBG_LEV);
        SIMUL.dbg.enable(_INSTR_DBG_LEV);
        SIMUL.dbg.enable(_KERNEL_DBG_LEV);

        // TextTrace ttrace("trace.txt");
  
        cout << "Creating Scheduler and kernel" << endl;
        EDFScheduler edfsched;
        RTKernel kern(&edfsched);

	MissCount mc("miss");

        cout << "Creating the first task" << endl;
        PeriodicTask t1(7, 5, 0, "TaskA");

        try {
	    cout << "Inserting code" << endl;
	    t1.insertCode("delay(PDF(c1.txt));");
	    t1.setAbort(false);
	}
	catch (std::string s) {
	    cout << s << endl;
	}
	    
        cout << "Creating the second task" << endl;
        PeriodicTask t2(11, 7, 0, "TaskB"); 

        cout << "Inserting code" << endl;
        t2.insertCode("delay(PDF(c2.txt));");
        t2.setAbort(false);

        cout << "Creating the third task" << endl;
        PeriodicTask t3(13, 10, 0, "TaskC"); 
        cout << "Inserting code" << endl;
        t3.insertCode("delay(PDF(c3.txt));");
        t3.setAbort(false);

        cout << "Setting up traces" << endl;
	
        // new way
        // ttrace.attachToTask(&t1);
        // ttrace.attachToTask(&t2);
        // ttrace.attachToTask(&t3);

	mc.attachToTask(&t1);
	mc.attachToTask(&t2);
	mc.attachToTask(&t3);

        cout << "Adding tasks to schedulers" << endl;

        kern.addTask(t1, "");
        kern.addTask(t2, "");
        kern.addTask(t3, "");
  
        cout << "Ready to run!" << endl;

	int count = 0;
	for (int i=0; i<10000; i++) {
	    // run the simulation for 500 units of time
	    SIMUL.run(1001, 1);
	    
	    cout << "Number of Deadline misses:" << mc.getLastValue() << endl;
	    if (mc.getLastValue() > 0) count ++;
	}
	cout << "Total count = " << count << endl;
	cout << "DM Perc     = " << double(count) / NRUNS * 100 << endl;
	cout << "Corr.       = " << (1 - double(count) / NRUNS) * 100 << endl;
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    } 
    
}
