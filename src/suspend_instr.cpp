#include <kernel.hpp>
#include <task.hpp>

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

    SuspendInstr::SuspendInstr(const SuspendInstr &other) :
        Instr(other),
        suspEvt(this, &SuspendInstr::onSuspend),
        resumeEvt(this, &SuspendInstr::onEnd),
        delay(other.getDelay())
    {}
        
    SuspendInstr * SuspendInstr::createInstance(vector<string> &par)
    {
        if (par.size() != 2) throw parse_util::ParseExc("SuspendInstr::createInstance", "Wrong number of arguments"); 
	
        Task *t = dynamic_cast<Task *>(Entity::_find(par[1]));
        Tick d = stoi(par[0]);

        return new SuspendInstr(t, d);
    }

    void SuspendInstr::schedule()
    {
        suspEvt.process();
    }

    void SuspendInstr::deschedule()
    {
    }

    void SuspendInstr::setTrace(Trace *t)
    {
    }

    void SuspendInstr::onSuspend(Event *evt)
    {
        AbsKernel *k = _father->getKernel();
        //RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());
        // if (k == 0) {
        //     throw BaseExc("SuspendInstr has no kernel set!");
        // }
        // else {
        k->suspend(_father);
        k->dispatch();
        //}
        resumeEvt.post(SIMUL.getTime() + delay);
    }

    void SuspendInstr::onEnd(Event *evt)
    {
        _father->onInstrEnd();
        //RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());
        AbsKernel *k = _father->getKernel();
        //if (k == 0) {
        //    throw BaseExc("SuspendInstr has no kernel set!");
        //}
        //else 
        //k->activate(_father);
        k->onArrival(_father);
        //k->dispatch();
    }
    
    void SuspendInstr::newRun()
    {
        // nothing to be done
    }
    
    void SuspendInstr::endRun()
    {
        suspEvt.drop();
        resumeEvt.drop();
    }

}
