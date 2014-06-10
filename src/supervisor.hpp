#ifndef __SUPERVISOR_HPP__
#define __SUPERVISOR_HPP__

#include <server.hpp>

namespace RTSim {
    using namespace MetaSim;
 
    /**
       This abstract class is an interface for a supervisor.  A
       supervisor is in charge of performing acceptance test for
       servers. It currently exports 2 functions: changeBudget() which
       verifies if the budget of a certain server can be increased (or
       decreased) by delta; and addServer() which is called at
       initialization time and register the server within the
       supervisor.
     */
    class Supervisor {
    public:
        virtual Tick changeBudget(Server *s, Tick delta) = 0;
        virtual void addServer(Server *s) = 0;
        virtual ~Supervisor() {}
    };
}

#endif
