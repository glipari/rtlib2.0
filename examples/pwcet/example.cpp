/*
  In this example, a simple system is simulated, consisting of two
  real-time tasks scheduled by EDF on a single processor.
*/
#include <kernel.hpp>
#include <edfsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <json_trace.hpp>
#include <rttask.hpp>
#include <npreclaiming.hpp>

extern "C"
{
#include <pwcet-public.h>
}

using namespace MetaSim;
using namespace RTSim;

int main()
{
    try {
        TextTrace ttrace("trace.txt");

        pwcet_library_init(); 
        
        EDFScheduler edfsched;
        RTKernel kern(&edfsched);
        
        NPReclaimingServer server("nps", true); 

        PeriodicTask t1(50000, 50000, 0, "sendautopilot");
        t1.insertCode("raise_thres(1);pwcet(nps,15550,50000);lower_thres(1);");

        PeriodicTask t2(60000, 60000, 0, "radir");
        t2.insertCode("raise_thres(1);pwcet(nps,5700,34450);lower_thres(1);");

        PeriodicTask t3(90000, 90000, 0, "checkmega128");
        t3.insertCode("raise_thres(1);pwcet(nps,20000,34450);lower_thres(1);");
        
        PeriodicTask t4(150000, 150000, 0, "testppm");
        t4.insertCode("raise_thres(1);pwcet(nps,30000,33460);lower_thres(1);");

        ttrace.attachToTask(t1);
        ttrace.attachToTask(t2);
        ttrace.attachToTask(t3);
        ttrace.attachToTask(t4);

        kern.addTask(t1);
        kern.addTask(t2);
        kern.addTask(t3);
        kern.addTask(t4);

        DropStat stat;
        stat.attach(server);
        
        cout << "Ready to run!" << endl;
        Tick simul_time = 5000000;
        SIMUL.run(simul_time);

        cout << "----------------------------" << endl;
        cout << "sendautopilot : dropped = " << stat.get_dropped("sendautopilot") << " | instances = " << simul_time / 50000 << endl;
        cout << "radir         : dropped = " << stat.get_dropped("radir") << " | instances = " << simul_time / 60000 << endl;
        cout << "checkmega128  : dropped = " << stat.get_dropped("checkmega128") << " | instances = " << simul_time / 90000 << endl;
        cout << "testppm       : dropped = " << stat.get_dropped("testppm") << " | instances = " << simul_time / 150000 << endl;

    } catch (BaseExc &e) {
        cout << e.what() << endl;
    }
}
