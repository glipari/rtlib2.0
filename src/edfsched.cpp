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
#include <edfsched.hpp>
#include <kernel.hpp>

namespace RTSim {

    void EDFScheduler::addTask(AbsRTTask* task) throw (RTSchedExc)
    {
        enqueueModel(new EDFModel(task));
    }

    void EDFScheduler::addTask(AbsRTTask* task, const std::string &p)
    {
        if (!dynamic_cast<AbsRTTask *>(task)) 
            throw RTSchedExc("Cannot add a AbsRTTask to EDF (should be AbsTask instead");
        // ignoring parameters
        addTask(dynamic_cast<AbsRTTask *>(task));
    }


    void EDFScheduler::removeTask(AbsRTTask* task)
    {
    }


    EDFScheduler * EDFScheduler::createInstance(vector<string> &par)
    {
        // todo: check the parameters (i.e. to set the default
        // time quantum)
        return new EDFScheduler;
    }

}
