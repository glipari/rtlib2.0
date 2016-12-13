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
       Models a simple periodic task. It's a simpler interface to
       Task.
    */
    class PeriodicTask: public Task
    {
        Tick period;
    public:
        PeriodicTask(Tick iat);
	
        PeriodicTask(Tick iat, Tick rdl, Tick ph = 0,
                     const std::string &name = "", long qs = 100);
	
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
        static std::unique_ptr<PeriodicTask> createInstance(const vector<string>& par);
    };

} // namespace RTSim 

#endif
