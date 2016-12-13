#include <kernel.hpp>
#include <rmsched.hpp>
//#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <sporadicserver.hpp>

using namespace MetaSim;
using namespace RTSim;

int main()
{
    try {
        TextTrace ttrace("trace.txt");
  
        // create the scheduler and the kernel
        RMScheduler sched;
        RTKernel kern(&sched);

        PeriodicTask t11(10, 10, 0, "TaskA1");
        t11.insertCode("fixed(1);");
        t11.setAbort(false);

        PeriodicTask t2(45, 45, 0, "TaskB"); 
        t2.insertCode("fixed(6);");
        t2.setAbort(false);

        PeriodicTask t3(60, 60, 0, "TaskC"); 
        t3.insertCode("fixed(10);");
        t3.setAbort(false);
	
        ttrace.attachToTask(t11);
        ttrace.attachToTask(t2);
        ttrace.attachToTask(t3);

        cout << "Task created" << endl;

        SporadicServer serv(6, 50, "server", "FIFOSched");

        cout << "Server created" << endl;
        
        serv.addTask(t11);
        kern.addTask(serv, "");

        cout << "Server added " << endl;
        
        kern.addTask(t2, "");
        kern.addTask(t3, "");

        cout << "Tasks added " << endl;
        
        // run the simulation for 500 units of time
        SIMUL.dbg.enable(_TASK_DBG_LEV);
        SIMUL.dbg.enable(_KERNEL_DBG_LEV);
        SIMUL.dbg.enable(_SERVER_DBG_LEV);
        SIMUL.run(500);
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }        
}
