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
#ifndef __SCHEDINSTR_HPP__
#define __SCHEDINSTR_HPP__

#include <string>
#include <vector>

//From METASIM
#include <event.hpp>
#include <factory.hpp>

//From RTLIB
#include <instr.hpp>
#include <taskevt.hpp>

namespace RTSim {

    using namespace MetaSim;

    class Task;
    class SchedInstr;

    /**
       \ingroup instr

       event for threshold instr
    */
    class SchedIEvt : public TaskEvt
    {
    protected:
        SchedInstr * ti;
    public:
        SchedIEvt(Task* t, SchedInstr* in) :TaskEvt(t, _DEFAULT_PRIORITY - 3), ti(in)
            {}
        SchedInstr *getInstr() { return ti; } 
        virtual void doit() {}
    };

    /** 
        \ingroup instr

        Simple classes which model instruction to set a preemption threshold
        @author Francesco Prosperi
        @see Instr 
    */
    class SchedInstr : public Instr {
        EndInstrEvt _endEvt; 
        SchedIEvt _threEvt;

        // Copy constructor
        SchedInstr(const SchedInstr &si); 
        
    public:
        /**
         //      This is the constructor of the SchedInstr.
         //      @param f is a pointer to the task containing the pseudo
         //      instruction
         */
        SchedInstr(Task * f, const std::string& s, const std::string &n = "");

        CLONEABLE(Instr, SchedInstr)
       
        static std::unique_ptr<SchedInstr> createInstance(const std::vector<std::string> &par);

        ///Virtual methods from Instr
        virtual void schedule();
        virtual void deschedule();
        virtual Tick getExecTime() const { return 0; }
        virtual Tick getDuration() const { return 0; }
        virtual Tick getWCET() const throw(RandomVar::MaxException) { return 0; }
        virtual void reset() {}

        template <class TraceClass>
        void setTrace(TraceClass &trace_obj) {
            attach_stat(trace_obj, _endEvt);
            attach_stat(trace_obj, _threEvt);
        }

        virtual void onEnd();
        virtual void newRun() {}
        virtual void endRun();


        /** Function inherited from clss Instr.It refreshes the state 
         *  of the executing instruction when a change of the CPU speed occurs. 
         */ 
        virtual void refreshExec(double, double){}

    };

} //namespace RTSim

#endif
