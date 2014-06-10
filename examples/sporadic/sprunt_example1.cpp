#include <kernel.hpp>
#include <rmsched.hpp>
#include <rrsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <sporadicserver.hpp>

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

//         PeriodicTask t11(20, 20, 0, "TaskA1");
//         t11.insertCode("fixed(2);");
//         t11.setAbort(false);

        PeriodicTask t1(10, 10, 0, "Task1"); 
        t1.insertCode("fixed(2);");
        t1.setAbort(false);

        PeriodicTask t2(28, 28, 0, "Task2"); 
        t2.insertCode("fixed(12);");
        t2.setAbort(false);


        PeriodicTask a1(1000, 1000, 9, "Aper1");
        a1.insertCode("fixed(2);");

        PeriodicTask a2(1000, 1000, 16, "Aper2");
        a2.insertCode("fixed(2);");
	
        t1.setTrace(&jtrace);
        t2.setTrace(&jtrace);
        a1.setTrace(&jtrace);
        a2.setTrace(&jtrace);

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&a1);
        ttrace.attachToTask(&a2);

        SporadicServer serv(5, 20, "server", "FIFOSched");//"RRSched(2);");
        serv.addTask(a1);
        serv.addTask(a2);

        kern.addTask(t1, "");
        kern.addTask(serv, "");        
        kern.addTask(t2, "");

        // run the simulation for 500 units of time
        SIMUL.dbg.enable(_TASK_DBG_LEV);
        SIMUL.dbg.enable(_KERNEL_DBG_LEV);
        SIMUL.dbg.enable(_SERVER_DBG_LEV);
        SIMUL.dbg.enable(_RR_SCHED_DBG_LEV);
        SIMUL.run(50);
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }        
}
