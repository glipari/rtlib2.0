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
#ifndef __TASKSTAT_HPP__
#define __TASKSTAT_HPP__

#include <string>
#include <cassert>

#include <baseexc.hpp>
#include <basestat.hpp>

#include <task.hpp>

namespace RTSim {

    using namespace MetaSim;

    const char _NEED_A_TASK[] = "Expecting a task argument";
    const char _NEED_A_RTTASK[] = "Expecting a RT task argument";
    const char _WRONG_EVENT[] = "Receiving the wrong event"; 

    class TaskStatExc : public BaseExc {
    public:
        TaskStatExc(string msg, string cl, string md = "taskstat.hpp") :
            BaseExc(msg, cl, md) {}

    };

    /**
       Abstract Statistical Probe Definitions; the user need to combine
       an abstract probe referring the quantity to measure with the kind
       of measure to do over it
 
       Ex: class MeanFTStat : public FinishingTimeStat<MeanStat>;
    */

    /**
       \ingroup measures

       Counts the number of preemptions for a single job of a single
       task.
    */
    template <class Measure>
    class PreemptionStat : public Measure {
        int count;
        Tick descTime;
        Tick schedTime;
    public:
        PreemptionStat(string name = "") : Measure(name) {};
 
        void probe(const DeschedEvt &e) {
            if (e.getLastTime() != schedTime) {
                descTime = e.getLastTime();
                count ++;
            }
            if (e.getLastTime() < Measure::_transitory)
                count = 0;
        }

        void probe(const SchedEvt &e) {
            if (e.getLastTime() != descTime) 
                schedTime = e.getLastTime();
            else count --;
            if (e.getLastTime() < Measure::_transitory)
                count = 0;
        }

        void probe(const EndEvt &e) {
            Measure::record(count);
            count = 0;
        }

        //virtual 
        void attachToTask(Task *t) 
            {
                new Particle<SchedEvt, PreemptionStat>(&t->schedEvt, this);
                new Particle<DeschedEvt, PreemptionStat>(&t->deschedEvt, this);
                new Particle<EndEvt, PreemptionStat>(&t->endEvt, this);
            }
    };

    /** 
        \ingroup measures

        Counts the total number of preemptions.
    */
    class GlobalPreemptionStat : public StatCount {
        int idSched;
        int idDesched;
        Tick descTime;
        Tick schedTime; 
    public:
  
        GlobalPreemptionStat(string name = "") : StatCount(name) { 
            idSched = -1;
            idDesched = -1;
            descTime = MAXTICK;
            schedTime = MAXTICK;
        }

        void probe(const SchedEvt &se)
            {
                if (SIMUL.getTime() < _transitory) return;
                if (se.getTime() == descTime && se.getTask()->getID() == idDesched)
                    record(-1);
            }

        void probe(const DeschedEvt &de)
            {
                if (SIMUL.getTime() < _transitory) return;
                descTime = SIMUL.getTime();
                idDesched = de.getTask()->getID();
                record(1);
            }

        virtual void attachToTask(Task *t)
            {
                new Particle<SchedEvt, GlobalPreemptionStat>(&t->schedEvt, this);
                new Particle<DeschedEvt, GlobalPreemptionStat>(&t->deschedEvt, this);
            }

        virtual void initValue() {
            idSched = -1;
            idDesched = -1;
            descTime = MAXTICK;
            schedTime = MAXTICK;
            StatCount::initValue();
        }
    };

    // --------------------------------------------------------------

    /** 
        \ingroup measures

        Measures the finishing time of a task.
    */
    template <class Measure>
    class FinishingTimeStat : public Measure {
    public:

        FinishingTimeStat(string name = "") : Measure(name) {};

        void probe(const EndEvt &ee) 
            {
                if (ee.getLastTime() < Measure::_transitory) return;
                Task *t = ee.getTask();
                Measure::record(ee.getLastTime() - t->getLastArrival());
            }

        void attachToTask(Task *t)
            {
                new Particle<EndEvt, FinishingTimeStat>(&t->endEvt, this);
            }
    };

    /**
       \ingroup measures

       Measures the lateness of a task. The lateness is defined as
       max(0,f-d), where f is the finishing time and d is the deadline.
       Measuring the lateness of more than one task does not make much
       sense...
    */
    template <class Measure>
    class LatenessStat : public Measure {
    public:
        LatenessStat(string name = "") : Measure(name){};

        void probe(const EndEvt &ee)
            {
                if (ee.getLastTime() < Measure::_transitory) return;

                Task *t = ee.getTask();
                Tick f = ee.getLastTime();
                Tick d = t->getDeadline();
                if (f > d) Measure::record (f - d);
                else Measure::record(0);
            }

        void attachToTask(Task *t) 
            {
                new Particle<EndEvt, LatenessStat>(&t->endEvt, this); 
            }
    };

    /**
       \ingroup measures

       Measures the tardiness of a task. Similar to the lateness,
       measures \f$\max(0, \frac{(f-d)}{D})\f$, where \f$ f \f$ is the
       finishing time, \f$ d \f$ is the absolute deadline, \f$ D \f$ is
       the relative deadline. You can also measure the global tardiness
       of the system (it makes sense, because the tardiness is relative
       to the relative deadline).
    */
    template <class Measure>
    class TardinessStat : public Measure {
    public:
        TardinessStat(string name = "") : Measure(name){};

        void probe(const EndEvt &ee) 
            {
                if (ee.getLastTime() < Measure::_transitory) return;

                Task *t = (Task *)ee.getTask();
                double f = (double)ee.getLastTime();
                double a =(double)t->getLastArrival();
                double D = (double)t->getRelDline();
                Measure::record(max(0.0,(f-a-D)/D));
            }

        void attachToTask(Task *t)
            {
                new Particle<EndEvt, TardinessStat>(&t->endEvt, this);
            }
    };

    /**
       \ingroup measures

       Measures the utilization of a task. This can be used for measuring
       the "real" utilization.
    */
    template <class Measure>
    class UtilizationStat : public Measure {
    public:
        UtilizationStat(string name = "") : Measure(name){};
    
        void probe(const EndEvt &ee)
            {
                if (ee.getLastTime() < Measure::_transitory) return;

                Task *t = (Task *)ee.getTask();
                double ex = (double)t->getExecTime();
                double a =(double)t->getLastArrival();
                double d = (double)t->getDeadline();
                Measure::record(ex / (d-a));
            }

        void attachToTask(Task *t)
            {
                new Particle<EndEvt, UtilizationStat>(&t->endEvt, this);
            }
    };

    /**
       \ingroup measures

       Computes the miss percentage. It can be applied to a single task or
       to the entire task set.
    */
    class MissPercentage : public StatPercent {
    public:
        MissPercentage(string name = "") : StatPercent(name) {};

        void probe(const EndEvt &ee)
            {
                if (ee.getLastTime() < _transitory) return;

                Task *task = (Task *) ee.getTask();
                if (SIMUL.getTime() > task->getLastArrival() + 
                    task->getRelDline()) {
                    record(1.0);
                }
                else record(0.0);
            }

        void attachToTask(Task *t)
            {
                new Particle<EndEvt, MissPercentage>(&t->endEvt, this);
            }
    };

  
    /**
       \ingroup measures

       Computes the number of deadline misses . It can be applied to a
       single task or to the entire task set.
    */
    class MissCount : public StatCount {
    public:
        MissCount(string name = "") : StatCount(name) {};

        void probe(const DeadEvt &e) {record(1.0);}

        void attachToTask(Task *t) 
            {
                new Particle<DeadEvt, MissCount>(&t->deadEvt, this);
            }
    };


    /**
       \ingroup measures
     
       Computes the power consumed during the simulation. It is
       performed through periodic sampling of the CPU speed.
    */
    template <class Measure>
    class ConsumedPower : public Measure {
    protected:
        CPU *cpu;
        PeriodicTimer mytimer;
    public:
        ConsumedPower(CPU* c, string name="") : Measure (name), cpu(c), mytimer(10) 
            {
                new Particle<MetaSim::GEvent<Timer>, ConsumedPower>(&mytimer._triggerEvt, this);
            }

        virtual void probe(const MetaSim::GEvent<Timer> &e)
            {
                Measure::record(cpu->getCurrentPowerConsumption() / 
                                cpu->getMaxPowerConsumption());
            }
    
    };

    /**
       \ingroup measures
     
       Computes the power saved during the simulation (with respect to
       the maximum power). It is performed through periodic sampling of
       the CPU speed.
    */
    template <class Measure>
    class SavedPower : public ConsumedPower<Measure> {
    public:
        SavedPower(CPU* cpu, string name="") : ConsumedPower<Measure>(cpu,name) {}

        virtual void probe(const MetaSim::GEvent<Timer> &e)
            {
                record(ConsumedPower<Measure>::cpu->getCurrentPowerSaving());
            }

    };

} // namespace RTSim

#endif
