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
#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <baseexc.hpp>
#include <plist.hpp>
#include <simul.hpp>
#include <entity.hpp>
#include <abstask.hpp>

namespace RTSim {

    using namespace MetaSim;

    class AbsKernel;

    
    const std::string _SCHED_DBG_LEVEL = "SCHEDULER_DEBUG";
    
    /**
       \ingroup sched 
    */
    class RTSchedExc : public BaseExc {
    public:
        RTSchedExc(string msg) : 
            BaseExc(msg, "Scheduler", "rtsched.cpp") {}
    };
 
    /**
       \ingroup kernels
     
       Contains the scheduling parameters and a pointer to the
       task. It is used by a scheduler to store the pointer to the
       task and the set of scheduling parameters.
     
       Each scheduler has its own task model. So the class inheritance
       trees of the task models and of the schedulers are similar.  
    */
    class TaskModel {
    protected:
        AbsRTTask* _rtTask;
        bool active;
        int _insertTime;
        int _threshold;
        Tick _savedPriority;

    public:
        TaskModel(AbsRTTask *t);
        virtual ~TaskModel();

        /// Returns a pointer to the task
        AbsRTTask *getTask() { return _rtTask; }

        /// Returns the priority of the task. It depends on the scheduler
        virtual Tick getPriority() const = 0;

        /// Returns the task's preemption level. It depends on the scheduler
        //virtual Tick getPreemptionLevel() const = 0;
        
        /// Returns the task number
        int getTaskNumber() { return _rtTask->getTaskNumber(); }

        /// changes the task's priority
        virtual void changePriority(Tick p) = 0;

        /// TODO deprecated
        int getThreshold() __attribute__((deprecated))
            { return _threshold; } 

        /// TODO deprecated
        void setThreshold(const int th) __attribute__((deprecated))
            { _threshold = th; }

        /**
           This function raises the threshold of the current task. This is equivalent to :
           - saving the old value of the priority 
           - setting the new priority value to th.

           You must make sure that the task model on which this
           function is called MUST BE the highest priority task.
         */
        void raiseThreshold() __attribute__((deprecated));

        /**
           Restores the original task priority. This is the opposite
           of raiseThreshold, and must be performed on the currently
           executing task, after it has been removed from the
           queue. In some cases, it may cause a change of context.
         */
        void restorePriority() __attribute__((deprecated));

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
        void setInsertTime(Tick t) { _insertTime = t; }

        /**
           Returns the insertion time.
        */ 
        Tick getInsertTime() { return _insertTime; }

        class TaskModelCmp {
        public:
            /* 
               Remember that lower numbers mean higher priorities...

               This function returns true if "a" has higher priority
               than "b". 
                           
               - when 2 tasks have the same priority (i.e. deadline in
               EDF*), then the priority is decided basing upon the
               insertion time (see setInsertTime() method).

               - when 2 tasks have the same priority and the same
               insertion time, then the priority is decided based
               upon the task number (lower number => higher priority)

               It is possible to specify a "slice" time for each
               task. This slice time models the quantum in round robin
               schedulers. When a task starts executing, it is
               assigned a timeout equal to the slice size. If the
               timeout expires while the task is still executing, the
               task is suspended, removed from the ready queue, and
               re-inserted with a higher insertion time. In this way,
               it is like inserting back in the queue of tasks with
               the same priority.

               By using the slice time, it is possible to implement
               the SCHED_RR policy of POSIX, by using a fixed
               priority, and setting the slice time for each task.
            */
            bool operator()(TaskModel* a, TaskModel* b) const;
        };
    };

    /** 
        \ingroup kernels

        Implements the scheduling policy for a set of tasks. Tipically
        a scheduler contains a queue of task models. The
        responsibility of this class is to mantain the queue. 
    */
    class Scheduler : public MetaSim::Entity {
    public:

        /**
           Default constructor
        */
        Scheduler();

        /**
           Virtual destructor
        */
        virtual ~Scheduler();

        /**
           Sets the kernel for this scheduler.
        */
        virtual void setKernel(AbsKernel* k);

        /**
           Add a task with the proper scheduling parameters
        */
        virtual void addTask(AbsRTTask *task, const std::string &params) = 0;

        /**
           Remove a task from this scheduler
        */
        virtual void removeTask(AbsRTTask *task) = 0;

        /**
         * Insert a task in the queue.
         */
        virtual void insert(AbsRTTask *) throw(RTSchedExc, BaseExc);

        /**
         *  extract a task from the queue.
         */
        virtual void extract(AbsRTTask *) throw(RTSchedExc, BaseExc);

        /** returns the priority of the task */
        int getPriority(AbsRTTask* task) const throw(RTSchedExc);

        /** raises the threshold of the task */
        void enableThreshold(AbsRTTask* t) throw(RTSchedExc);
        /** lowers the threshold of the task */
        void disableThreshold(AbsRTTask* t) throw(RTSchedExc);

        /**
         * Sets the preemption threshold of task t. Throws an
         * exception if the task does not exist. 
         *
         * Note that the preemption threshold is currently a constant
         * (integer) value. Different schedulers needs to interpret
         * this value differently. For example, in EDF this would be a
         * relative deadline, in FixedPriority it is just a priority,
         * in RRSched it makes no sense at all. This makes the
         * interface not really robust. 
         */        
        void setThreshold(AbsRTTask *t, int th) throw(RTSchedExc);

        /**
         * Returns the preemption threshold of task t. Throws an
         * exception if the task does not exist or if the scheduler
         * does not support preemption thresholds
         */
         int getThreshold(AbsRTTask *t) throw(RTSchedExc);


        /**
         *  returns the first task in the queue, or NULL if
         *  the queue is empty.
         */
        virtual AbsRTTask *getFirst();
  
        /**
         *  returns the (n+1)-th (0==first) task in the queue
         *  or NULL if the queue has less than n+1 elements.
         */ 
        virtual AbsRTTask * getTaskN(unsigned int);

        /**
         * Returns the number of elements in queue.
         */
        virtual int getSize() { return _queue.size(); }


        /**
           Notify the scheduler that the task has been
           dispatched and it is now executing. This function
           is useful for some schedulers (for example RR).
        */
        virtual void notify(AbsRTTask *);


        /** 
         * Discards all tasks from the scheduler.
         *
         * @param f if true, the tasks are deleted.
         */
        virtual void discardTasks(bool f);
 
        virtual void newRun();
        virtual void endRun();
        virtual void print();

    protected:
        /// pointer to the kernel
        AbsKernel* _kernel;

        /// priority queue, ordered by a TaskModelCmp
        priority_list<TaskModel*, TaskModel::TaskModelCmp> _queue;

        /// map between tasks and models
        std::map<AbsRTTask*, TaskModel*> _tasks;
        
        /// current executing task
        AbsRTTask* _currExe;

        // stores the old task priorities
        std::map<AbsRTTask *, int> oldPriorities;

        /**
           This is the internal version of the addTask, it
           enqueues a model and adds the corresponding task to
           the kernel.
        */
        virtual void enqueueModel(TaskModel* model);

        /** 
         * This function returns a TaskModel from a task. It is
         * used mainly inside this class, but it can also be
         * used by some resource manager. */
        TaskModel* find(AbsRTTask* task) const;
    
        /// @todo change it into ResManager
        friend class PIRManager;
    };    
} // namespace RTSim 

#endif
