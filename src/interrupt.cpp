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
 * $Id: interrupt.cpp,v 1.5 2005/04/28 01:34:48 cesare Exp $
 *
 * $Log: interrupt.cpp,v $
 * Revision 1.5  2005/04/28 01:34:48  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.4  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#include <cassert>

#include <randomvar.hpp>
#include <simul.hpp>

#include <interrupt.hpp>
#include <task.hpp>

namespace RTSim {

  using namespace std;
  using namespace MetaSim;

    Interrupt::Interrupt(RandomVar *iat, 
			 int burstperiod, 
			 RandomVar *burstlenght, const char *name) :
	Entity(name),
	int_time(iat),
	bp(burstperiod),
	burst_lenght(burstlenght),
	count(0),
	max_act(0),
	triggerEvt(this, &Interrupt::onTrigger)
    {
	if (burst_lenght == NULL) burst_lenght = new DeltaVar(1);   
	//register_handler(triggerEvt, this, &Interrupt::onTrigger);
    }

  Interrupt::~Interrupt()
  {
    delete int_time;
    delete burst_lenght;
  }

  void Interrupt::addTask(Task *t)
  {
    tasks.push_back(t);
  }

  void Interrupt::newRun()
  {
    assert(int_time != NULL);
    assert(burst_lenght != NULL);

    triggerEvt.post((int)(int_time->get()));
    max_act = static_cast<int>(burst_lenght->get());
    count = 0;
  }

  void Interrupt::endRun() 
  {
  }

  void Interrupt::onTrigger(Event *e)
  {
    for (unsigned int i=0; i<tasks.size(); ++i) tasks[i]->activate();
    
    count ++;
    if (count == max_act) {
      triggerEvt.post(SIMUL.getTime() + (int)(int_time->get()));
      max_act = static_cast<int>(burst_lenght->get());
      count = 0;
    }
    else
      triggerEvt.post(SIMUL.getTime() + bp);
  }

}
