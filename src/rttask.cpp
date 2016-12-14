#include "rttask.hpp"

namespace RTSim {
    PeriodicTask::PeriodicTask(Tick iat)
        : Task(unique_ptr<RandomVar>(new DeltaVar(iat)), iat, 0, "", 1000), period(iat) 
    {
    }

    PeriodicTask::PeriodicTask(Tick iat, Tick rdl, Tick ph,
                               const std::string &name, long qs)
        : Task(unique_ptr<RandomVar>(new DeltaVar(iat)), rdl, ph, name, qs), period(iat) 
    {
    }

    unique_ptr<PeriodicTask> PeriodicTask::createInstance(const vector<string>& par)
    {
        Tick i = Tick(par[0]);
        Tick d = Tick(par[1]);
        Tick p = Tick(par[2]);
        //const char* n = "";
        string n;
        if (par.size() > 2) n = par[3];
        long q = 100;
        if (par.size() > 4) q = stoi(par[4]);
        
        // @todo what is a? 
        bool a = true;
        if (par.size() > 5 && par[5] == "false") a = false; //!strcmp(par[5].c_str(), "false")) a = false;

        unique_ptr<PeriodicTask> tt(new PeriodicTask(i, d, p, n, q));
        
        return tt;
    }
}
