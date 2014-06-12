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
#include <timer.hpp>
#include <simul.hpp>

namespace RTSim {

    using namespace std;
    using namespace MetaSim;

    Timer::Timer(const std::string &n, int p) : 
	Entity(n), _triggerEvt(this, &Timer::onTrigger, p) 
    {
    }

    void Timer::newRun() 
    {
	onTrigger(NULL);
    }

    void Timer::endRun() {
	_triggerEvt.drop();
    }

    void Timer::onTrigger(MetaSim::Event *) {
	DBGENTER(_TIMER_DBG_LEV);

	_triggerEvt.drop();
	lastTrigger = SIMUL.getTime();
	action();

	DBGPRINT("Timer fired at "<<lastTrigger);

	reArm();
    }

    PeriodicTimer::PeriodicTimer(Tick p, const std::string &n, int prio) 
	: Timer(n, prio), _period(p) 
    {
    }

    void PeriodicTimer::reArm() {
	Tick t = SIMUL.getTime();
	_triggerEvt.post(t+_period);
	DBGENTER(_TIMER_DBG_LEV);
	DBGPRINT_2("Timer rearmed at ", (t+_period));
    

    }
    void PeriodicTimer::action() {};

}
