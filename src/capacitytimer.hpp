#ifndef __CAPACITY_TIMER_HPP__
#define __CAPACITY_TIMER_HPP__

#include <simul.hpp>
#include <entity.hpp>

namespace RTSim {
    using namespace MetaSim;

    class CapacityTimer : public Entity {
    public:
        typedef enum {RUNNING, STOPPED} status_t;

        CapacityTimer();
        ~CapacityTimer();

        void start(double speed=1.0);
        Tick stop();
        status_t get_status() { return status; }
        Tick get_value() const;
        void set_value(const Tick &v);

	/** 
	    Returns how much time from now it will take for the this
	    times to reach value v.
	 */
        Tick get_intercept(const Tick &v) const;

        void newRun();
        void endRun();
    private:
        Tick last_time;
        Tick value;
        status_t status;
        double der;
    };
}

#endif
