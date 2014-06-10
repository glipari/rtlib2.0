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
 * $Id: rmsched.cpp,v 1.4 2005/04/28 01:34:48 cesare Exp $
 *
 * $Log: rmsched.cpp,v $
 * Revision 1.4  2005/04/28 01:34:48  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.3  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#include <kernel.hpp>
#include <rmsched.hpp>

namespace RTSim {

    void RMScheduler::addTask(AbsRTTask* task) throw (RTSchedExc)
    {
        enqueueModel(new RMModel(task));
    }

    void RMScheduler::addTask(AbsRTTask* task, const std::string &p)
    {
        AbsRTTask *t = dynamic_cast<AbsRTTask *>(task);

        if (t != 0) enqueueModel(new RMModel(t));
        else throw RTSchedExc("Cannot add a AbsRTTask to RM");

    }

}
