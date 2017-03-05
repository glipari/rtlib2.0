/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <simul.hpp>

#include <kernel.hpp>
#include <task.hpp>
#include <threinstr.hpp>

namespace RTSim {

    using namespace std;
    
    ThreInstr::ThreInstr(Task * f, int th, const string &n)
        : Instr(f, n), _endEvt(this), _threEvt(f, this), _th(th)  
    {}

    ThreInstr::ThreInstr(const ThreInstr &other) 
        : Instr(other), _endEvt(this), _threEvt(other.getTask(), this), _th(other.getThres())
    {
    }
   
    unique_ptr<ThreInstr> ThreInstr::createInstance(const vector<string> &par)
    {
        int th = stoi(par[0]);
        unique_ptr<ThreInstr> ptr(new ThreInstr(dynamic_cast<Task*>(Entity::_find(par[1])), th));
        
        return ptr;
    }

    void ThreInstr::endRun() 
    {
        _endEvt.drop(); 
        _threEvt.drop();
    }

    void ThreInstr::schedule()
    {
        DBGENTER(_INSTR_DBG_LEV);
        DBGPRINT("Scheduling ThreInstr named: " << getName());

        _endEvt.post(SIMUL.getTime());
    }

    void ThreInstr::deschedule()
    {
        DBGTAG(_INSTR_DBG_LEV,"ThreInstr::deschedule()");
        _endEvt.drop();
    }

    void ThreInstr::onEnd() 
    {
        DBGENTER(_INSTR_DBG_LEV);

        DBGPRINT("Ending ThreInstr named: " << getName());

        _father->onInstrEnd();

        RTKernel *k = dynamic_cast<RTKernel *>(_father->getKernel());

        if (k == NULL) throw BaseExc("Kernel not found!");

        DBGPRINT("Before lowing threshold for task " << _father->getName());

        k->setThreshold(_th);
        k->enableThreshold();

        DBGPRINT("After raising threshold for task " << _father->getName());

        _threEvt.process();    
    }
}
