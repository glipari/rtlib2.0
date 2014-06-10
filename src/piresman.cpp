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
#include <simul.hpp>

#include <abskernel.hpp>
#include <piresman.hpp>
#include <resource.hpp>

namespace RTSim {

    using namespace std;
    using namespace MetaSim;


    PIRManager::PIRManager(const string &n) : ResManager(n)
    {
    }

//     void PIRManager::setScheduler(Scheduler *s)
//     {
//         _sched = s;
//     }

    void PIRManager::newRun()
    {
    }

    void PIRManager::endRun()
    {
    }

    bool PIRManager::request(AbsRTTask *t, Resource *r, int n)
    {
        bool ret;

        DBGENTER(_PIRESMAN_DBG_LEV);

        TaskModel* taskModel = _sched->find(t);

        if (taskModel == NULL) {
            throw BaseExc("Cannot find task model!");
        }

        // if resource is not free...
        if (r->isLocked()) {
            DBGPRINT("Resource is locked");
            // Task that owns the resource
            AbsRTTask *owner = dynamic_cast<AbsRTTask *>(r->getOwner());
            TaskModel* ownerModel = _sched->find(owner);

            if (ownerModel == NULL) {
                throw BaseExc("Cannot find owner model!");
            }   

            // remove the owner from the scheduler. It is not executing.
            _sched->extract(owner);

            // suspend the task.
            _kernel->suspend(t);

            DBGPRINT("Raising priority");
            // owner priority = task priority
            ownerModel->changePriority(taskModel->getPriority());

            // reactivate the owner with the new priority
            _kernel->activate(owner);

            // push the blocked task into the blocked queue
            blocked[r->getName()].insert(taskModel);
    
            ret = false;
        }
        else {
            // save owner's priority
            DBGPRINT("Storing old priority");

            // (if the entry in oldPriorities does not exist?)
            if (oldPriorities.find(t) == oldPriorities.end()) {
                PRIORITY_MAP mm;
                mm[r->getName()] = taskModel->getPriority();
                oldPriorities[t] = mm;
            }
            else {
                (oldPriorities[t])[r->getName()] = taskModel->getPriority(); 
            }

            r->lock(t);
            ret = true;
        }

        

        return ret;
    }

    void PIRManager::release(AbsRTTask *t, Resource *r, int n)
    {
        TaskModel* taskModel = _sched->find(t);

        DBGENTER(_PIRESMAN_DBG_LEV);

        r->unlock();
        // see if there is any blocked task
        if (!blocked[r->getName()].empty()) 
        {
            TaskModel *newTaskModel = blocked[r->getName()].front();
            blocked[r->getName()].erase(newTaskModel);
            _kernel->suspend(t);
            taskModel->changePriority((oldPriorities[t])[r->getName()]);
            if (t->isActive()) _kernel->activate(t);
            _kernel->activate(newTaskModel->getTask());
            r->lock(newTaskModel->getTask());
        }

        
    }

} // namespace RTSim
