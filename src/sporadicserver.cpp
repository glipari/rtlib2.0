#include "sporadicserver.hpp"

#include <cassert>

namespace RTSim {

    using namespace MetaSim;

    SporadicServer::SporadicServer(Tick q, Tick p, const std::string &name,
                                   const std::string &s) :
        Server(name, s),
        Q(q),
        P(p),
        cap(0),
        last_time(0),
        recharging_time(0),
        repl_queue(),
        capacity_queue(),
        _replEvt(this, &SporadicServer::onReplenishment, 
		 Event::_DEFAULT_PRIORITY - 1),
	_idleEvt(this, &SporadicServer::onIdle),
        vtime()
    {
        DBGENTER(_SERVER_DBG_LEV);
        DBGPRINT(s);
        dline = p;

        // register_handler(_replEvt, this, &SporadicServer::onReplenishment); 
        // register_handler(_idleEvt, this, &SporadicServer::onIdle); 
    }

    void SporadicServer::newRun()
    {
        cap = Q;
        last_time = 0;
        recharging_time = 0;
        status = IDLE;
        repl_queue.clear();
        capacity_queue.clear();
        if (vtime.get_status() == CapacityTimer::RUNNING) vtime.stop();
        vtime.set_value(0);
    }

    double SporadicServer::getVirtualTime()
    {
        DBGENTER(_SERVER_DBG_LEV);
        DBGPRINT("Status = " << status_string[status]);
        if (status == IDLE) return double(SIMUL.getTime());
        else return vtime.get_value();
    }

    void SporadicServer::endRun()
    {
    }

    void SporadicServer::idle_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        assert (status == IDLE);

        status = READY;
        cap = Q;
        vtime.set_value(SIMUL.getTime());
        // prepare a replenishment (partial)
        prepare_replenishment(SIMUL.getTime());
        DBGPRINT_2("Inserting replenishment at", repl_queue.back().first);
    }

    void SporadicServer::releasing_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);
        status = READY;
        // prepare a replenishment (partial)
        prepare_replenishment(SIMUL.getTime());
        _idleEvt.drop();
        DBGPRINT_2("Inserting replenishment at", repl_queue.back().first);
    }

    void SporadicServer::ready_executing()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = EXECUTING;

        last_time = SIMUL.getTime();

        vtime.start((double)P/double(Q));

        DBGPRINT_2("Last time is: ", last_time);

        _bandExEvt.post(last_time + cap);
    }

    void SporadicServer::executing_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = READY;
        
        cap = cap - (SIMUL.getTime() - last_time);
        _bandExEvt.drop();
        vtime.stop();
        repl_queue.back().second += SIMUL.getTime() - last_time;
    }

    void SporadicServer::executing_releasing()
    {
        DBGENTER(_SERVER_DBG_LEV);

        DBGPRINT("Status: " << status_string[status]);

        if (status == EXECUTING) {
            cap = cap - (SIMUL.getTime() - last_time);
            _bandExEvt.drop();
            repl_queue.back().second += SIMUL.getTime() - last_time;
            check_repl();

            vtime.stop();
        }

        if (vtime.get_value() <= double(SIMUL.getTime())) 
            status = IDLE;
        else {
            _idleEvt.post(Tick::ceil(vtime.get_value()));
            status = RELEASING;
        }        
        DBGPRINT("Status is now " << status_string[status]);
    }

    void SporadicServer::releasing_idle()
    {
        DBGENTER(_SERVER_DBG_LEV);
        status = IDLE;
    }

    void SporadicServer::executing_recharging()
    {
        DBGENTER(_SERVER_DBG_LEV);

        _bandExEvt.drop();

        DBGPRINT_2("Capacity before: ", cap);
        DBGPRINT_2("Time is: ", SIMUL.getTime());
        DBGPRINT_2("Last time is: ", last_time);

        cap = cap - (SIMUL.getTime() - last_time);

        DBGPRINT_2("Capacity is now: ", cap);

        repl_queue.back().second += SIMUL.getTime() - last_time;

        check_repl();
	
	//inserted by rodrigo seems we do not stop the capacity_timer
	vtime.stop();

        if (capacity_queue.size() > 0) {
            repl_t r = capacity_queue.front();
            capacity_queue.pop_front();
            cap = cap + r.second;
            prepare_replenishment(r.first);
            last_time = SIMUL.getTime();
            status = READY;
        }
        else {
            status = RECHARGING;
        }
        DBGPRINT("The status is now " << status_string[status]);
    }

    void SporadicServer::recharging_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = READY;
        prepare_replenishment(SIMUL.getTime());
    }

    void SporadicServer::recharging_idle()
    {
        assert(false);
    }

    void SporadicServer::onIdle(Event *e)
    {
        DBGENTER(_SERVER_DBG_LEV);
        releasing_idle();
    }

    void SporadicServer::onReplenishment(Event *e)
    {
        DBGENTER(_SERVER_DBG_LEV);
        
        _replEvt.drop();

        DBGPRINT_2("Status before: ", status);
        DBGPRINT_2("Capacity before: ", cap);

        if (status == RECHARGING || status == RELEASING || status == IDLE) {
            cap = cap + repl_queue.front().second;
            repl_queue.pop_front();
            DBGPRINT_3("There are ", repl_queue.size(), 
                       " elements in the repl_queue");
            check_repl();

            if (sched_->getFirst() != NULL) {
                recharging_ready();
                kernel->onArrival(this);
            }
            else if (status != IDLE) {
                status = RELEASING;
                currExe_ = NULL;
                sched_->notify(NULL);
            }
        }
        else if (status == READY || status == EXECUTING) {
            repl_t r = repl_queue.front();
            repl_queue.pop_front();
            capacity_queue.push_back(r);
            if (repl_queue.size() > 1) check_repl();
        }

        DBGPRINT_2("Status is now: ", status_string[status]);
        DBGPRINT_2("Capacity is now: ", cap);
    }

    void SporadicServer::prepare_replenishment(const Tick &t)
    {
        repl_t r;
        r.first = t + P;
        r.second = 0; // still don't know...
        repl_queue.push_back(r);
    }

    void SporadicServer::check_repl()
    {
        DBGENTER(_SERVER_DBG_LEV);

        if (_replEvt.isInQueue()) {
            DBGPRINT("replEvt already in queue, returning");
        }
        else if (repl_queue.size() > 0) {
            DBGPRINT_4("Posting replenishment of ", 
                       repl_queue.front().second, " at ", 
                       repl_queue.front().first);
            _replEvt.post(repl_queue.front().first);
        }
    }

    Tick SporadicServer::changeBudget(const Tick &n)
    {
        Tick ret = 0;
        DBGENTER(_SERVER_DBG_LEV);

        if (n > Q) {
            DBGPRINT_4("Capacity Increment: n=", n, " Q=", Q);
            cap += n - Q;
            if (status == EXECUTING) {
                DBGPRINT_3("Server ", getName(), " is executing");
                cap = cap - (SIMUL.getTime() - last_time);
                repl_queue.back().second += SIMUL.getTime() - last_time;
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
                repl_queue.back().second += SIMUL.getTime() - last_time;
                last_time = SIMUL.getTime();
                DBGVAR(cap);
                _bandExEvt.drop();
                vtime.stop();
            }

            // First, reduce the current capacity
            Tick delta = Q-n;
            Tick x = min(delta, cap);
            DBGVAR(delta);
            DBGVAR(x);

            cap -= x;
            delta -= x;

            DBGVAR(cap);
            DBGVAR(delta);

            // if there is some delta left, reduce the capacity queue
            list<repl_t>::iterator i= capacity_queue.begin();
            while (delta > 0 && i!=capacity_queue.end()) {
                Tick x = min(delta, i->second);
                i->second -= x;
                delta -= x;
                DBGPRINT_2("Capacity queue dec, now delta is: ", delta);
                i++;
            }
            
            // if there is some delta left, reduce the replenishment queue
            i = repl_queue.begin();
            Tick last_repl_time = repl_queue.begin()->first;
            while (delta>0 && i!=repl_queue.end()) {
                Tick x = min(delta, i->second);
                i->second -= x;
                delta -= x;
                DBGPRINT_2("Repl queue dec, now delta is: ", delta);
                DBGVAR(i->second);
                last_repl_time = i->first;
                i++;
            }
            
            // cannot be true, or delta > Q
            assert((delta == 0) || (i!=repl_queue.end()));

            // the time of the first non-zero replenishment
            ret = last_repl_time;

            Q = n;

            // remove empty elements in the capacity queue and in 
            // the replenishment queue
            while (capacity_queue.size()>0 && 
                   capacity_queue.front().second == 0) 
                capacity_queue.pop_front();

            while (repl_queue.size()>0 && 
                   repl_queue.front().second == 0) {
                repl_queue.pop_front();
                if (_replEvt.isInQueue()) {
                    _replEvt.drop();
                    _replEvt.post(repl_queue.front().first);
                }
            }

            if (status == EXECUTING) {
                vtime.start(double(P)/double(Q));
                DBGPRINT("Server was executing");
                if (cap == 0) {
                    DBGPRINT("capacity is zero, go to recharging");
                    _bandExEvt.drop();
                    _bandExEvt.post(SIMUL.getTime());
                    //executing_recharging();
//                     if (currExe_) {
//                         suspend(currExe_);
//                         dispatch();
//                     }
                }
                else {
                    DBGPRINT_2("Reposting bandExEvt at ", last_time + cap);    
                    _bandExEvt.post(last_time + cap);
                }
            }
        }
        return ret;    
    }
 Tick SporadicServer::changeQ(const Tick &n)
{
  Q=n;
  return 0;
}


}

