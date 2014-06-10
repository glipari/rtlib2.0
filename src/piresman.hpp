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
#ifndef __PIRESMAN_HPP__
#define __PIRESMAN_HPP__

#include <map>

#include <plist.hpp>

#include <resmanager.hpp>
#include <scheduler.hpp>

#define _PIRESMAN_DBG_LEV  "piresman"

namespace RTSim {

//   using namespace std;
    using namespace MetaSim;

    /**
       \ingroup resman

       This class implements the Priority Inheritance Protocol. It has the
       same interface of any Resource Manager class, in addition it has a
       setScheduler() function, because the PI needs the scheduler for
       changing the priority of a task.
    */
    class PIRManager : public ResManager {
//         Scheduler *_sched;
    public:

        /**
         * Constructor
         */
        PIRManager(const std::string &n = "");
  
        /**
         * Sets the scheduler for this resmanager
         */
//         void setScheduler(Scheduler *s);

        /**
         * @todo clear the maps!!
         */
        void newRun();

        void endRun();

    protected:
        /**
           Returns true if the resource can be locked, false otherwise
           (in such a case, the task should be blocked)
         */
        virtual bool request(AbsRTTask *t, Resource *r, int n=1);
        
        /**
           Releases the resource.
         */
        virtual void release(AbsRTTask *t, Resource *r, int n=1);

    private:
        /// correspondence task / priority 
        typedef map<std::string, int> PRIORITY_MAP;
        
        /// Blocked tasks, ordered by priority. 
        /// There is one such queue for each resource
        typedef priority_list<TaskModel *, TaskModel::TaskModelCmp> BLOCKED_QUEUE;

        // Stores the old task priorities
        map<AbsRTTask *, PRIORITY_MAP> oldPriorities;

        // stores the blocked tasks for each resource, ordered by priority
        map<string, BLOCKED_QUEUE> blocked;
    };
}

#endif
