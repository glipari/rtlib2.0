#ifndef __SPAREPOT_H__
#define __SPAREPOT_H__

#include <vector>
#include <map>
#include <sporadicserver.hpp>
#include <supervisor.hpp>

#define _SPARE_POT_DBG_LEV  "SparePot"

namespace RTSim {

    using namespace MetaSim;

    /**
       This class implements the Spare Pot Algorithm. See paper:

       <i>Rodrigo Santos, Giuseppe Lipari, Enrico Bini, "Efficient
       on-line schedulability test for feedback scheduling of soft
       real-time tasks under fixed-priority", IEEE Real-Time and
       Embedded Technology and Applications Symposium, Saint Louis USA
       (2008)</i>
    */
    class SparePot : public Entity, public Supervisor {
    public:
        typedef std::vector<double> row_t;

        struct server_struct {
            SporadicServer *s;
            Tick wcet;
            Tick period;
        };

    protected:
        std::vector<server_struct> server_vector;

        std::map<SporadicServer *, int> servers;
        int counter;
        
        // this is the lend/borrow matrix
        std::vector<row_t> pi;

        // this is the total amount of budget lended/borrowed by each
        // server
        std::vector<double> delta;

        // these are called mij in the paper
        std::vector<row_t> coeff;

        Tick last_change_time;

        Tick spare_budget;

        // not implemented
        SparePot(const SparePot&);
        SparePot& operator=(const SparePot&);
    
    public:
    
        class SparePotExc : public BaseExc {
        public:
            SparePotExc(const string& m) : 
                BaseExc(m,"SparePot","SparePot") {};
        };

        SparePot(const string &name);
        ~SparePot();
        
        void addServer(Server *s);
    
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
           It takes budget and period of the spare pot
         */
        void compute_matrix(Tick b, Tick p);

        void newRun();
        void endRun();
    protected:

        /**
           Adds a new server to the Spare Pot algorithm. The servers
           have to be added in decreasing priority order.

           @param s reference to the server

           @param mij list of coefficients for the server. For each
           server, I have to specify the list of the coefficient of
           this server with all the higher priority ones. So, for the
           i-th server in the matrix, I have to specify i elements
           in the list, where the first element is m_{i,0}, and the
           last is m_{i,i} (always equal to 1).
        */
        void addMyServer(SporadicServer *s, const row_t &m);

        /**
           Specifies the amount of spare handled by this algorithm.
           It corresponds to server 0. 
        */
        void setSpare(const Tick &budget, const Tick &period);


        class ChangeBudgetEvt;
        friend class ChangeBudgetEvt;
        void onChangeBudget(ChangeBudgetEvt *e);

        class ChangeBudgetEvt : public Event {
            SparePot *sp;
            SporadicServer *ss;
            Tick budget;
        public:
            ChangeBudgetEvt(SparePot *s1, SporadicServer *s2, Tick b) :
                Event(EndEvt::_END_EVT_PRIORITY + 4), sp(s1), ss(s2), budget(b) {}
            virtual void doit() { sp->onChangeBudget(this); }
            SporadicServer *getServer() { return ss; }
            Tick getBudget() { return budget; }
        };
    };

    inline bool operator<(const SparePot::server_struct &a, const SparePot::server_struct &b)
    {
        return a.period < b.period;
    }

}

#endif
