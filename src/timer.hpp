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
#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <simul.hpp>
#include <event.hpp>

#include <abstask.hpp>

#define _TIMER_DBG_LEV "Timer"

namespace RTSim {

  using namespace MetaSim;

  class Timer : public Entity {
  public:
    class TimerEvt : public MetaSim::Event {
      Timer * _timer;
    public:
      TimerEvt(Timer * t, int p) : MetaSim::Event(p), _timer(t) {};
      virtual void doit() {
	_timer->onTrigger();
      };
    } _triggerEvt;
    Tick lastTrigger; //Instant of the last trigger event 
  public:
    Timer(char *n = "", int p = 16) : Entity(n), _triggerEvt(this,p) {};
    virtual void reArm()=0;
    virtual void action() = 0;
    virtual void onTrigger();
    virtual void newRun();
    virtual void endRun();
  };

  class PeriodicTimer : public Timer {
    Tick _period;
  public:
    PeriodicTimer(Tick p, char * n="",int prio = 16) 
      :Timer(n,prio), _period(p) {}
    virtual void reArm();
    virtual void action();
  };

} // RTSim

#endif
