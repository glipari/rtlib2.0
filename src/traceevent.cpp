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
 * $Id: traceevent.cpp,v 1.2 2005/12/21 14:40:12 cesare Exp $
 *
 * $Log: traceevent.cpp,v $
 * Revision 1.2  2005/12/21 14:40:12  cesare
 * Vertical scrollbar for tracer.
 *
 * Revision 1.1  2005/11/18 17:08:57  cesare
 * Improved the Tracer.
 *
 */
#include <jtrace.hpp>
#include <traceevent.hpp>

namespace RTSim {

  using namespace std;

  const string TraceEvent::evt_name[] = {
    "ARRIVAL", "SCHEDULE", "DESCHEDULE", "END", "DLINE POSTPONED",
    "DLINE SET", "WAIT", "SIGNAL", "IDLE", "NAME", "DLINE MISS"};

  void TraceEvent::encode(char *in, int size)
  {
    char temp[20];
    if (JavaTrace::endianess == JavaTrace::TRACE_LITTLE_ENDIAN) {
      for(int i = 0; i < size; i++)
	temp[i] = in[size - 1 - i];
      for(int i = 0; i < size; i++)
	in[i] = temp[i];
    }
  }

  const TraceEvent* TraceEvent::getEvent(int actType)
  {
    if (actType < 0 || actType >= EVT_NUMBER)
      return NULL;

    TraceEvent* result = NULL;

    switch (actType) {
    case TASK_ARRIVAL:
      result = new TraceArrEvent();
      break;
    case TASK_SCHEDULE:
      result = new TraceSchedEvent();
      break;
    case TASK_DESCHEDULE:
      result = new TraceDeschedEvent();
      break;
    case TASK_END:
      result = new TraceEndEvent();
      break;
    case TASK_DLINEPOST:
      result = new TraceDlinePostEvent();
      break;
    case TASK_DLINESET:
      result = new TraceDlineSetEvent();
      break;
    case TASK_WAIT:
      result = new TraceWaitEvent();
      break;
    case TASK_SIGNAL:
      result = new TraceSignalEvent();
      break;
    case TASK_IDLE:
      result = new TraceIdleEvent();
      break;
    case TASK_NAME:
      result = new TraceNameEvent();
      break;
    case TASK_DLINEMISS:
      result = new TraceDlineMissEvent();
      break;
    default:
      cerr<<"Class not found!!\n";
    }
    return result;
  }

  bool TraceEvent::somethingToDraw(int t)
  {
    if (_time >= t) return true;
    return false;
  }

  void TraceEvent::write(ofstream &f)
  {
    unsigned int size = getSize();
    int type = _type, time = _time;
    encode((char *)& size, sizeof(size));
    f.write((char *)& size, sizeof(size));
    encode((char *)& type, sizeof(type));
    f.write((char *)& type, sizeof(type));
    encode((char *)& time, sizeof(time));
    f.write((char *)& time, sizeof(time));
  }

  void TraceEvent::read(ifstream& in)
  {
    in.read((char*)& _time, sizeof(_time));
    encode((char*)& _time, sizeof(_time));
  }

  bool TraceEvent::equals(TraceEvent* e)
  {
    if (e->_type == _type && e->_time == _time)
      return true;
    return false;
  }

  void TraceTaskEvent::write(ofstream &f)
  {
    TraceEvent::write(f);
    int task = _task;
    encode((char*)& task, sizeof(task));
    f.write((char*)& task, sizeof(task));
  }

  string TraceTaskEvent::print()
  {
    char ctime[50], ctask[50];
    sprintf(ctime, "%d", _time);
    sprintf(ctask, "%d", _task);
    string s = "- time : " + string(ctime) + "\n- task : "
      + tdesc->name + "\n- Type : " + evt_name[_type]  + "\n";
    return s;
  }

  string TraceTaskEvent::getDescription()
  {
    string s;
    if (_type == 0) s = "Task Arrival";
    if (_type == 1) s = "Task Scheduled";
    if (_type == 2) s = "Task Descheduled";
    if (_type == 3) s = "Task End";
    if (_type == 4) s = "Deadline Postponed";
    if (_type == 5) s = "Deadline Set";
    if (_type == 6) s = "Wait";
    if (_type == 7) s = "Signal";
    if (_type == 8) s = "Idle";
    if (_type == 9) s = "Task Name";
    if (_type == 10) s = "Deadline Miss";
    return tdesc->name + " - " + s;
  }

  void TraceTaskEvent::read(ifstream& in)
  {
    TraceEvent::read(in);
    in.read((char*)& _task, sizeof(_task));
    encode((char*)& _task, sizeof(_task));
  }

  bool TraceTaskEvent::equals(TraceEvent* e)
  {
    if (TraceEvent::equals(e)) {
      TraceTaskEvent* ev = dynamic_cast<TraceTaskEvent*>(e);
      if (ev != NULL && _task == ev->_task) return true;
      else return false;
    } else return false;
  }

  string TraceCPUEvent::print()
  {
    string ret = TraceTaskEvent::print();
    if (JavaTrace::version >= "1.2") {
      char ccpu[50];
      sprintf(ccpu, "%d", _cpu);
      ret += "- CPU: " + string(ccpu) + "\n";
    }
    return ret;
  }

  string TraceCPUEvent::getDescription()
  {
    string ret = TraceTaskEvent::getDescription();
    if (JavaTrace::version >= "1.2") {
      char ccpu[50];
      sprintf(ccpu, "%d", _cpu);
      ret += " on CPU "	+ string(ccpu);
    }
    return ret;
  }

  void TraceCPUEvent::write(ofstream& f)
  {
    TraceTaskEvent::write(f);
    int cpu = _cpu;
    encode((char *)& cpu, sizeof(cpu));
    f.write((char *)& cpu, sizeof(cpu));
  }

  void TraceCPUEvent::read(ifstream& in)
  {
    TraceTaskEvent::read(in);
    if (JavaTrace::version >= "1.2") {
      in.read((char*)& _cpu, sizeof(_cpu));
      encode((char*)& _cpu, sizeof(_cpu));
    }
  }

  string TraceDlinePostEvent::print()
  {
    char ctaskD[50], ctaskD2[50];
    sprintf(ctaskD, "%d", _taskD);
    sprintf(ctaskD2, "%d", _taskD2);
    return TraceTaskEvent::print() + "- taskD: " + string(ctaskD)
      + "\n- taskD2: " + string(ctaskD2) + "\n";
  }

  string TraceDlinePostEvent::getDescription()
  {
    char ctaskD[50], ctaskD2[50];
    sprintf(ctaskD, "%d", _taskD);
    sprintf(ctaskD2, "%d", _taskD2);
    return TraceTaskEvent::getDescription() + " - from "
      + string(ctaskD) + " to " + string(ctaskD2);
  }

  bool TraceDlinePostEvent::somethingToDraw(int t)
  {
    if (_taskD2 >= t) return true;
    return TraceTaskEvent::somethingToDraw(t);
  }

  void TraceDlinePostEvent::write(ofstream& f)
  {
    TraceTaskEvent::write(f);
    int taskD = _taskD, taskD2 = _taskD2;
    encode((char*)& taskD, sizeof(taskD));
    f.write((char*)& taskD, sizeof(taskD));
    encode((char*)& taskD2, sizeof(taskD2));
    f.write((char*)& taskD2, sizeof(taskD2));
  }

  void TraceDlinePostEvent::read(ifstream& in)
  {
    TraceTaskEvent::read(in);
    in.read((char*)& _taskD, sizeof(_taskD));
    encode((char*)& _taskD, sizeof(_taskD));
    in.read((char*)& _taskD2, sizeof(_taskD2));
    encode((char*)& _taskD2, sizeof(_taskD2));
  }

  bool TraceDlinePostEvent::equals(TraceEvent* e)
  {
    if (TraceTaskEvent::equals(e)) {
      TraceDlinePostEvent* ev = dynamic_cast<TraceDlinePostEvent*>(e);
      if (ev != NULL && _taskD == ev->_taskD && _taskD2 == ev->_taskD2)
	return true;
      return false;
    }
    return false;
  }

  string TraceDlineSetEvent::print()
  {
    char ctaskD[50];
    sprintf(ctaskD, "%d", _taskD);
    return TraceTaskEvent::print() + "- taskD: " + string(ctaskD) + "\n";
  }

  string TraceDlineSetEvent::getDescription()
  {
    char ctaskD[50];
    sprintf(ctaskD, "%d", _taskD);
    return TraceTaskEvent::getDescription() + " at " + string(ctaskD);
  }

  bool TraceDlineSetEvent::somethingToDraw(int t)
  {
    if (_taskD >= t) return true;
    else return TraceTaskEvent::somethingToDraw(t);
  }

  void TraceDlineSetEvent::write(ofstream& f)
  {
    TraceTaskEvent::write(f);
    int taskD = _taskD;
    encode((char*)& taskD, sizeof(taskD));
    f.write((char*)& taskD, sizeof(taskD));
  }

  void TraceDlineSetEvent::read(ifstream& in)
  {
    TraceTaskEvent::read(in);
    in.read((char*)& _taskD, sizeof(_taskD));
    encode((char*)& _taskD, sizeof(_taskD));
  }

  bool TraceDlineSetEvent::equals(TraceEvent* e)
  {
    if (TraceTaskEvent::equals(e)) {
      TraceDlineSetEvent* ev = dynamic_cast<TraceDlineSetEvent*>(e);
      if (ev != NULL && _taskD != ev->_taskD) return false;
      return true;
    }
    return false;
  }

  unsigned int TraceWaitEvent::getSize() const
  {
    unsigned int len = _res.size();
    return TraceTaskEvent::getSize() + sizeof(int) + sizeof(len) + len;
  }

  string TraceWaitEvent::print()
  {
    return TraceTaskEvent::print() + " on resource : " + _res + "\n";
  }

  string TraceWaitEvent::getDescription()
  {
    return TraceTaskEvent::getDescription() + " on " + _res;
  }

  void TraceWaitEvent::write(ofstream& f)
  {
    int len;
    int resID = 1;
    TraceTaskEvent::write(f);
    encode((char*)& resID, sizeof(resID));
    f.write((char*)& resID, sizeof(resID));
    len = _res.size();
    encode((char*)& len, sizeof(len));
    f.write((char*)& len, sizeof(len));
    f.write(_res.c_str(), _res.size());
  }

  void TraceWaitEvent::read(ifstream& in)
  {
    int res;
    TraceTaskEvent::read(in);
    in.read((char*)& res, sizeof(res));
    encode((char*)& res, sizeof(res));
    _res = "";

    int len;
    in.read((char*)& len, sizeof(len));
    encode((char*)& len, sizeof(len));
    for (int i = 0; i < len; i++) {
      char c = in.get();
      _res += c;
    }
  }

  bool TraceWaitEvent::equals(TraceEvent* e)
  {
    if (TraceTaskEvent::equals(e)) {
      TraceWaitEvent* ev = dynamic_cast<TraceWaitEvent*>(e);
      if (ev != NULL && ev->_res == _res)
	return true;
      return false;
    }
    return false;
  }

  unsigned int TraceSignalEvent::getSize() const
  {
    unsigned int len = _res.size();
    return TraceTaskEvent::getSize() + sizeof(int) + sizeof(len) + len;
  }

  string TraceSignalEvent::print()
  {
    return TraceTaskEvent::print() + " on resource : " + _res + "\n";
  }

  string TraceSignalEvent::getDescription()
  {
    return TraceTaskEvent::getDescription() + " on " + _res;
  }

  void TraceSignalEvent::write(ofstream& f)
  {
    int len;
    int resID = 1;
    TraceTaskEvent::write(f);
    encode((char*)& resID, sizeof(resID));
    f.write((char*)& resID, sizeof(resID));
    len = _res.size();
    encode((char*)& len, sizeof(len));
    f.write((char*)& len, sizeof(len));
    f.write(_res.c_str(), _res.size());
  }

  void TraceSignalEvent::read(ifstream& in)
  {
    int res;
    TraceTaskEvent::read(in);
    in.read((char*)& res, sizeof(res));
    encode((char*)& res, sizeof(res));
    _res = "";

    int len;
    in.read((char*)& len, sizeof(len));
    encode((char*)& len, sizeof(len));
    for (int i = 0; i < len; i++) {
      char c = in.get();
      _res += c;
    }
  }

  bool TraceSignalEvent::equals(TraceEvent* e)
  {
    if (TraceTaskEvent::equals(e)) {
      TraceSignalEvent* ev = dynamic_cast<TraceSignalEvent*>(e);
      if (ev != NULL && ev->_res == _res)
	return true;
      return false;
    }
    return false;
  }

  unsigned int TraceNameEvent::getSize() const
  {
    unsigned int len = _name.size();
    return TraceTaskEvent::getSize() + sizeof(len) + len;
  }

  string TraceNameEvent::getDescription()
  {
    return TraceTaskEvent::getDescription() + ": " + _name;
  }

  void TraceNameEvent::write(ofstream& f)
  {
    int len = _name.size();
    int len2 = len;

    TraceTaskEvent::write(f);
    encode((char*)& len2, sizeof(len2));
    f.write((char*)& len2, sizeof(len2));
    f.write(_name.c_str(), len);
  }

  void TraceNameEvent::read(ifstream& in)
  {
    TraceTaskEvent::read(in);
    _name = "";
    int len;
    in.read((char*)& len, sizeof(len));
    encode((char*)& len, sizeof(len));
    for (int i = 0; i < len; i++) {
      char c = (char) in.get();
      _name += c;
    }
  }

  bool TraceNameEvent::equals(TraceEvent* e)
  {
    if (TraceTaskEvent::equals(e)) {
      TraceNameEvent* ev = dynamic_cast<TraceNameEvent*>(e);
      if (ev != NULL && ev->_name == _name) return true;
      return false;
    }
    return false;
  }

} // namespace RTSim
