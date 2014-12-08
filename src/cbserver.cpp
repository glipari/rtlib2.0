#include "cbserver.hpp"

#include <cassert>

namespace RTSim {

    using namespace MetaSim;

    CBServer::CBServer(Tick q, Tick p,Tick d, bool HR, const std::string &name,
		       const std::string &s) :
        Server(name, s),
        Q(q),
        P(p),
	d(d),
        cap(0),
	last_time(0),
	HR(HR),
        _replEvt(this, &CBServer::onReplenishment, 
		 Event::_DEFAULT_PRIORITY - 1),
	_idleEvt(this, &CBServer::onIdle),
        vtime(),
	idle_policy(ORIGINAL)
    {
        DBGENTER(_SERVER_DBG_LEV);
        DBGPRINT(s);
    }

    void CBServer::newRun()
    {
	DBGENTER(_SERVER_DBG_LEV);
	DBGPRINT_2("HR ", HR);
        cap = Q;
        last_time = 0;
        recharging_time = 0;
        status = IDLE;
        capacity_queue.clear();
        if (vtime.get_status() == CapacityTimer::RUNNING) vtime.stop();
        vtime.set_value(0);
    }

    double CBServer::getVirtualTime()
    {
        DBGENTER(_SERVER_DBG_LEV);
        DBGPRINT("Status = " << status_string[status]);
        if (status == IDLE)
	    return double(SIMUL.getTime());
        else 
	    return vtime.get_value();
    }

    void CBServer::endRun()
    {
    }

    void CBServer::idle_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);
        assert (status == IDLE);
        status = READY;

	cap = 0;

	if (idle_policy == REUSE_DLINE && SIMUL.getTime() < getDeadline()) {
	    double diff = double(getDeadline() - SIMUL.getTime()) * 
		double(Q) / double(P);
	    cap = Tick(std::floor(diff));
	}

	if (cap == 0) {
	    cap = Q;
	    //added relative deadline
	    d = SIMUL.getTime() + P;
	    DBGPRINT_2("new deadline ",d);
	    setAbsDead(d);
	}
	vtime.set_value(SIMUL.getTime());
	DBGPRINT_2("Going to active contending ",SIMUL.getTime());
    }
    
    /*I should compare the actual bandwidth with the assignedserver Q
     *  and postpone deadline and full recharge or just use what is
     *  left*/
    // this should not be necessary. 
    // In fact, the releasing state should be the state in which: 
    // 1) the server is not executing
    // 2) the if (condition) is false.
    // in other words, it should be possible to avoid the if.
    void CBServer::releasing_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);
	status = READY;
        _idleEvt.drop();
        DBGPRINT("FROM NON CONTENDING TO CONTENDING");
    }

    void CBServer::ready_executing()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = EXECUTING;
        last_time = SIMUL.getTime();
        vtime.start((double)P/double(Q));
        DBGPRINT_2("Last time is: ", last_time);
        _bandExEvt.post(last_time + cap);
    }

    /*The server is preempted. */
    void CBServer::executing_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = READY;
        cap = cap - (SIMUL.getTime() - last_time);
        vtime.stop();
        _bandExEvt.drop();
    }

    /*The sporadic task ends execution*/
    void CBServer::executing_releasing()
    {
        DBGENTER(_SERVER_DBG_LEV);
	
        if (status == EXECUTING) {
            cap = cap - (SIMUL.getTime() - last_time);
      	    vtime.stop();
            _bandExEvt.drop();
        }
        if (vtime.get_value() <= double(SIMUL.getTime())) 
            status = IDLE;
        else {
	    _idleEvt.post(Tick::ceil(vtime.get_value()));
            status = RELEASING;
        }        
        DBGPRINT("Status is now XXXYYY " << status_string[status]);
    }

    void CBServer::releasing_idle()
    {
        DBGENTER(_SERVER_DBG_LEV);
        status = IDLE;
    }

    /*The server has no more bandwidth The server queue may be empty or not*/
    void CBServer::executing_recharging()
    {
        DBGENTER(_SERVER_DBG_LEV);

        _bandExEvt.drop();
        vtime.stop();

        DBGPRINT_2("Capacity before: ", cap);
        DBGPRINT_2("Time is: ", SIMUL.getTime());
        DBGPRINT_2("Last time is: ", last_time);
	DBGPRINT_2("HR: ", HR);
	if (!HR) {
	    cap=Q;
	    d=d+P;
	    setAbsDead(d);
	    DBGPRINT_2("Capacity is now: ", cap);
	    DBGPRINT_2("Capacity queue: ", capacity_queue.size());
            DBGPRINT_2("new_deadline: ", d);
	    status=READY;
	    _replEvt.post(SIMUL.getTime());
        }
	else
	  {
              cap=0;
              _replEvt.post(d);
              d=d+P;
              setAbsDead(d);
              status=RECHARGING;
	  }

	//inserted by rodrigo seems we do not stop the capacity_timer
        // moved up
	// vtime.stop();

        DBGPRINT("The status is now " << status_string[status]);
    }

    /*The server has recovered its bandwidth and there is at least one task left in the queue*/
    void CBServer::recharging_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);
        status = READY;
    }

    void CBServer::recharging_idle()
    {
        assert(false);
    }

    void CBServer::onIdle(Event *e)
    {
        DBGENTER(_SERVER_DBG_LEV);
        releasing_idle();
    }

    void CBServer::onReplenishment(Event *e)
    {
        DBGENTER(_SERVER_DBG_LEV);
        
        _replEvt.drop();

        DBGPRINT_2("Status before: ", status);
        DBGPRINT_2("Capacity before: ", cap);

        if (status == RECHARGING || 
            status == RELEASING || 
            status == IDLE) {
	    cap = Q;//repl_queue.front().second;
            if (sched_->getFirst() != NULL) {
                recharging_ready();
                kernel->onArrival(this);
            }
            else if (status != IDLE) {
                if (double(SIMUL.getTime()) < vtime.get_value()) {
                    status = RELEASING;
                    _idleEvt.post(Tick::ceil(vtime.get_value()));
                }
                else status = IDLE;
                
                currExe_ = NULL;
                sched_->notify(NULL);
            }
        }
        else if (status == READY || status == EXECUTING) {
            if (sched_->getFirst() == this) {
            }

            //       repl_queue.pop_front();
            //capacity_queue.push_back(r);
            //if (repl_queue.size() > 1) check_repl();
            //me falta reinsertar el servidor con la prioridad adecuada
	}

        DBGPRINT_2("Status is now: ", status_string[status]);
        DBGPRINT_2("Capacity is now: ", cap);
    }

    Tick CBServer::changeBudget(const Tick &n)
    {
        Tick ret = 0;
        DBGENTER(_SERVER_DBG_LEV);

        if (n > Q) {
            DBGPRINT_4("Capacity Increment: n=", n, " Q=", Q);
            cap += n - Q;
            if (status == EXECUTING) {
                DBGPRINT_3("Server ", getName(), " is executing");
                cap = cap - (SIMUL.getTime() - last_time);
		_bandExEvt.drop();
                vtime.stop();
                last_time = SIMUL.getTime();
                _bandExEvt.post(last_time + cap);
                vtime.start((double)P/double(n));
                DBGPRINT_2("Reposting bandExEvt at ", last_time + cap);
            }
            Q = n;
            ret = SIMUL.getTime();
        }
        else if (n == Q) {
            DBGPRINT_2("No Capacity change: n=Q=", n);
            ret = SIMUL.getTime();
        }
        else if (n > 0) {
            DBGPRINT_4("Capacity Decrement: n=", n, " Q=", Q);
            if (status == EXECUTING) {
                DBGPRINT_3("Server ", getName(), " is executing");
                cap = cap - (SIMUL.getTime() - last_time);
		last_time = SIMUL.getTime();
                DBGVAR(cap);
                _bandExEvt.drop();
                vtime.stop();
            }
            
            Q = n;

            if (status == EXECUTING) {
                vtime.start(double(P)/double(Q));
                DBGPRINT("Server was executing");
                if (cap == 0) {
                    DBGPRINT("capacity is zero, go to recharging");
                    _bandExEvt.drop();
                    _bandExEvt.post(SIMUL.getTime());
                }
                else {
                    DBGPRINT_2("Reposting bandExEvt at ", last_time + cap);    
                    _bandExEvt.post(last_time + cap);
                }
            }
        }
        return ret;    
    }

    Tick CBServer::changeQ(const Tick &n)
    {
	Q=n; 
	return 0;
    }

    Tick CBServer::get_remaining_budget()
    {
	double dist = (double(getDeadline()) - vtime.get_value()) * 
	    double(Q) / double(P) + 0.00000000001;
	
	return Tick::floor(dist);
    }
}

