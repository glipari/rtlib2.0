/***************************************************************************
    begin                : Feb 20 2010
    copyright            : (C) 2010 by Giuseppe Lipari
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
#ifndef __POLLINGSERVER_H__
#define __POLLINGSERVER_H__

#include <server.hpp>

namespace RTSim {

    class PollingServer : public Server {
    private:
        Tick Q, P;
        Tick cap; 
        Tick last_time;
        Tick recharging_time;
    public:
        PollingServer(Tick q, Tick p, const std::string &name, 
                      const std::string &sched = "FIFOSched");

        void newRun();
        void endRun();

        virtual Tick getBudget() const { return Q;}
        virtual Tick getPeriod() const { return P;}

        Tick changeBudget(const Tick &n);

        /** @todo to be completed */
        virtual double getVirtualTime() {return 0;}

    protected:
                
        /// from idle to active contending (new work to do)
        virtual void idle_ready();

        /// from active non contending to active contending (more work)
        virtual void releasing_ready();
                
        /// from active contending to executing (dispatching)
        virtual void ready_executing();

        /// from executing to active contenting (preemption)
        virtual void executing_ready();

        /// from executing to active non contending (no more work)
        virtual void executing_releasing();

        /// from active non contending to idle (no lag)
        virtual void releasing_idle();

        /// from executing to recharging (budget exhausted)
        virtual void executing_recharging();

        /// from recharging to active contending (budget recharged)
        virtual void recharging_ready();

        /// from recharging to active contending (budget recharged)
        virtual void recharging_idle();
    };
}


#endif
