#ifndef __SCHEDPOINTS_H__
#define __SCHEDPOINTS_H__

#include <vector>
#include <map>

#include <supervisor.hpp>
#include <sporadicserver.hpp>

namespace RTSim {
    using namespace MetaSim;

    /**
       This class implements the schedpoints algorithm. See paper:

       <i>by Enrico Bini</i>
    */
    class SchedPoint : public Entity, public Supervisor {
    public:
        //this stands for 1d array or vector
        typedef std::vector<Tick> row_t;

        typedef std::vector<double> u_row_t;

        //this stands for a 2d array
        typedef std::vector<u_row_t> matriz;
     
        //this stands for a 3d array 
        typedef std::vector<matriz> constraints;    
        struct points {
            Tick puntos;
        };
    protected:
        int counter;
        Tick last_change_time;
        std::map<Server *, int> servers;
           
        // not implemented
        SchedPoint(const SchedPoint&);
        SchedPoint& operator=(const SchedPoint&);
            
        int nTask;
        
        // these are the original periods of each task/server
        row_t period;

        // these are the actual WCET/budgets of each task/server
        row_t wcet;

        // these are the lambdas of each task/server
        //std::vector<double> lambdas;
        u_row_t lambdas;
      
        matriz OneTaskConstraints;
      
        constraints exactConstraints;

        //These are the matrix for the exact constraints
        //std::vector<constraints> exactConstraints;
 
        //these are the SchedPoints
        row_t schedpoints;

        u_row_t U;

        int task;
    
        class ChangeBudgetEvt;
        friend class ChangeBudgetEvt;
        void onChangeBudget(ChangeBudgetEvt *e);

        class ChangeBudgetEvt : public Event {
            SchedPoint *sp;
            Server *ss;
            Tick budget;
        public:
            ChangeBudgetEvt(SchedPoint *s1, Server *s2, double b) :
                Event(), sp(s1), ss(s2), budget(b) {}
            virtual void doit() { sp->onChangeBudget(this); }
            Server *getServer() { return ss; }
            Tick getBudget() { return budget; }
        };

    public:
 
        class SchedPointExc : public BaseExc {
        public:
            SchedPointExc(const string& m) : 
                BaseExc(m,"SchedPoint","SchedPoint") {};
        };


        SchedPoint(const string &name);
        ~SchedPoint();
        
        row_t SetP(int D, const row_t &schedpoints, int task);

        constraints buildconstraints();

        //  Tick  sensitivity(const constraints &exactConstraints, const row_t &U, int task);

        double sensitivity(int task);
      /*This function is called to update the vector utilization**/
      void updateU(int task,Tick req);
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
           Adds a new server to the SchedPoint algorithm. The servers
           have to be added in decreasing priority order.

           @param s reference to the server

        */
        void addServer(Server *s);

        void newRun();
        void endRun();

    };

    inline bool operator<(const SchedPoint::points &a, const SchedPoint::points &b)
    {
        return a.puntos < b.puntos;
    }
}

#endif
