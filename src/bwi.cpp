#include "bwi.hpp"
#include "resource.hpp"

namespace RTSim {
    BWI::BWI(const string &n) :
        ResManager(n)
    {
    }
    
    BWI::~BWI() {}

    void BWI::addServer(Server *serv, Scheduler *sched)
    {
        schedulers[serv] = sched;
    }

    bool BWI::request(AbsRTTask *t, Resource *r, int n)
    {
        bool ret;
        
        if (r->isLocked()) {
            ret = false;
            // find the server that holds the task. 
            AbsKernel *k = t->getKernel();
            // we now assume that it is a server. If it is not, 
            // raise an exception.
            Server *s = dynamic_cast<Server *>(k);
            if (!s) 
                throw BaseExc("BWI operating on something that it is not a server");
            
            if (schedulers.find(s) == schedulers.end()) 
                throw BaseExc("BWI: Can't find server/scheduler correspondence");

            Scheduler *sched = schedulers[s];

            // remove the task from the server scheduler queue
            s->suspend(t);

            // now let's find the lock owner
            AbsRTTask *absowner = r->getOwner();
            // Should be a task! 
            Task *owner = dynamic_cast<Task *>(absowner);
            if (!owner) 
                throw BaseExc("BWI lock holder is not a task!");
            // remove the owner from its own queue
            AbsKernel *k_owner = owner->getKernel();
            k_owner->suspend(owner);
            
            // insert the owner in the server
            // this is difficult! It depends on the scheduler inside the server,
            // which I do not know. 
            // I should call the function with the parameters set to "MAX_PRIO"
            // which has a specific meaning. Then, change all schedulers 

            // 1) Introduce the removeTask() from the scheduler interface
            // 2) Add a specific parameter MAX_PRIO to all schedulers, to mean 
            //    that the task is activated (and equeued) at maximum possible priority
            // 3) Modify the task to store its "scheduling" parameters, so that 
            //    it can bring such parameters when moving from one scheduler to the other.
        }
        else {
            r->lock(t);
            ret = true;
        }

        return ret;
    }

    void BWI::release(AbsRTTask *t, Resource *r, int n)
    {
        return;
    }

}
