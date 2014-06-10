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
#ifndef __RTTASK_HPP__
#define __RTTASK_HPP__

#include <cstdlib>
#include <cstring>

#include <regvar.hpp>
#include <simul.hpp>

#include <task.hpp>

namespace RTSim {

    using namespace std;
    using namespace MetaSim;

    /**
       Task with real-time properties. It modifies a few things:
     
       - when a job arrives, the corresponding deadline is set
       (the class Task has no deadline parameter).

       - it adds a deadline event to check deadline misses; it can
       abort the simulation in case of deadline miss (depending on
       the abort parameter in the constructor).

       @todo generalize the abort strategy for aborting only the
       task, or only the current instance of the task.
    */
//     class RTTask: public Task, public virtual AbsRTTask
//     {
//     protected:
//         Tick _dl;
//         Tick _rdl;

//         virtual void handleArrival(Tick arr) 
//             {
//                 Task::handleArrival(arr);
//                 _dl = getArrival() + _rdl;
//                 if (_dl >= SIMUL.getTime()) deadEvt.post(_dl);
//             }

//         virtual void onEnd(MetaSim::Event* e) 
//             {
//                 deadEvt.drop(); Task::onEnd(e);
//             }

//         virtual void onSched(MetaSim::Event* e)
//             {
//                 Task::onSched(e);
//                 deadEvt.setCPU(getCPUIndex());
//             }

//     public:
//         DeadEvt deadEvt;

//         /**
//            This constructor is a simple interface to the Task
//            constructor, that adds the relative deadline
//            parameter rdl.
//         */
//         RTTask(RandomVar* iat, Tick rdl, Tick ph = 0,
//                const std::string &name = "", long qs = __LONG_MAX__, 
//                bool abort = true)
//             :Task(iat, ph, name, qs), _rdl(rdl), 
//              deadEvt(this, abort)
//             {
//                 _dl = 0;
//             }

//         /** 
//             Used to build tasks with the Factory. The string
//             must contain a set of comma separated values, in
//             the same order as in the constructor:

//             - interarrival time, deadline, phase

//             the interarrival time can be 0 (no interarrival at
//             all, so non-cyclic task), or it can be a string
//             specification of a random variable (for example
//             unif(10,20)). See RandomVar in MetaSim for more
//             information.

//             Then a set of optional paramters can be given:

//             - name (a string), queuesize, abort (true/false)

//             Please take into account that at least 3 arguments
//             of numerical type must be given!

//             Example:

//             - RTTask *p = RTTask::createInstance("unif(10,20),
//             10, 0, task1");

//             is the same as

//             - RTTask *p = new RTTask(new UniformVar(10,20),
//             10, 0, "task1");
//         */
//         static RTTask* createInstance(vector<string> &par)
//             {
//                 RandomVar* i = NULL;
//                 if (strcmp(par[0].c_str(), "0")) i = parsevar(par[0]);
//                 Tick d = Tick(par[1]);
//                 Tick p = Tick(par[2]);
//                 const char* n = "";
//                 if (par.size() > 2) n = par[3].c_str();
//                 long q = __LONG_MAX__;
//                 if (par.size() > 4) q = atoi(par[4].c_str());
//                 bool a = true;
//                 if (par.size() > 5 && !strcmp(par[5].c_str(), "false")) a = false;
//                 RTTask* t = new RTTask(i, d, p, n, q, a);
//                 return t;
//             }

//         virtual Tick getDeadline() const {return _dl;}
//         virtual Tick getRelDline() const {return _rdl;}

//         /**
//            Tells the simulator that this task needs to be
//            traced.
       
//            @todo add a traceall function somewhere.
//         */
//         void setTrace(Trace* t) 
//             {
//                 deadEvt.addTrace(t); Task::setTrace(t); 
//             }

//         virtual void newRun()
//             { 
//                 _dl = 0; Task::newRun(); 
//             }
//         virtual void endRun() 
//             { 
//                 deadEvt.drop(); Task::endRun();
//             }
  
//         void setAbort(bool f) 
//             { 
//                 deadEvt.setAbort(f); 
//             }

//         /* Function inherited from class Entity */
//         int getTaskNumber() const
//             {
//                 return getID(); 
//             }	

//         /* Function inherited from class AbsRTTask */
//         Tick getMaxExecutionTime()
//             {
//                 return _maxC; /* _maxC is inherited from class
//                                  Task */
//             }
//     };

    /**
       Models a simple periodic task. It's a simpler interface to
       RTTask, just for constructing new tasks.
    */
    class PeriodicTask: public Task
    {
        Tick period;
    public:
        PeriodicTask(Tick iat)
            : Task(new DeltaVar(iat), iat, 0, "", 
                   1000), period(iat) {}

        PeriodicTask(Tick iat, Tick rdl, Tick ph = 0,
                     const std::string &name = "", long qs = 100)
            : Task(new DeltaVar(iat), rdl, ph, name, qs), period(iat) {}

        inline Tick getPeriod() { return period; } 


        /** Used to build tasks with the Factory.  The string
            must contain a set of comma separated values, in
            the same order as in the constructor: 

            - period, deadline, phase

            Then a set of optional paramters can be given: -
            name (a string), queuesize, abort (true/false)

            Please take into account that at least 3 arguments of
            numerical type must be given!

            Example: 
            - PeriodiTask *p = PeriodicTask::createInstance("10, 10, 0, task1");

            is the same as

            - PeriodicTask *p = new PeriodicTask(10, 10, 0, "task1");
	
        */
        static PeriodicTask* createInstance(vector<string>& par)
            {
                Tick i = Tick(par[0]);
                Tick d = Tick(par[1]);
                Tick p = Tick(par[2]);
                const char* n = "";
                if (par.size() > 2) n = par[3].c_str();
                long q = 100;
                if (par.size() > 4) q = atoi(par[4].c_str());
                bool a = true;
                if (par.size() > 5 && !strcmp(par[5].c_str(), "false")) a = false;
                return new PeriodicTask(i, d, p, n, q);
            }
    };

} // namespace RTSim 

#endif
