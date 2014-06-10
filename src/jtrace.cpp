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
#include <algorithm>
#include <string>

#include <simul.hpp>

#include <server.hpp>
#include <jtrace.hpp>
#include <task.hpp>
#include <task.hpp>
#include <waitinstr.hpp>

namespace RTSim {

  using namespace std;
  using namespace MetaSim;
 
  // Init globals
  JavaTrace::TRACE_ENDIANESS JavaTrace::endianess = TRACE_UNKNOWN_ENDIAN;  
  string JavaTrace::version = "1.2";

  JavaTrace::JavaTrace(const char *name, bool tof, unsigned long int limit)
    :Trace(name, Trace::BINARY, tof), taskList(10)
  {
    if (endianess == TRACE_UNKNOWN_ENDIAN) probeEndianess();
    const char cver[] = "version 1.2";
    if (toFile) _os.write(cver, sizeof(cver));
    filenum = 0;
    fileLimit = limit;
  }

  JavaTrace::~JavaTrace()
  {
    // WARNING: the data vector is cleared but its content isn't. It
    // must be deleted manually (close).
    if (toFile) Trace::close();
    else data.clear();
  }

  void JavaTrace::probeEndianess(void)
  {
    // Used for endianess check (Big/Little!)
    static char const big_endian[] = {
        static_cast<char const>(0xff),
        static_cast<char const>(0),
        static_cast<char const>(0xff),
        static_cast<char const>(0)};
    static int const probe = 0xff00ff00;
    int *probePtr = (int *)(big_endian);
 
    if (*probePtr == probe) endianess = TRACE_BIG_ENDIAN;
    else endianess = TRACE_LITTLE_ENDIAN;
    //DBGFORCE("Endianess: " << (endianess == TRACE_LITTLE_ENDIAN ? "Little\n" : "Big\n"));
  }

  void JavaTrace::close()
  {
    if (toFile) Trace::close();
    else {
      for (unsigned int i = 0; i < data.size(); i++) delete data[i];
      data.clear();
    }
  }

  void JavaTrace::record(Event* e)
  {
    DBGENTER(_JTRACE_DBG_LEV);
    TaskEvt* ee = dynamic_cast<TaskEvt*>(e);

    if (ee == NULL) {
      DBGPRINT("The event is not a TaskEvt");
      
      return;
    }

    Task* task = ee->getTask();
    if (task != NULL) {
      vector<int>::const_iterator p = find(taskList.begin(), taskList.end(),
					   task->getID());
      if (p == taskList.end()) {
	TraceNameEvent* a = new TraceNameEvent(ee->getLastTime(),
					       task->getID(), task->getName());
	if (toFile) a->write(_os);
	else data.push_back(a);
	taskList.push_back(task->getID());
      }
    }
 
    // at this point we have to see what kind of event it is...

    if (dynamic_cast<ArrEvt*>(e) != NULL) {
      DBGPRINT("ArrEvt");
      ArrEvt* tae = dynamic_cast<ArrEvt*>(e);
      TraceArrEvent* a = new TraceArrEvent(e->getLastTime(), task->getID());
      if (toFile) a->write(_os);
      else data.push_back(a);
      Task* rttask = dynamic_cast<Task*>(task);
      if (rttask) {
	TraceDlineSetEvent* b = new TraceDlineSetEvent(tae->getLastTime(),
						       rttask->getID(),
						       rttask->getDeadline());
	if (toFile) b->write(_os);
	else data.push_back(b);
      }
    } else if (dynamic_cast<EndEvt*>(e) != NULL) {
      DBGPRINT("EndEvt");
      EndEvt* tee = dynamic_cast<EndEvt*>(e);

      TraceEndEvent* a = new TraceEndEvent(tee->getLastTime(), task->getID(),
					   tee->getCPU());
      if (toFile) a->write(_os);
      else data.push_back(a);
    } else if (dynamic_cast<DeschedEvt*>(e) != NULL) {
      DBGPRINT("DeschedEvt");
      DeschedEvt *tde = dynamic_cast<DeschedEvt *>(e);

      TraceDeschedEvent* a = new TraceDeschedEvent(tde->getLastTime(),
						   task->getID(),
						   tde->getCPU());
      if (toFile) a->write(_os);
      else data.push_back(a);
    } else if (dynamic_cast<WaitEvt*>(e) != NULL) {
      DBGPRINT("WaitEvt");
      WaitEvt* we = dynamic_cast<WaitEvt*>(e);
      //WaitInstr* instr = dynamic_cast<WaitInstr*>(we->getInstr());
      WaitInstr* instr = we->getInstr();
      string res = instr->getResource();
      TraceWaitEvent* a = new TraceWaitEvent(we->getLastTime(),
					     task->getID(), res);
      if (toFile) a->write(_os);
      else data.push_back(a);
    } else if (dynamic_cast<SignalEvt*>(e) != NULL) {
      DBGPRINT("SignalEvt");
      SignalEvt* se = dynamic_cast<SignalEvt*>(e);
      //SignalInstr* instr = dynamic_cast<SignalInstr*>(se->getInstr());
      SignalInstr* instr = se->getInstr();
      string res = instr->getResource();
      TraceSignalEvent* a = new TraceSignalEvent(se->getLastTime(),
						 task->getID(), res);
      if (toFile) a->write(_os);
      else data.push_back(a);
    } else if (dynamic_cast<SchedEvt*>(e) != NULL) {
      DBGPRINT("SchedEvt");
      SchedEvt* tse = dynamic_cast<SchedEvt*>(e);

      TraceSchedEvent* a = new TraceSchedEvent(tse->getLastTime(),
					       task->getID(), tse->getCPU());
      if (toFile) a->write(_os);
      else data.push_back(a);
//     } else if (dynamic_cast<DlinePostEvt*>(e) != NULL) {
//       DBGPRINT("DlinePostEvt");
//       DlinePostEvt* dpe = dynamic_cast<DlinePostEvt*>(e);
//       TraceDlinePostEvent* a = new TraceDlinePostEvent(dpe->getLastTime(),
// 						       task->getID(),
// 						       dpe->getPrevDline(),
// 						       dpe->getPostDline());
//       if (toFile) a->write(_os);
//       else data.push_back(a);
    } else if (dynamic_cast<DeadEvt*>(e) != NULL) {
      DBGPRINT("DlineMissEvt");
      DeadEvt* de = dynamic_cast<DeadEvt*>(e);
      TraceDlineMissEvent* a = new TraceDlineMissEvent(de->getLastTime(),
						       task->getID());
      if (toFile) a->write(_os);
      else data.push_back(a);
    }

    if (toFile) _os.flush();
    
  }         

}
