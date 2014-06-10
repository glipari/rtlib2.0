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
#include <fifosched.hpp>
#include <kernel.hpp>

namespace RTSim {

    void FIFOScheduler::addTask(AbsRTTask *task) throw(RTSchedExc)
    {
        FIFOModel *model = new FIFOModel(task); 	

        if (find(task) != NULL) 
            throw RTSchedExc("Element already present");
	
        _tasks[task] = model;
    }

    void FIFOScheduler::addTask(AbsRTTask* task, const std::string &p)
    {
        if (!dynamic_cast<AbsRTTask *>(task)) 
            throw RTSchedExc("Cannot add a AbsRTTask to FIFO");
        // ignoring parameters
        addTask(dynamic_cast<AbsRTTask *>(task));
    }

    FIFOScheduler * FIFOScheduler::createInstance(vector<string> &par)
    {
        // todo: check the parameters (i.e. to set the default
        // time quantum)
        return new FIFOScheduler;
    }
}
