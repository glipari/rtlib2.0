#ifndef __NP_RECLAIMING_SERVER_HPP__
#define __NP_RECLAIMING_SERVER_HPP__

#include <vector>
#include <map>
#include <simul.hpp>
#include <task.hpp>

extern "C"
{
#include <pwcet-public.h>
}



namespace RTSim {
    
    class NPReclaimingServer : public MetaSim::Entity {
        /// budget for each task
        std::map<Task *, Tick> budget_list;
        /// max allowed budget for every task
        std::map<Task *, Tick> maxb_list; 
        Tick remaining_budget;
        Tick last_update;
        state_t *current_state;

        bool pwcet_flag;
        
    public:

        class PWCETDropEvent : public MetaSim::GEvent<NPReclaimingServer> {
            Task *task;
        public:
            using MetaSim::GEvent<NPReclaimingServer>::GEvent;
            void set_dropped_task(Task *t);
            Task *get_dropped_task();
        };

        PWCETDropEvent drop_evt;
        
        NPReclaimingServer(const std::string &name, bool flag);
        
        bool add_task(Task *t, Tick budget, Tick maxbudget); 
        void end_instance(Task *t, Tick remaining); // end of instance
        Tick get_budget(Task *t); // returns the budget for the currently executing task

        Tick get_wcet(Task *t); // call clement's code
        void update_state(Task *t); // call clement's code 
        Tick get_pwcet_cost(Task *t); // call clement's code

        void drop_instance(Task *t); // signal the fact the an instance has been dropped

        void onDrop(MetaSim::Event *e);
        
        void newRun() { remaining_budget = 0; }
        void endRun() {}
    };

    class DropStat : public StatCount {
        std::map<std::string, int> drop_count;
    public:
        void probe(NPReclaimingServer::PWCETDropEvent &e);
        int get_dropped(std::string taskname);
        void attach(NPReclaimingServer &s);
    };
    
}


#endif
