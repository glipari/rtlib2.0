#include <kernel.hpp>
#include <edfsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <json_trace.hpp>
#include <rttask.hpp>
#include <npreclaiming.hpp>
#include <taskstat.hpp>

#include <cstdlib>
#include <unistd.h>
#include <iomanip> 

extern "C"
{
#include <pwcet-public.h>
}

using namespace MetaSim;
using namespace RTSim;

using namespace std;

struct TaskStruct {
    std::string name;
    long min = 0;
    long max = 0;
    long period = 0;
    long deadline = 0;
    long budget = 0;
    long maxbudget = 0;
    long blocking_budget = 0;
    long blocking_max = 0;
    MissCount mc;
};

std::vector<TaskStruct> taskdata;
    
void parse_file(char *filename)
{
    //cout << "Opening file " << filename << endl;
    std::ifstream myfile(filename);

    string line;
    std::getline(myfile, line);
    while (std::getline(myfile, line)) {
        TaskStruct ts;
        //cout << "read: " << line << endl;
        stringstream ss(line);
        string token;
        std::getline(ss, token, ';');
        std::getline(ss, token, ';');
        cout << "task name : " << token << endl;
        if (token != "") {
            ts.name = token;
            std::getline(ss, token, ';');
            cout << "min       : " << std::stoi(token) << endl;
            ts.min = std::stoi(token);
            std::getline(ss, token, ';');
            cout << "max       : " << std::stoi(token) << endl;
            ts.max = std::stoi(token);
            std::getline(ss, token, ';');
            cout << "period    : " << std::stoi(token) << endl;
            ts.period = std::stoi(token);
            ts.deadline = std::stoi(token);
            std::getline(ss, token, ';');
            cout << "umax      : " << token << endl;
            std::getline(ss, token, ';');
            cout << "budget    : " << std::stoi(token) << endl;
            ts.budget = std::stoi(token);
            std::getline(ss, token, ';');
            cout << "maxbudget : " << std::stoi(token) << endl;
            ts.maxbudget = std::stoi(token);

            taskdata.push_back(ts);
        }
    }
}

set<long> enumerate_deadlines(vector<TaskStruct> &taskdata, long limit)
{
    set<long> deadlines;
    for (auto t : taskdata) {
        long p = 0;
        while (p <= limit) {
            if (p+t.deadline <= limit) deadlines.insert(p + t.deadline);
            p += t.period;
        }
    }
    for (auto d : deadlines) cout << d << ", ";
    cout << endl;
    return deadlines;
}

void compute_blocking_budget(vector<TaskStruct> &tdata)
{
    for (auto t : tdata) {
        t.blocking_budget = 0;
        for (auto k : tdata) {
            if (k.deadline > t.deadline) t.blocking_budget = std::max(t.blocking_budget, t.budget);
        }
    }
}

void compute_blocking_max(vector<TaskStruct> &tdata)
{
    for (auto t : tdata) {
        t.blocking_max = 0;
        for (auto k : tdata) {
            if (k.deadline > t.deadline) t.blocking_max = std::max(t.blocking_max, t.max);
        }
    }
}



bool sched_analysis(vector<TaskStruct> &taskdata)
{
    sort(taskdata.begin(), taskdata.end(), [](TaskStruct a, TaskStruct b) {
            return a.period < b.period;
        });

    compute_blocking_budget(taskdata);
    compute_blocking_max(taskdata);
    
    for (int i = 0; i<taskdata.size(); i++) {
        TaskStruct &t = taskdata[i];
        cout << "Checking " << t.name  << " (deadline = " << t.deadline << ")" << endl;
        auto dlines = enumerate_deadlines(taskdata, t.deadline);
        for (auto d : dlines) {
            long demand = t.blocking_budget;
            for (int j=0; j<=i; j++) {
                int f = std::floor(double(d-taskdata[j].deadline)/double(taskdata[j].period))+1;
                //cout << "Floor : " << f << endl;
                demand += taskdata[j].budget * std::max(0, f);
            }
            cout << "Demand(" << d << ") : " << demand << endl;
            if (demand>d) return false;
        }
    }
    return true;
}

void print_usage(const char *progname)
{
    cout << "Usage: " << progname << " [-polt] filename " << endl;
    cout << "Options: " << endl;
    cout << "            -p : disables pwcet control" << endl;
    cout << "   -o filename : output file name" << endl;
    cout << "     -l length : simulation length" << endl;
    cout << "     -t length : transitory length" << endl;
}

int main(int argc, char *argv[])
{
    int c;
    bool pwcet_flag = true;
    string outfilename = "results.txt";
    Tick transitory = 0;
    Tick simul_time = 5000000;

    if (argc < 2) {
        print_usage(argv[0]);
        exit(-1);
    }
    
    while ((c = getopt (argc, argv, "po:l:t:")) != -1) {
        switch(c) {
        case 'p' :
            cout << "PWCET disabled" << endl;
            pwcet_flag = false;
            break;
        case 'o' :
            cout << "Output file name = " << optarg << endl;
            outfilename = string(optarg);
            break;
        case 't' :
            transitory = atoi(optarg);
            cout << "Transitory = " << transitory << endl;
            break;
        case 'l' :
            simul_time = atoi(optarg);
            cout << "Simulation time = " << simul_time << endl;
            break;
        case '?' :
            print_usage(argv[0]);
            exit(-1);
        }
    }

    TextTrace ttrace("trace.txt");
    
    pwcet_library_init(); 
    
    EDFScheduler edfsched;
    RTKernel kern(&edfsched);
    
    NPReclaimingServer server("nps", pwcet_flag); 
    
    parse_file(argv[optind]);

    cout << "Schedulable : " << sched_analysis(taskdata) << endl;
    
    vector< unique_ptr<PeriodicTask> > tasks;
    
    for (auto x : taskdata) {
        unique_ptr<PeriodicTask> task(new PeriodicTask(x.period, x.period, 0, x.name));
        stringstream code;
        code << "raise_thres(1); pwcet(nps," << x.budget << "," << x.maxbudget << ");lower_thres(1);";
        task->insertCode(code.str());
        ttrace.attachToTask(*task);
        kern.addTask(*task);
        x.mc.attachToTask(task.get());
        tasks.push_back(move(task));
    }
    
    DropStat stat;
    stat.attach(server);    
    
    BaseStat::setTransitory(transitory);

    
    SIMUL.run(simul_time);
    
    cout << "----------------------------" << endl;

    ofstream out(outfilename);
    
    for (auto x : taskdata)
        out << left <<  setw(15) << x.name
            << ": dropped = " << setw(3) << stat.get_dropped(x.name)
            << " | instances = " << setw(3) << simul_time / x.period
            << " | miss = " << setw(3) << x.mc.getValue() << endl;

    out.close();
}
