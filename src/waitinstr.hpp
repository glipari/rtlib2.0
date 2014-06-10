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
 * $Id: waitinstr.hpp,v 1.10 2005/10/05 16:46:22 cesare Exp $
 *
 * $Log: waitinstr.hpp,v $
 * Revision 1.10  2005/10/05 16:46:22  cesare
 * Added DLL support.
 *
 * Revision 1.9  2005/08/03 16:41:09  cesare
 * gcc-4 compatibility (maybe).
 *
 * Revision 1.8  2005/04/28 01:34:47  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.7  2005/03/31 15:27:27  cesare
 * Corrected some documentations. Added XML docs.
 *
 * Revision 1.6  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#ifndef __WAITINSTR_HPP__
#define __WAITINSTR_HPP__

#include <string>
#include <vector>

//From metasim
#include <event.hpp>
#include <factory.hpp>

//From RTLIB
#include <instr.hpp>
#include <taskevt.hpp>

namespace RTSim {

  using namespace std;
  using namespace MetaSim;

  class Task;
  class WaitInstr;
  class SignalInstr;

  /**
     \ingroup instr

     event for wait instr
   */
  class WaitEvt : public TaskEvt
  {
  protected:
    WaitInstr * wi;
  public:
    WaitEvt(Task* t, WaitInstr* in) :TaskEvt(t, _DEFAULT_PRIORITY - 3), wi(in)
    {}
    WaitInstr *getInstr() {return wi;} 
    virtual void doit() {}
  };

  /**
     \ingroup instr

     event for signal instr
   */
  class SignalEvt : public TaskEvt
  {
  protected:
    SignalInstr *si;
  public:
    SignalEvt(Task* t, SignalInstr* in) :TaskEvt(t), si(in) {} 
    virtual void doit() {}
    SignalInstr *getInstr() {return si;}
  };

  /** 
      \ingroup instr

      Simple classes which model wait and signal instruction to use a resource 
      @author Fabio Rossi and Giuseppe Lipari
      @see Instr 
   */

  class WaitInstr : public Instr {
    string _res;
    EndInstrEvt _endEvt; 
    WaitEvt _waitEvt;
    int _numberOfRes;
  public:
    /**
       This is the constructor of the WaitInstr.
       @param f is a pointer to the task containing the pseudo
       instruction
       @param r is the name of the resorce manager handling the
       resource which the task is accessing
       @param nr is the number of resources being taken
       @param n is the instruction name
     */
    WaitInstr(Task * f, const char *r, int nr=1, char *n = "");
    WaitInstr(Task * f, const string &r, int nr=1, char *n = "");

    static Instr* createInstance(vector<string> &par);

    ///Virtual methods from Instr
    virtual void schedule();
    virtual void deschedule();
    virtual Tick getExecTime() const { return 0;};
    virtual Tick getDuration() const { return 0;};
    virtual Tick getWCET() const throw(RandomVar::MaxException) { return 0; }
    string getResource() const { return _res; };
    virtual void reset() {}
    virtual void setTrace(Trace *);

    virtual void onEnd();
    virtual void newRun() {};
    virtual void endRun();


    /** Function inherited from clss Instr.It refreshes the state 
     *  of the executing instruction when a change of the CPU speed occurs. 
     */ 
    virtual void refreshExec(double, double){}

  };

  /**
     \ingroup instr

     Simple class which models signal instruction to use a resource. 
     @author Fabio Rossi and Giuseppe Lipari
     @see Instr 
   */

  class SignalInstr : public Instr {
    string _res;
    EndInstrEvt _endEvt;
    SignalEvt _signalEvt;
 
    int _numberOfRes;
  public:
    /**
       This is the constructor of the SignalInstr
       @param f is a pointer to the task containing the pseudo
       instruction
       @param r is the name of the resource which the task has
       accessed
       @param nr is the number of resources being taken
       @param n is the instruction name
     */
    SignalInstr(Task *f, const char *r, int nr=1, char *n = "");
    SignalInstr(Task *f, const string &r, int nr=1, char *n = "");

    static Instr* createInstance(vector<string> &par);

    ///Virtual methods from Instr
    virtual void schedule();
    virtual void deschedule();
    virtual Tick getExecTime() const { return 0;};
    virtual Tick getDuration() const { return 0;};
    virtual Tick getWCET() const throw(RandomVar::MaxException) {return 0;}
    virtual void reset() {}
    virtual void setTrace(Trace *);
    string getResource() const { return _res; }; 
    virtual void onEnd();
    virtual void newRun() {};
    virtual void endRun();


    /** Function inherited from clss Instr.It refreshes the state 
     *  of the executing instruction when a change of the CPU speed occurs. 
     */ 
    virtual void refreshExec(double, double) {}

  };

} //namespace RTSim

#endif
