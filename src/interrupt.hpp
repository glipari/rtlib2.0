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
 * $Id: interrupt.hpp,v 1.5 2005/10/05 16:46:22 cesare Exp $
 *
 * $Log: interrupt.hpp,v $
 * Revision 1.5  2005/10/05 16:46:22  cesare
 * Added DLL support.
 *
 * Revision 1.4  2005/03/31 15:27:27  cesare
 * Corrected some documentations. Added XML docs.
 *
 * Revision 1.3  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#ifndef __INTERRUPT_HPP__
#define __INTERRUPT_HPP__

#include <vector>

#include <entity.hpp>
#include <gevent.hpp>
#include <randomvar.hpp>

namespace RTSim {

  using namespace std;
  using namespace MetaSim;

  class Task;

  /**
     This class models an interrupt. It can be periodic/sporadic (the
     default), or bursty. The corresponding action is to activate a
     number of tasks.

     @author Giuseppe Lipari 
   */
  class Interrupt : public Entity {
  protected:
    /// the tasks
    vector<Task *> tasks;

    /// interarrival time between bursts
    RandomVar *int_time;

    /// bursty period (minimum interval btw consecutive interrupts)
    int bp;
  
    /// burst lenght (number of consecutive interrupts in a burst).
    RandomVar *burst_lenght;

    /// Counts the number of activations in the burst
    int count;

    /// max number of activations in this round 
    int max_act;

  public:

    /// Trigger Event
    GEvent<Interrupt> triggerEvt;

    /**
       Constructor
       @param iat interarrival time btw bursts
       @param burstperiod minimum interval btw consecutive interrupts
       @param burstlenght number of consecutive instances in a burst
       @param name the name of the interrupt
     */
    Interrupt(RandomVar *iat, 
	      int burstperiod = 1, 
	      RandomVar *burstlenght = NULL, const char *name="");

    ~Interrupt();

    /// add a task to the interrupt activation list
    void addTask(Task *t);

    /**
       Called by the trigger event. Activates the tasks and posts the
       trigger event again.
     */
    void onTrigger(Event *);

    void newRun();
    void endRun();
  };

} // namespace RTSim

#endif
