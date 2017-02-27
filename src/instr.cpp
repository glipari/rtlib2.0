#include <instr.hpp>

namespace RTSim
{

    void EndInstrEvt::doit()
    {
        _instr->onEnd();
    }
    
    Instr* EndInstrEvt::getInstruction() const
    {
        return (_instr);
    }

    Instr::Instr(const Instr &obj) :
        Entity(obj),
        _father(obj._father) {
    }
    
}
