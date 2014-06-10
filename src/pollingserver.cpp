#include "pollingserver.hpp"

#include <cassert>

namespace RTSim {

    using namespace MetaSim;

    PollingServer::PollingServer(Tick q, Tick p, const std::string &name,
                                 const std::string &s) :
        Server(name, s),
        Q(q),
        P(p),
        cap(0),
        last_time(0),
        recharging_time(0)
    {
        DBGENTER(_SERVER_DBG_LEV);
        dline = p;
        DBGPRINT(s);
        
    }

    void PollingServer::newRun()
    {
        cap = 0;
        last_time = 0;
        recharging_time = 0;
        status = IDLE;
    }


    void PollingServer::endRun()
    {
    }

    void PollingServer::idle_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        assert (status == IDLE);

        status = READY;
        cap = Q;
        recharging_time = SIMUL.getTime() + P;
        _rechargingEvt.post(recharging_time);

        DBGPRINT_2("Recharging at time ", recharging_time);

        _dlineMissEvt.setPriority(Event::_DEFAULT_PRIORITY + 2);
        _dlineMissEvt.post(recharging_time);
        
        
    }

    void PollingServer::releasing_ready()
    {
        assert(false); // should not be here!
    }

    void PollingServer::ready_executing()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = EXECUTING;

        last_time = SIMUL.getTime();
        _bandExEvt.post(last_time + cap);

        
    }

    void PollingServer::executing_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = READY;
        
        cap -= SIMUL.getTime() - last_time;
        _bandExEvt.drop();

        
    }

    void PollingServer::executing_releasing()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = RECHARGING;  // go immediately to recharging, instead
        cap = 0;
        _bandExEvt.drop();

        
    }

    void PollingServer::releasing_idle()
    {
        assert(false); // should not be here!
    }

    void PollingServer::executing_recharging()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = RECHARGING;
        cap = 0;
        _bandExEvt.drop();

        
    }

    void PollingServer::recharging_ready()
    {
        DBGENTER(_SERVER_DBG_LEV);

        status = READY;
        cap = Q;
        recharging_time = SIMUL.getTime() + P;
        _rechargingEvt.post(recharging_time);               
        _dlineMissEvt.drop();
//         _dlineMissEvt.setPriority(Event::_DEFAULT_PRIORITY + 2);
        _dlineMissEvt.post(recharging_time);

        
    }

    void PollingServer::recharging_idle()
    {
        status = IDLE;
        _dlineMissEvt.drop();
    }

    Tick PollingServer::changeBudget(const Tick &n)
    {
        Tick ret;
        DBGENTER(_SERVER_DBG_LEV);
        
        if (n >= Q) {
            Q = n;
            ret = SIMUL.getTime();
        }
        else if (n > 0) {
            Q = n;
            ret = recharging_time;
        }
        
        return ret;
    }

}
