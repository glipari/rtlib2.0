#ifndef __PWCET_HPP__
#define __PWCET_HPP__

#include <factory.hpp>
#include <exeinstr.hpp>
#include <npreclaiming.hpp>


namespace RTSim {
    class PWCETInstr : public ExecInstr {
        NPReclaimingServer *server;
        Tick currentBudget;
    public:
        PWCETInstr(Task *t, NPReclaimingServer *s, Tick budget, Tick maxbudget);
        
        static Instr *createInstance(const std::vector<std::string> &par);
        
        CLONEABLE(Instr, PWCETInstr)
        
        virtual ~PWCETInstr(); 

        virtual void schedule() throw (InstrExc);  // to be modified to avoid calling cost
        virtual void onEnd();                      // to be modified, to call the server
        virtual void newRun();                     // initialize internal variables ? 
    };
}

#endif
