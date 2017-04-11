#include "npreclaiming.hpp"
#include <cstring>

namespace RTSim {

    void NPReclaimingServer::PWCETDropEvent::set_dropped_task(Task *t)
    {
        task = t;
    }

    Task *NPReclaimingServer::PWCETDropEvent::get_dropped_task()
    {
        return task;
    }
    
    NPReclaimingServer::NPReclaimingServer(const std::string &name, bool flag) :
        Entity(name),
        remaining_budget(0),
        last_update(0),
        drop_evt(this, &NPReclaimingServer::onDrop),
        pwcet_flag(flag)
    {
        current_state = init_state();
    }
    
    bool NPReclaimingServer::add_task(Task *t, Tick budget, Tick maxbudget)
    {
        budget_list[t] = budget;
        maxb_list[t] = maxbudget;        
        return true;
    }
    
    void NPReclaimingServer::end_instance(Task *t, Tick remaining)
    {
        remaining_budget = remaining;
        last_update = SIMUL.getTime();
    }
    
    Tick NPReclaimingServer::get_budget(Task *t)
    {
        Tick r = max(Tick(0), remaining_budget - (SIMUL.getTime() - last_update));
        Tick b = min(budget_list[t] + r, maxb_list[t]);
        remaining_budget = 0;
        return b;
    }

    Tick NPReclaimingServer::get_wcet(Task *t)
    {
        char *taskname = new char[t->getName().size() + 1];
        std::strcpy(taskname, t->getName().c_str());

        Tick wcet = compute_wcet(taskname, current_state) + computation_time(taskname);

        delete taskname;
        
        // call clement's code
        return wcet;
    }

    void NPReclaimingServer::update_state(Task *t)
    {
        char *taskname = new char[t->getName().size() + 1];
        std::strcpy(taskname, t->getName().c_str());
        // call clement's code
        if (pwcet_flag) 
            ::update_state(taskname, current_state);

        delete taskname;
    }

    void NPReclaimingServer::drop_instance(Task *t)
    {
        cout << SIMUL.getTime() << " NPReclaimingServer::drop_instance() for task " << t->getName() << endl;
        drop_evt.set_dropped_task(t);
        drop_evt.process();
    }

    void NPReclaimingServer::onDrop(Event *evt)
    {
        cout << "onDrop called" << endl;
    }

    Tick NPReclaimingServer::get_pwcet_cost(Task *t)
    {
        char *taskname = new char[t->getName().size() + 1];
        std::strcpy(taskname, t->getName().c_str());
        Tick r = computation_time(taskname);
        delete taskname;
        return r;
    }

    void DropStat::probe(NPReclaimingServer::PWCETDropEvent &e)
    {
        record(1);
        drop_count[e.get_dropped_task()->getName()]++;
    }
    
    int DropStat::get_dropped(std::string taskname)
    {
        return drop_count[taskname];
    }

    void DropStat::attach(NPReclaimingServer &s)
    {
        attach_stat(*this, s.drop_evt);
    }

}
