#include <capacitytimer.hpp>
#include <cassert>
#include <cmath>

#include <iostream>
#include <sstream>

namespace RTSim {

    using namespace std;

    CapacityTimer::CapacityTimer() : Entity(""), 
                                     last_time(0), 
                                     value(0), 
                                     status(STOPPED), der(1) {}

    CapacityTimer::~CapacityTimer() {}

    void CapacityTimer::start(double speed) 
    {
        assert(status == STOPPED);
        der = speed;
        last_time = SIMUL.getTime();
        status = RUNNING;
    }

    double CapacityTimer::stop() 
    {
        assert(status == RUNNING);
        //value += Tick::ceil(double(SIMUL.getTime() - last_time) * der);
	value += double(SIMUL.getTime() - last_time) * der;
        status = STOPPED;
        return value;
    }

    Tick CapacityTimer::get_intercept(const Tick &v) const 
    {
	assert(status == RUNNING && der != 0);
	return Tick::floor((double(v) - get_value()) / der);
    }

    double CapacityTimer::get_value() const
    {
        if (status == RUNNING) 
            //return value + Tick::ceil(double(SIMUL.getTime() - last_time) * der);
	    return value + double(SIMUL.getTime() - last_time) * der;
        else return value;
    }

    void CapacityTimer::set_value(const double &v)
    {
        assert(status == STOPPED);
        value = v;
    }

    void CapacityTimer::newRun()
    {
        status = STOPPED;
        value = 0;
        der = 1;
    }

    void CapacityTimer::endRun()
    {
    }
}
