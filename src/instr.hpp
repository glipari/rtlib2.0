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
/*
 * $Id: instr.hpp,v 1.6 2005/08/02 16:08:35 cesare Exp $
 *
 * $Log: instr.hpp,v $
 * Revision 1.6  2005/08/02 16:08:35  cesare
 * Towards RTSim 0.5: dynamic libraries support.
 *
 * Revision 1.5  2005/04/28 01:34:47  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.4  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#ifndef __INSTR_HPP__
#define __INSTR_HPP__

//...from metasim
#include <baseexc.hpp>
#include <entity.hpp>
#include <event.hpp>
#include <randomvar.hpp>

//...from rtlib
#include <taskevt.hpp>

#define _INSTR_DBG_LEV "Instruction"

namespace RTSim {

    using namespace std;
    using namespace MetaSim;

    class Task;

    /**
       \ingroup instr

       Exceptions for the instructions
    */
    class InstrExc: public BaseExc
    {
    public: 
        InstrExc(const string &msg, const string &cl)
            :BaseExc(msg, cl, "InstrExc") {}

    };

    /**
       \ingroup instr
 
       The base class for every pseudo instruction. Pseudo-instructions
       represents the code that a task executes. An instruction is identified
       by an execution time (possibly random) and by a certain optional 
       functionality. 

       A task contains a list of instructions, that are executed in
       sequence.

       @see Task.

       @todo Implement labels, and non-sequential constructs.
    */
    class Instr: public Entity
    {
    protected:
        Task* _father;

    public:

        typedef string BASE_KEY_TYPE;

        Instr(Task *f, const std::string &n = "") :Entity(n), _father(f) {}

        virtual ~Instr() {}

        /** 
         * Returns a ponter to that task which ownes this instruction.
         */ 
        Task* getTask() const {return _father;}
  
        /** 
         * Called when the instruction is scheduled.
         */
        virtual void schedule() = 0;
  
        /** 
         * Called when the instruction  is descheduled.
         */
        virtual void deschedule() = 0;
  
        /** 
         * Called upon the instruction end event
         */
        virtual void onEnd() = 0;

        /** 
            This method permits to kill a task which is currently
            executing. It resets the internal state of the executing
            instruction.

            This is currently used only by the fault-tolerant scheduling
            algorithms.
        */ 
        virtual void reset() = 0;

        /**
           Returns how long the instrucion has been executed from the last
           reset().

           NOTE: the resetExecdTime is now implicit!!!!
        */
        virtual Tick getExecTime() const = 0;

        /**
           Returns the total computation time of the instruction
        */
        virtual Tick getDuration() const = 0;

        /**
           Returns the worst-case execution time for this instruction.
        */
        virtual Tick getWCET() const throw(RandomVar::MaxException) = 0;

        virtual void setTrace(Trace*) = 0;

        // virtual methods from entity
        virtual void newRun() = 0;
        virtual void endRun() = 0;  

        /** 
            It refreshes the state of the executing instruction 
            when a change of the CPU speed occurs. 
        */ 
        virtual void refreshExec(double oldSpeed, double newSpeed) = 0;
    };

    /**
       \ingroup instr
     
       End event for instructions
    */
    class EndInstrEvt : public MetaSim::Event {
        Instr *_instr;
    public:
        EndInstrEvt(Instr * in) : 
            MetaSim::Event(Event::_DEFAULT_PRIORITY-3), _instr(in) {} 
        virtual void doit();
        
        virtual Instr* getInstruction() const;

    };

} // namespace RTSim 

#endif
