#include <kernel.hpp>
#include <rmsched.hpp>
#include <rrsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <pollingserver.hpp>

using namespace MetaSim;
using namespace RTSim;

int main()
{
    try {

        JavaTrace jtrace("trace.trc");
        TextTrace ttrace("trace.txt");
  
        // create the scheduler and the kernel
        RMScheduler sched;
        RTKernel kern(&sched);

        PeriodicTask t11(20, 20, 0, "TaskA1");
        t11.insertCode("fixed(3);");
        t11.setAbort(false);

        PeriodicTask t12(30, 30, 0, "TaskA2");
        t12.insertCode("fixed(4);");
        t12.setAbort(false);

        PeriodicTask t2(45, 45, 0, "TaskB"); 
        t2.insertCode("fixed(6);");
        t2.setAbort(false);

        PeriodicTask t3(60, 60, 0, "TaskC"); 
        t3.insertCode("fixed(10);");
        t3.setAbort(false);
	
        t11.setTrace(&jtrace);
        t12.setTrace(&jtrace);
        t2.setTrace(&jtrace);
        t3.setTrace(&jtrace);

        ttrace.attachToTask(&t11);
        ttrace.attachToTask(&t12);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);

        PollingServer serv(4, 10, "server", "FIFOSched");//"RRSched(2);");
        serv.addTask(t11);
        serv.addTask(t12);
        kern.addTask(serv, "");
        
        kern.addTask(t2, "");
        kern.addTask(t3, "");

        // run the simulation for 500 units of time
        SIMUL.dbg.enable(_TASK_DBG_LEV);
        SIMUL.dbg.enable(_KERNEL_DBG_LEV);
        SIMUL.dbg.enable(_SERVER_DBG_LEV);
        SIMUL.dbg.enable(_RR_SCHED_DBG_LEV);
        SIMUL.run(500);
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }        
}
