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
#ifndef __RMSCHED_HPP__
#define __RMSCHED_HPP__

#include <fpsched.hpp>

namespace RTSim {
    /**
       \ingroup kernels

       This class implements a Rate Monotonic scheduler (or, more
       precisely, a deadline monotonic). Tasks are ordered in the queue by
       their relative deadline. 

       This class redefines only the addTask function, because most of the
       work is done in the Scheduler class.
    */
    class RMScheduler: public FPScheduler
    {
        /** 
            \ingroup kernels
	
            Model for RM.
        */
        class RMModel : public FPScheduler::FPModel
        {
        protected:
            bool extP;
        public:
            RMModel(AbsRTTask *t) : FPModel(t, 0), extP(false) {}
            Tick getPriority() {
                if (extP) return _prio;
                else return _rtTask->getRelDline(); 
            }

            void changePriority(MetaSim::Tick p) {
                if (p == _rtTask->getRelDline()) 
                    extP = false;
                else { 
                    extP = true;
                    _prio = p;
                }
            }

        };

    public:
        /**
         * Creates an RMModel passing the task. It throws a RTSchedExc
         * exception if the task is already present in this scheduler.
         */
        void addTask(AbsRTTask* task) throw (RTSchedExc);

        void addTask(AbsRTTask *t, const std::string &p);

        void removeTask(AbsRTTask *t) { }
    };

}

#endif
