
#include <kernel.hpp>
#include <kernevt.hpp>

namespace RTSim {
/*	
    void DispatchEvt::doit(){
        _kernel->onDispatch(this);
    }
*/
    void BeginDispatchEvt::doit(){
        _kernel->onBeginDispatch(this);
    }

    void EndDispatchEvt::doit(){
        _kernel->onEndDispatch(this);
    }

}
