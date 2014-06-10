#ifndef __KERNEVT_HPP__
#define __KERNEVT_HPP__

#include <event.hpp>

#include <kernel.hpp>

namespace RTSim {

    class RTKernel;

    class KernelEvt : public MetaSim::Event
    {
    protected:
        RTKernel* _kernel;

    public:
        KernelEvt(RTKernel* k, int p = MetaSim::Event::_DEFAULT_PRIORITY + 10) 
  : MetaSim::Event(p) { _kernel=k; }
	void setKernel(RTKernel* k) { _kernel=k; };
	RTKernel* getKernel(){ return _kernel; };
    };

    class DispatchEvt : public KernelEvt
    {
    public:
        DispatchEvt(RTKernel* k) : KernelEvt(k) {}
	virtual void doit();
    };

    class BeginDispatchEvt : public KernelEvt
    {
    public:
        BeginDispatchEvt(RTKernel* k) : KernelEvt(k) {}
	virtual void doit();
    };

    class EndDispatchEvt : public KernelEvt
    {
    public:
        EndDispatchEvt(RTKernel* k) : KernelEvt(k) {}
	virtual void doit();
    };

}

#endif // __KERNEVT_HPP__ 
