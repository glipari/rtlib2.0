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
#ifndef __RRSCHED_HPP__
#define __RRSCHED_HPP__

#include <map>

#include <baseexc.hpp>
#include <plist.hpp>
#include <simul.hpp>
#include <gevent.hpp>

#include <scheduler.hpp>

#define _RR_SCHED_DBG_LEV "RRSched"

namespace RTSim {

    using namespace MetaSim;

    class RTKernel;

    /** 
        \ingroup sched
        
        Round Robin scheduler. 
    */
    class RRScheduler: public Scheduler
    {
    protected:

        /**
           \ingroup sched 
        */
        class RRSchedExc : public BaseExc {
        public:
            RRSchedExc(string msg) : 
                BaseExc(msg, "RoundRobinScheduler", "rrsched.cpp") {}
        };

        class RRModel : public TaskModel {
        protected:
            Tick _rrSlice;      

        public:

            RRModel(AbsRTTask* t) : TaskModel(t), _rrSlice(1) {}
            virtual ~RRModel() {}

            virtual Tick getPriority();
            virtual void changePriority(Tick p);

            /**
               Returns the slice size (in number of ticks)
            */
            Tick getRRSlice() {return _rrSlice;}

            /** 
                Sets the slice size to s (in number of ticks)
            */
            void setRRSlice(Tick s) {_rrSlice = s;}

            /**
               This function returns true if the round has expired for the
               currently executing thread.
            */
            bool isRoundExpired();
        };

        GEvent<RRScheduler> _rrEvt;
    
        int defaultSlice;

    public:

        /** Constructor */
        RRScheduler(int defSlice);

        /**
           Set the Round Robin slice.
        */
        virtual void setRRSlice(AbsRTTask* task, Tick slice);

        /**
           Notify to recompute the round
        */
        virtual void notify(AbsRTTask* task);

        /**
           This is called by the event rrEvt.
        */
        void round(Event *);

        void addTask(AbsRTTask *t) throw(RRSchedExc);

        void addTask(AbsRTTask *t, const std::string &p);

        void removeTask(AbsRTTask *t) {}

        static RRScheduler *createInstance(vector<string> &par);
    };

} // namespace RTSim

#endif
