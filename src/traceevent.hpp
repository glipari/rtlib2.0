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
 * $Id: traceevent.hpp,v 1.1 2005/11/18 17:08:57 cesare Exp $
 *
 * $Log: traceevent.hpp,v $
 * Revision 1.1  2005/11/18 17:08:57  cesare
 * Improved the Tracer.
 *
 */
#ifndef __TRACEEVENT_HPP__
#define __TRACEEVENT_HPP__

#include <fstream>

#include <baseexc.hpp>

namespace RTSim {

  using namespace std;

  class Desc
  {
  public:
    string name;
    int tid;

    Desc(string n, int id) :name(n), tid(id) {}
    Desc(int id) :name(""), tid(id) {}

    virtual ~Desc() {}
  };

  class TraceEvent
  {
  protected:
    int _type;
    int _time;
 
  public:
    static const int TASK_ARRIVAL = 0;
    static const int TASK_SCHEDULE = 1;
    static const int TASK_DESCHEDULE = 2;
    static const int TASK_END = 3;
    static const int TASK_DLINEPOST = 4;
    static const int TASK_DLINESET = 5;
    static const int TASK_WAIT = 6;
    static const int TASK_SIGNAL = 7;
    static const int TASK_IDLE = 8;
    static const int TASK_NAME = 9;
    static const int TASK_DLINEMISS = 10;
    static const int EVT_NUMBER = 11;

    static const string evt_name[EVT_NUMBER];
 
    TraceEvent() {}
    TraceEvent(int type, int t) :_type(type), _time(t) {}
    virtual ~TraceEvent() {}
 
    static void encode(char* in, int size);

    virtual unsigned int getSize() const
    {return sizeof(_type) + sizeof(_time);}
    int getType() const {return _type;}
    int getTime() const {return _time;}

    /**
       This function builds and returns the proper TraceEvent object
       starting from the Trace type returns null if for some reason
       the object could not be created!
     */
    static const TraceEvent* getEvent(int actType);

    /**
       Returns a descriptive string: used for debug.
     */
    virtual string print() = 0;

    /**
       Returns a string used for displaying text info.
     */
    virtual string getDescription() = 0;

    /**
       true if there is something to draw in the temporal window that
       begins at t.
     */
    virtual bool somethingToDraw(int t);

    virtual void write(ofstream& f);

    virtual void read(ifstream& in);

    virtual bool equals(TraceEvent* e);

  }; 

  class TraceTaskEvent: public TraceEvent
  {
  protected:
    int _task;

  public:
    Desc* tdesc;

    TraceTaskEvent() :tdesc(NULL) {}

    TraceTaskEvent(int type, int time, int task)
      :TraceEvent(type, time), _task(task) {}

    virtual ~TraceTaskEvent() {}

    virtual unsigned int getSize() const
    {return TraceEvent::getSize() + sizeof(_task);}

    int getTask() const {return _task;}
 
    virtual string print();

    virtual string getDescription();

    virtual void write(ofstream& f);
 
    virtual void read(ifstream& in);

    virtual bool equals(TraceEvent* e);

  };

  class TraceArrEvent: public TraceTaskEvent
  {
  public:
    TraceArrEvent() {_type = TASK_ARRIVAL;}

    TraceArrEvent(int time, int task)
      :TraceTaskEvent(TASK_ARRIVAL, time, task) {}

  };

  class TraceCPUEvent: public TraceTaskEvent
  {
  protected:
    int _cpu;

  public:
    TraceCPUEvent() {}

    TraceCPUEvent(int type, int time, int task, int cpu)
      :TraceTaskEvent(type, time, task), _cpu(cpu) {}

    virtual unsigned int getSize() const
    {return TraceTaskEvent::getSize() + sizeof(_cpu);}

    int getCPU() const {return _cpu;}

    virtual string print();

    virtual string getDescription();

    virtual void write(ofstream& f);

    virtual void read(ifstream& in);

  };

  class TraceSchedEvent: public TraceCPUEvent
  {
  public:
    TraceSchedEvent() {_type = TASK_SCHEDULE;}

    TraceSchedEvent(int time, int task, int cpu)
      :TraceCPUEvent(TASK_SCHEDULE, time, task, cpu) {}

  };

  class TraceDeschedEvent: public TraceCPUEvent
  {
  public:
    TraceDeschedEvent() {_type = TASK_DESCHEDULE;}

    TraceDeschedEvent(int time, int task, int cpu)
      :TraceCPUEvent(TASK_DESCHEDULE, time, task, cpu) {}

  };

  class TraceEndEvent : public TraceCPUEvent
  {
  public:
    TraceEndEvent() {_type = TASK_END;}

    TraceEndEvent(int time, int task, int cpu)
      :TraceCPUEvent(TASK_END, time, task, cpu) {}

  };

  class TraceDlinePostEvent: public TraceTaskEvent
  {
  protected:
    int _taskD;
    int _taskD2;

  public:
    TraceDlinePostEvent() {_type = TASK_DLINEPOST;}

    TraceDlinePostEvent(int time, int task, int dline, int dline2)
      :TraceTaskEvent(TASK_DLINEPOST, time, task), _taskD(dline), _taskD2(dline2) {}

    virtual ~TraceDlinePostEvent() {}

    virtual unsigned int getSize() const
    {return TraceTaskEvent::getSize() + sizeof(_taskD) + sizeof(_taskD2);}

    int getTaskD() const {return _taskD;}
    int getTaskD2() const {return _taskD2;}

    string print();

    string getDescription();

    bool somethingToDraw(int t);

    virtual void write(ofstream& f);

    void read(ifstream& in);

    bool equals(TraceEvent* e);

  };

  class TraceDlineSetEvent: public TraceTaskEvent
  {
  protected:
    int _taskD;

  public:
    TraceDlineSetEvent() {_type = TASK_DLINESET;}

    TraceDlineSetEvent(int time, int task, int dline)
      :TraceTaskEvent(TASK_DLINESET, time, task), _taskD(dline) {}

    virtual ~TraceDlineSetEvent() {}
 
    virtual unsigned int getSize() const
    {return TraceTaskEvent::getSize() + sizeof(_taskD);}

    int getTaskD() const {return _taskD;}

    string print();

    string getDescription();

    bool somethingToDraw(int t);

    virtual void write(ofstream& f);

    void read(ifstream& in);

    bool equals(TraceEvent* e);

  };      

  class TraceWaitEvent: public TraceTaskEvent
  {
  protected:
    string _res;  

  public:
    TraceWaitEvent() {_type = TASK_WAIT;}

    TraceWaitEvent(int time, int task, string res)
      :TraceTaskEvent(TASK_WAIT, time, task), _res(res) {}

    virtual ~TraceWaitEvent() {}

    virtual unsigned int getSize() const;

    string getResource() const {return _res;}

    string print();

    string getDescription();

    virtual void write(ofstream& f);

    void read(ifstream& in);

    bool equals(TraceEvent* e);

  };

  class TraceSignalEvent: public TraceTaskEvent
  {
  protected:
    string _res;  

  public:
    TraceSignalEvent() {_type = TASK_SIGNAL;}

    TraceSignalEvent(int time, int task, string res)
      :TraceTaskEvent(TASK_SIGNAL, time, task), _res(res) {}

    virtual ~TraceSignalEvent() {}

    virtual unsigned int getSize() const;

    string getResource() const {return _res;}

    string print();

    string getDescription();

    virtual void write(ofstream& f);

    void read(ifstream& in);

    bool equals(TraceEvent* e);

  };

  class TraceIdleEvent: public TraceTaskEvent
  {
  public:
    TraceIdleEvent() {_type = TASK_IDLE;}

    TraceIdleEvent(int time, int task)
      :TraceTaskEvent(TASK_IDLE, time, task) {}

  };

  class TraceNameEvent: public TraceTaskEvent
  {
  protected:
    string _name;

  public:
    TraceNameEvent() {_type = TASK_NAME;}

    TraceNameEvent(int time, int task, const string &s)
      :TraceTaskEvent(TASK_NAME, time, task), _name(s) {}

    virtual ~TraceNameEvent() {}

    virtual unsigned int getSize() const;

    string getName() {return _name;}

    string getDescription();

    virtual void write(ofstream& f);

    void read(ifstream& in);

    bool equals(TraceEvent* e);

  };

  class TraceDlineMissEvent: public TraceTaskEvent
  {
  public:
    TraceDlineMissEvent() {_type = TASK_DLINEMISS;}

    TraceDlineMissEvent(int time, int task)
      :TraceTaskEvent(TASK_DLINEMISS, time, task) {}

  };

} //namespace RTSim

#endif
