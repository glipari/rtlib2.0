#include "suspend_instr.hpp"

namespace RTSim {
    using namespace std;
    using namespace MetaSim;
    
    

    SuspendInstr::SuspendInstr(Task *f, Tick d) :
	Instr(f), 
	suspEvt(this, &SuspendInstr::onSuspend),
	resumeEvt(this, &SuspendInstr::onEnd),
	delay(d)
    {
    }

    SuspendInstr * SuspendInstr::createInstance(vector<string> &par)
    {
	// TODO
	return NULL;
    }

    void SuspendInstr::schedule()
    {
	// TODO
    }

    void SuspendInstr::deschedule()
    {
	// TODO
    }

    void SuspendInstr::setTrace(Trace *t)
    {
	// TODO
    }

    void SuspendInstr::onSuspend(Event *evt)
    {
	// TODO
    }

    void SuspendInstr::onEnd(Event *evt)
    {
	// TODO
    }
    
    void SuspendInstr::newRun()
    {
	// TODO
    }
    
    void SuspendInstr::endRun()
    {
	// TODO
    }

}
