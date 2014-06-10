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
#ifndef __RTSCHED_HPP__
#define __RTSCHED_HPP__

#include <map>

#include <baseexc.hpp>
#include <plist.hpp>
#include <simul.hpp>

#include <scheduler.hpp>
#include <rttask.hpp>

namespace RTSim {

    using namespace MetaSim;

    class AbsKernel;

    /**
       \ingroup sched 
    */
    class RTSchedExc : public BaseExc {
    public:
        RTSchedExc(string msg) : 
            BaseExc(msg, "Scheduler", "rtsched.cpp") {}
    };

    /** 
        \ingroup sched

        Model for real time tasks. This is an abstract class and
        cannot be instantiated.
    */
    class RTModel : public TaskModel {
    protected:
        AbsRTTask* _rtTask;
        bool active;
        int _insertTime;

	int _threshold;

    public:

        /**
           \ingroup sched
       
           This function object is used to order the ready
           queue. Recall, the ready queue contains
           RTModels....
        */
        class RTModelCmp {
        public:
            /* 
               Remember that lower numbers mean higher
               priorities...  This function returns true
               if "a" has higher priority than "b".  This
               function was modified to work also in EDF*:
                           
               - when 2 tasks have the same priority
               (i.e. deadline in EDF*), then the priority
               is decided basing upon the insertion time
               (see setInsertTime() method).  

               - when 2 tasks have the same priority and
               the same insertion time, then the priority
               is decided basing upon the task number
               (lower number => higher priority)

               It is possible to specify a "slice" time
               for each task. This slice time models the
               quantum in round robin schedulers. When a
               task starts executing, it is assigned a
               timeout equal to the slice size. If the
               timeout expires while the task is still
               executing, the task is suspended, removed
               from the ready queue, and re-inserted with
               a higher insertion time. In this way, it is
               like inserting back in the queue of tasks
               with the same priority.

               By using the slice time, it is possible to
               implement the SCHED_RR policy of POSIX, by
               using a fixed priority, and setting the
               slice time for each task.

               @todo check if this works with
               multi-processors (I guess not).
            */
            bool operator()(RTModel* a, RTModel* b);
        };

        /**
         * Constructor.
         */
        RTModel(AbsRTTask* t);

        /**
           Virtual destructor. 
        */
        virtual ~RTModel() {}

        /**
         * Returns a pointer to the task;
         */
        AbsRTTask* getTask() { return _rtTask; }

//         /**
//          * Returns the task arrival time;
//          */
//         Tick getArrival() { return _rtTask->getArrival(); }

        /**
         * Returns the task number;
         */
        int getTaskNumber() { return _rtTask->getTaskNumber(); }
  
        /**
         * Returns the priority of the task.
         */
        virtual Tick getPriority() = 0;

        /**
         * Changes the task priority
         */
        virtual void changePriority(Tick p) = 0;

	/**
         * Returns the preemption threshold of the task t
	 */
	int getThreshold(){return _threshold;}; 

	/**
         * Sets the preemption threshold of the task t.
	 */
	void setThreshold(const int th){ _threshold = th; }; 

        /**
         * Set the active flag of the task. It happens when
         * the task is inserted in the queue.
         */
        void setActive();

        /**
         * Unset the active flag. It happens when the task is
         * extracted from the queue.
         */
        void setInactive();

        /**
         * Returns the active flag.
         */
        bool isActive();

        /**
           Set the insertion time. Used to control the order
           between two tasks with the same priority.
        */
        virtual void setInsertTime(Tick t) {_insertTime = t;}

        /**
           Returns the insertion time.
        */ 
        virtual Tick getInsertTime() {return _insertTime;}
    };

    /** 
        \ingroup sched
      
        This is an abstract class and cannot be instanciated.

        This class models a generic real-time scheduler. It
        implements the Scheduler interface. Basically, this and
        the derived classes manage a priority queue in a
        convenient manner, and offer a clean interface toward the
        kernel and the resource manager.

        The class keeps internally a repository of all tasks that
        can be scheduled by this scheduler. Every time a task is
        "added" to the scheduler, an appropriate RTModel object is
        built, which contains the scheduling paramenters of the
        task. In this way, we clearly separate the task parameters
        (like period, deadline, wcet, etc.)  that are contained in
        the task class, from the scheduling parameters that are
        contained in the RTModel class wrappers.
      
        See the derived classes for more details.
    */
    class Scheduler: public Scheduler
    {
    protected:

        AbsKernel* _kernel;
        priority_list<RTModel*, RTModel::RTModelCmp> _queue;
        map<AbsTask*, RTModel*> _tasks;
	AbsTask* _currExe;

        // stores the old task priorities
        map<AbsTask *, int> oldPriorities;

        /**
           This is the internal version of the addTask, it
           enqueues a model and adds the corresponding task to
           the kernel.
        */
        virtual void enqueueModel(RTModel* model);

        /** 
         * This function returns a RTModel from a task. It is
         * used mainly inside this class, but it can also be
         * used by some resource manager. */
        RTModel* find(AbsTask* task);
    
        friend class PIRManager;

    public:

        /** Constructor */
        Scheduler() :_kernel(0), _queue(), _tasks(), _currExe(0) {}

        /** 
         * Inserts a new task in the queue, at the right
         * position, according to the ordering. The task
         * should have been already added to the scheduler via
         * some addTask() function. For example, look at
         * EDFScheduler::addTask().  If the task is not
         * present in the scheduler, the function throws a
         * RTSchedExc exception.

         * @param t task
         * @see EDFScheduler
         */
        virtual void insert(AbsTask* t) throw(RTSchedExc, BaseExc);

        /**
         * Extracts a task from the queue. The task should be
         * present in the scheduler.  After this function, the
         * function refresh() should be called...
         */
        virtual void extract(AbsTask* t) throw(RTSchedExc, BaseExc);

        /**
         * Returns the priority of task t. Throws an exception
         * if the task does not exist.
         */
        virtual int getPriority(AbsTask* t) throw(RTSchedExc);

	/**
	 * Manages the request of a task to raise his own 
	 * preemption threshold
	 */
	virtual void setThreshold(AbsTask *t, const int th) throw(RTSchedExc);
           

        virtual int enableThreshold(AbsTask* t) throw(RTSchedExc);

        virtual void disableThreshold(AbsTask* t) throw(RTSchedExc);

	/**
         * Returns the preemption threshold of task t. Throws an exception
         * if the task does not exist
         */
	virtual int getThreshold(AbsTask *t) throw(RTSchedExc);

        /**
         * Returns the first task in the queue, or NULL if the
         * queue is empty. Calls the getTaskN();
         */

	virtual AbsTask* getFirst(); 

        /** 
         * Returns a pointer to the N-th task in the
         * queue. Returns NULL if there are less than N tasks
         * in queue.
         *
         * @param n n-th position in the queue (the first task
         * in the queue has position 0).
         */
        virtual AbsTask* getTaskN(unsigned int n);

        /**
           Notifies this class that currExe is the newly
           executing task. This event re-initializes the round
           for the round-robin.
        */
        virtual void notify(AbsTask* currExe);

        /**
         * Sets the kernel in which this queue is
         * contained. This function should never be called by
         * the user directly. It is automatically called by
         * the Kernel() constructor.
         */
        void setKernel(AbsKernel* k);

        /** 
         * Discards all tasks from the scheduler.
         *
         * @param f if true, the tasks are deleted.
         */
        virtual void discardTasks(bool f);
 
        virtual void newRun();
        virtual void endRun();
        virtual void print();

        /**
         * Returns the number of elements in queue.
         */
        virtual int getSize() {return _queue.size();}
    };

} // namespace RTSim

#endif
