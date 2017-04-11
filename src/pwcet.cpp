#include "pwcet.hpp"

#include <strtoken.hpp>

namespace RTSim {
    using namespace MetaSim;
    using namespace std;
    using namespace parse_util;
    
    PWCETInstr::PWCETInstr(Task *t, NPReclaimingServer *s, Tick budget, Tick maxbudget) :
        ExecInstr(t, std::unique_ptr<RandomVar>(new DeltaVar(0))),
        server(s),
        currentBudget(0)
    {
        if (!s->add_task(t, budget, maxbudget))
            throw ParseExc("PWCETInstr", "Task " + t->getName() + " not schedulable");
    }
        
    Instr *PWCETInstr::createInstance(const std::vector<std::string> &par)
    {
        // First parameter: server name
        if (par.size() < 4) throw ParseExc("PWCETInstr", "not enough parameters");
        NPReclaimingServer *serv = dynamic_cast<NPReclaimingServer *>(Entity::_find(par[0]));
        if (serv == nullptr) throw ParseExc("PWCETInstr", "cannot find server " + par[0]);
        
        Tick budget = std::stol(par[1]);
        if (budget == 0) throw ParseExc("PWCETInstr", "Initializing instruction with zero budget");

        Tick maxbudget = std::stol(par[2]);
        if (maxbudget == 0) throw ParseExc("PWCETInstr", "Initializing instruction with zero maxbudget");

        Task *task = dynamic_cast<Task *>(Entity::_find(par[3]));
        if (task == nullptr) throw ParseExc("PWCETInstr", "Cannot find task " + par[3]);

        return new PWCETInstr(task, serv, budget, maxbudget);
    }
        
    PWCETInstr::~PWCETInstr()
    {
    }

    void PWCETInstr::schedule() throw (InstrExc)  // to be modified to avoid calling cost
    {
        Tick t = SIMUL.getTime();
        if (flag) {
            execdTime = 0;
            actTime = 0;
            flag = false;

            cout << SIMUL.getTime() << ": PWCET::schedule() for task " << _father->getName() << endl;
            
            currentCost = server->get_wcet(_father);    // call clement's code
            currentBudget = server->get_budget(_father);

            cout << "   currentCost = " << currentCost << " | currentBudget = " << currentBudget << endl;

            if (currentCost > currentBudget) {
                currentCost = server->get_pwcet_cost(_father); // just count the time to compute the pwcet
                cout << "   currentCost = " << currentCost << endl;
                server->drop_instance(_father);
            } else {
                server->update_state(_father);          // call's Clement's code
            }
        }
        ExecInstr::schedule();
    }

    void PWCETInstr::onEnd()                      // to be modified, to call the server
    {
        ExecInstr::onEnd();
        cout << SIMUL.getTime() << ": PWCETInstr::onEnd() for task " << _father->getName() << endl;
        cout << "   Executed = " << _father->getExecTime() << " | remaining = " << currentBudget - _father->getExecTime() << endl;
        server->end_instance(_father, currentBudget - _father->getExecTime());
    }
    
    void PWCETInstr::newRun()                     // initialize internal variables ?
    {
    }
}
