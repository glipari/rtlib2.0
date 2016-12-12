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
#ifndef __THREINSTR_HPP__
#define __THREINSTR_HPP__

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
    class ThreInstr;

    /**
       \ingroup instr

       event for threshold instr
    */
    class ThreEvt : public TaskEvt
    {
    protected:
        ThreInstr * ti;
    public:
        ThreEvt(Task* t, ThreInstr* in) :TaskEvt(t, _DEFAULT_PRIORITY - 3), ti(in)
            {}
        ThreInstr *getInstr() {return ti;} 
        virtual void doit() {}
    };

    /** 
        \ingroup instr

        Simple classes which model instruction to set a preemption threshold
        @author Francesco Prosperi
        @see Instr 
    */

    class ThreInstr : public Instr {
        EndInstrEvt _endEvt; 
        ThreEvt _threEvt;
        string _th;
    public:
        /**
         //      This is the constructor of the ThreInstr.
         //      @param f is a pointer to the task containing the pseudo
         //      instruction
         //      @param r is the name of the resorce manager handling the
         //      resource which the task is accessing
         //      @param nr is the number of resources being taken
         //      @param n is the instruction name
         */
        ThreInstr(Task * f, const std::string& th, const std::string &n = "");

        static std::unique_ptr<ThreInstr> createInstance(const std::vector<std::string> &par);

        ///Virtual methods from Instr
        virtual void schedule();
        virtual void deschedule();
        virtual Tick getExecTime() const { return 0;};
        virtual Tick getDuration() const { return 0;};
        virtual Tick getWCET() const throw(RandomVar::MaxException) { return 0; }
        virtual void reset() {}
        
        //virtual void setTrace(Trace *);

        template <class TraceClass>
        void setTrace(TraceClass &trace_obj) {
            attach_stat(trace_obj, _endEvt);
            attach_stat(trace_obj, _threEvt);
        }
        
        virtual void onEnd();
        virtual void newRun() {};
        virtual void endRun();


        /** Function inherited from clss Instr.It refreshes the state 
         *  of the executing instruction when a change of the CPU speed occurs. 
         */ 
        virtual void refreshExec(double, double){}
    };

} //namespace RTSim

#endif
