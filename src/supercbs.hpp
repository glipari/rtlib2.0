#ifndef __SUPERCBS_H__
#define __SUPERCBS_H__

#include <vector>
#include <map>

#include <supervisor.hpp>
#include <sporadicserver.hpp>
#include <cbserver.hpp>
namespace RTSim {
    using namespace MetaSim;

    /**
       This class implements the schedpoints algorithm. See paper:

       <i>by Enrico Bini</i>
    */
    class SuperCBS : public Entity, public Supervisor {
    public:
        //this stands for 1d array or vector
        typedef std::vector<Tick> row_t;

        typedef std::vector<double> u_row_t;

        //this stands for a 2d array
        
    protected:
        int counter;
        Tick last_change_time;
        std::map<Server *, int> servers;
           
        // not implemented
        SuperCBS(const SuperCBS&);
        SuperCBS& operator=(const SuperCBS&);
            
        int nTask;
        
        // these are the original periods of each task/server
        row_t period;

        // these are the actual WCET/budgets of each task/server
        row_t wcet;

        // these are the lambdas of each task/server
        //std::vector<double> lambdas;
         u_row_t U;

        int task;
    
        class ChangeBudgetEvt;
        friend class ChangeBudgetEvt;
        void onChangeBudget(ChangeBudgetEvt *e);

        class ChangeBudgetEvt : public Event {
            SuperCBS *sp;
            Server *ss;
            Tick budget;
        public:
            ChangeBudgetEvt(SuperCBS *s1, Server *s2, double b) :
                Event(), sp(s1), ss(s2), budget(b) {}
            virtual void doit() { sp->onChangeBudget(this); }
            Server *getServer() { return ss; }
            Tick getBudget() { return budget; }
        };

    public:
 
        class SuperCBSExc : public BaseExc {
        public:
            SuperCBSExc(const string& m) : 
                BaseExc(m,"SchedPoint","SchedPoint") {};
        };


        SuperCBS(const string &name);
        ~SuperCBS();
        
     //  Tick  sensitivity(const constraints &exactConstraints, const row_t &U, int task);

        double sensitivity(int task);
      /*This function is called to update the vector utilization**/
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

    // inline bool operator<(const SuperCBS::points &a, const SuperCBS::points &b)
    // {
    //     return a.puntos < b.puntos;
    // }
}

#endif
