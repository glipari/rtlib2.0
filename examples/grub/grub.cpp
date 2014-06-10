#include <kernel.hpp>
#include <edfsched.hpp>
#include <fifosched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <grubserver.hpp>

using namespace MetaSim;
using namespace RTSim;

int main()
{
    try {

        JavaTrace jtrace("trace.trc");
        TextTrace ttrace("trace.txt");
  
        // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);
      
        PeriodicTask t2(8, 4 , 0, "TaskA"); 
        t2.insertCode("fixed(2);");
        t2.setAbort(false);

        PeriodicTask t3(6, 6, 0, "TaskB"); 
        t3.insertCode("fixed(4);");
        t3.setAbort(false);

        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);


	Grub serv1(2, 4, "HIGH", "FIFOSched");
	serv1.addTask(t2);
        kern.addTask(serv1, "");	
	
	Grub serv2(3, 6, "LOW", "FIFOSched");
	serv2.addTask(t3);
        kern.addTask(serv2, "");

	GrubSupervisor super;

	bool flag1 = super.addGrub(&serv1);
	cout << "Server1 added = " << flag1 << endl;

	bool flag2 = super.addGrub(&serv2);
	cout << "Server2 added = " << flag2 << endl;

        SIMUL.dbg.enable(_TASK_DBG_LEV);
        SIMUL.dbg.enable(_KERNEL_DBG_LEV);
        SIMUL.dbg.enable(_SERVER_DBG_LEV);
        SIMUL.run(24);
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }        
}
