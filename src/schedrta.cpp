#include <schedrta.hpp>
#include <cassert>
#include <math.h>
#include <sporadicserver.hpp>
#include <algorithm>

namespace RTSim {

    using namespace std;

    /// Rate-monotonic ordering
    static bool less_rm(const SchedRTA::ServerInfo & s1, const SchedRTA::ServerInfo & s2) {
	return s1.p_server->getPeriod() < s2.p_server->getPeriod();
    }

    void SchedRTA::addServer(Server *s)
    {
        servers.push_back(ServerInfo(s));
        std::sort(servers.begin(), servers.end(), less_rm);
	Tick cc=s->getBudget();
	Tick pp=s->getPeriod();
        DBGVAR(cc); DBGVAR(pp);
	period.push_back(pp);
        wcet.push_back(cc);
        U.push_back(double(cc)/double(pp));

    }

    SchedRTA::SchedRTA(const string &name) : 
        Entity(name)
    {
    }

    SchedRTA::~SchedRTA()
    {
    }

    Tick SchedRTA::computeResponseTime(int i) const
    {
        Tick r_cur;
        Tick r_new = servers[i].Q;
        do {
            r_cur = r_new;
            r_new = servers[i].Q;
            for (int j = 0; j < i; ++j)
                // This is a ceil()
                r_new += (r_cur + servers[j].P - 1) / int(servers[j].P) * servers[j].Q;
        } while(r_new > r_cur && r_new <= servers[i].P);
        return r_new;
    }

    void SchedRTA::updateResponseTimes()
    {
      for (int i = 0; i < (int) servers.size(); ++i){
            servers[i].R = computeResponseTime(i);
	    // DBGVAR(servers[i].R);
      }
    }

    bool SchedRTA::tryBudget(int i, Tick b)
    {
      //      DBGENTER(_SERVER_DBG_LEV);
        
      Tick old_b = servers[i].Q;
        servers[i].Q = b;
	for (int h=0;h<i+1;h++)
	  {
	    DBGVAR(servers[h].Q);
	    DBGVAR(servers[h].P);
	    DBGVAR(U[h]);
	  }
        bool schedules = computeResponseTime(i) <= servers[i].P;
        for (int j = i + 1; schedules && j < (int)servers.size(); ++j)
	  {
	    schedules = computeResponseTime(j) <= servers[j].P;
	    DBGVAR(j);
	    DBGVAR(computeResponseTime(j));
	    DBGVAR(servers[j].Q);
	    DBGVAR(servers[j].P);
	    
	  }
	    servers[i].Q = old_b;
	    DBGVAR(schedules);
	    DBGVAR(b);
        return schedules;
    }

    Tick SchedRTA::searchBudget(int i, Tick b1, Tick b2)
    {
        if (b1 == b2)
            return b1;

        if (b2 == b1 + 1) {
            if (tryBudget(i, b2))
                return b2;
            else
                return b1;
        }

        Tick b = (b1 + b2) / 2;
        if (tryBudget(i, b))
            return searchBudget(i, b, b2);
        else
            return searchBudget(i, b1, b);
    }

    Tick SchedRTA::searchBudget(int i)
    {
      //        DBGENTER(_SERVER_DBG_LEV);
      Tick budget= searchBudget(i, servers[i].Q, servers[i].P);
	DBGVAR(budget);
	return budget;
    }

    Tick SchedRTA::changeBudget(Server *s, Tick db)
    {
        int i;
        for (i = 0; i < (int)servers.size(); ++i)
            if (servers[i].p_server == s)
                break;
        assert(i < (int)servers.size());
        Tick cur_b = servers[i].Q;
        Tick new_b = cur_b + db;
        Tick max_b = searchBudget(i);
        if (new_b > max_b)
            new_b = max_b;
        servers[i].p_server->changeBudget(new_b);
        servers[i].Q = new_b;
        return new_b - cur_b;
    }

//     void SchedRTA::onChangeBudget(ChangeBudgetEvt *e)
//     {
//         DBGPRINT("SchedRTA::onChangeBudget(ChangeBudgetEvt *e)");
//         Server *ss = e->getServer();
//         ss->changeBudget(e->getBudget());
//     }

    void SchedRTA::updateU(int task,Tick req)
    {
        wcet[task]=req;
        U[task]=double (double(wcet[task])/double(period[task]));
	servers[task].Q=req;
    }

    void SchedRTA::newRun()
    {
    }

    void SchedRTA::endRun()
    {
    }
  
}
