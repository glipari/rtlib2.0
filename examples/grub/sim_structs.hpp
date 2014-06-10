#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <kernel.hpp>
#include <edfsched.hpp>
#include <fifosched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <grubserver.hpp>
#include <load.hpp>
#include <taskstat.hpp>

using namespace MetaSim;
using namespace RTSim;

using namespace std;


struct Stats {
    MissCount miss_count;
    MissPercentage miss_perc;
    TardinessStat<StatMean> tard;

    void dump(double par, const std::string &filename) {
	string fname = filename + "_miss_count.txt";
	ofstream m;
	m.open(fname.c_str(), ios::app);
	m << par << ", " << miss_count.getMean() << ", " 
	  << miss_count.getConfInterval() << endl; 
	m.close();

	fname = filename + "_miss_perc.txt";
	ofstream p;
	p.open(fname.c_str(), ios::app);
	p << par << ", " << miss_perc.getMean() << ", " 
	  << miss_perc.getConfInterval() << endl; 
	p.close();

	fname = filename + "_tard.txt";
	ofstream t;
	t.open(fname.c_str(), ios::app);
	t << par << ", " << tard.getMean() << ", " 
	  << tard.getConfInterval() << endl; 
	t.close();
    }
    void newStatRun() {
	miss_count.newRun();
	miss_perc.newRun();
	tard.newRun();    
    }
};

class MySim {
public:
    EDFScheduler sched;
    RTKernel kern;
    GrubSupervisor super;
    TextTrace ttrace;
    
    vector<Grub *> servers;
    Grub *LowServer; 
    double hl_ratio;
    double HighUtil;
    double HighLowUtil;
    double LowUtil;
    double upper_limit;

    MySim(double u, double hl, double ul=0.99) :
	sched(),
	kern(&sched),
	super(),
	ttrace("trace.txt"),
	servers(),
	LowServer(0),
	hl_ratio(hl),
	HighUtil(u),
	upper_limit(ul)
	{   
	    HighLowUtil = HighUtil * hl_ratio;
	    LowUtil = upper_limit - HighLowUtil;
	}

    void add_to_sim(Grub *s, Task &task) {
	s->addTask(task);
	servers.push_back(s);
	bool flag = super.addGrub(s);
	cout << "Grub added: " << flag << endl;
	kern.addTask(*s, "");
	ttrace.attachToTask(&task);
    }    

    void create_high_server(Task &task) {
	Grub *s = new Grub(task.getWCET(), 
			   Tick(double(task.getMinIAT()) * hl_ratio), 
			   "", 
			   "FIFOSched");
	cout << "Created a server with period : " 
	     << Tick(double(task.getMinIAT()) * hl_ratio) << endl;
	add_to_sim(s, task);
    }

    void create_low_server(Task &task, double lu) {
	double ratio = (upper_limit - HighUtil)/lu;
	if (ratio > 1) ratio = 1;
	Grub *s = new Grub(Tick(double(task.getWCET())*ratio), task.getMinIAT(), "", "FIFOSched");    
	cout << "Created a LOW server with budget : " 
	     << Tick(double(task.getWCET())*ratio) << endl;
	add_to_sim(s, task);
    }
    
    void create_single_server(Task &task, Tick period) {
	if (LowServer == 0) {
	    Tick cap = Tick(double(period) * (upper_limit - HighUtil));
	    LowServer = new Grub(cap, period, "", "EDFSched");
	    bool flag = super.addGrub(LowServer);
	    cout << "Single Grub added: " << flag << endl;
	    kern.addTask(*LowServer, "");
	    servers.push_back(LowServer);
	}
	LowServer->addTask(task);
	ttrace.attachToTask(&task);	
    }

    ~MySim() {
	cleanVector(servers);
    }


};
