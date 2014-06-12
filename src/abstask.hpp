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
#ifndef __ABSTASK_HPP__
#define __ABSTASK_HPP__

#include <basetype.hpp>

namespace RTSim {

  using namespace std;
  using namespace MetaSim;

  class AbsKernel;

  /**
     \ingroup task
 
      Abstract Task interface. This interface is common to all objects
      that will implement something similar to a task, i.e. anything
      that can be scheduled by a kernel. For example, both Task and
      Server implement this interface.

      Therefore, this interface defines all methods that are essential
      for a kernel to schedule a task.
  */
    class AbsTask {
    public:
	/**
	   Virtual destructor. It avoids a warning for the presence of
	   virtual functions.
	*/
	virtual ~AbsTask() {}

	/**
	   Called when the task is scheduled to execute.
	*/
	virtual void schedule()   = 0;

	/**
	 * Called when the task is preempted.
	 */
	virtual void deschedule() = 0;

	/**
	   Activates the task. Different from calling onArrival, because
	   this should post a event, rather than directly calling the
	   function. */
	virtual void activate() = 0; 

	/// returns true if the task is active
	virtual bool isActive(void) const = 0;

	/// returns true if the task is executing
	virtual bool isExecuting(void) const = 0;

	/// get current arrival time of the job
	virtual Tick getArrival(void) const = 0;
    
	/// get arrival time of the last executed job
	virtual Tick getLastArrival(void) const = 0;

	/// set the kernel for this task
	virtual void setKernel(AbsKernel*) = 0;

	/// get the kernel for this task
	virtual AbsKernel * getKernel() = 0;

	/** It refreshes the state of the executing task 
	 *  when a change of the CPU speed occurs. 
	 */ 
	virtual void refreshExec(double oldSpeed, double newSpeed) = 0;

	/// returns the wcet for this task, if available! otherwise returns 0.
	virtual Tick getMaxExecutionTime() const { return 0; }

    };

  /**
     \ingroup task

     Interface for a real-time task. In addition to the AbsTask
     interface, here we have also the deadline.
   */
  class AbsRTTask : public virtual AbsTask {
  public:
    /**
      returns the task's deadline
     */
    virtual Tick getDeadline() const = 0;

    /**
      returns the task's relative deadline
     */
    virtual Tick getRelDline() const = 0;

    /**  
      returns the Task ID.
     */ 
    virtual int getTaskNumber() const = 0;

  };

} // namespace RTSim

#endif
