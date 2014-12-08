#ifndef __SUSPEND_INSTR__
#define __SUSPEND_INSTR__

#include <vector>
#include <string>
#include <gevent.hpp>
#include <randomvar.hpp>
#include <instr.hpp>


namespace RTSim {
    class SuspendInstr : public Instr { 
	MetaSim::GEvent<SuspendInstr> suspEvt;
	MetaSim::GEvent<SuspendInstr> resumeEvt;

	Tick delay;

    public:
	SuspendInstr(Task *f, MetaSim::Tick delay);
	static SuspendInstr * createInstance(std::vector<std::string> &par);
	
	virtual void schedule();
	virtual void deschedule();
	virtual Tick getExecTime() const { return 0;};
	virtual Tick getDuration() const { return 0;};
	virtual Tick getWCET() const throw(RandomVar::MaxException) { return 0; }
	virtual void reset() {}
	virtual void setTrace(Trace *);
	
	void onSuspend(MetaSim::Event *evt);
	void onEnd(MetaSim::Event *evt);
	void newRun();
	void endRun();
	
	/** Function inherited from clss Instr.It refreshes the state 
	 *  of the executing instruction when a change of the CPU speed occurs. 
	 */ 
	virtual void refreshExec(double, double){}
    };
}

#endif 
