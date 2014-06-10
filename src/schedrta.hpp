#ifndef __SCHEDRTA_H__
#define __SCHEDRTA_H__

#include <vector>

#include <supervisor.hpp>
#include <server.hpp>

namespace RTSim {
    using namespace MetaSim;

    /** Naive implementation of the RTA test */
    class SchedRTA : public Entity, public Supervisor {
    public:
        struct ServerInfo {
            Server *p_server;
            Tick Q, P, R;
            ServerInfo(Server *s) : p_server(s), Q(s->getBudget()), P(s->getPeriod()), R(0) { }
        };

      /* Added by ROd to modify the budget like in schedpoints*/

        typedef std::vector<Tick> row_t;

        typedef std::vector<double> u_row_t;

      /******************************************************/

    protected:
        std::vector<ServerInfo> servers;

        /*Added by Rod to modify the budget like in schedpoints */

        // these are the original periods of each task/server
        row_t period;
        
        // these are the actual WCET/budgets of each task/server
        row_t wcet;
        
        u_row_t U;

      /**********************************************************/

        // not implemented
        
        SchedRTA(const SchedRTA&);


    public:

        SchedRTA(const string &name);
        ~SchedRTA();

        double sensitivity(int s);

        /**
           This function requests a change (positive or negative) to
           the budget of the server. The function is usually called
           from a feedback module. 

           @param delta_budget increment (or decrement) in the budget

           @return the effective increment (or decrement) in the
           budget.
        */
        Tick changeBudget(Server *s, Tick delta_budget);

        /**
           Adds a new server to the SchedRTA algorithm. The servers
           have to be added in decreasing priority order.

           @param s reference to the server

        */
        void addServer(Server *s);

      /*This function is called to update the vector utilization**/
      void updateU(int task,Tick req);

        void newRun();
        void endRun();

        void updateResponseTimes();
        Tick computeResponseTime(int i) const;
        Tick searchBudget(int i, Tick b1, Tick b2);
        Tick searchBudget(int i);
        bool tryBudget(int i, Tick b);
    };
}

#endif
