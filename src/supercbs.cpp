#include <supercbs.hpp>
#include <cassert>
#include <math.h>
#include <sporadicserver.hpp>
#include <algorithm>

namespace RTSim {

    using namespace std;

    void SuperCBS::addServer(Server *s)
    {
        DBGENTER(_SERVER_DBG_LEV);
        servers[s] = counter;
        counter++;
        Tick cc=s->getBudget();
        Tick pp=s->getPeriod();
        // Tick dd=s->getPeriod();
        DBGVAR(cc); DBGVAR(pp);
        period.push_back(pp);
        wcet.push_back(cc);
        U.push_back(double(cc)/double(pp));
    }

    SuperCBS::SuperCBS(const string &name) : 
        Entity(name),
        counter(0),
        last_change_time(0), 
        servers(), 
        period(),
        wcet(),
        U() 
    {    
    }
    
    SuperCBS::~SuperCBS()
    {
    }
        
    Tick SuperCBS::changeBudget(Server *s, Tick db)
    {
        DBGENTER(_SERVER_DBG_LEV);
        Tick delta_budget = db;
        //SporadicServer *s = (SporadicServer *)ss;
        int index = servers[s];
        double lambda;
        DBGVAR(index);
        DBGVAR(delta_budget);
        DBGVAR(wcet.size());
        DBGVAR(period.size());
        if (delta_budget > 0) {
            DBGPRINT("Positive part");
            lambda = sensitivity(index);  
            DBGVAR(lambda);
            if (double(delta_budget)/double(period[index]) <= lambda){
                DBGPRINT("Less than or equal to lambda");
            }
            else{
                DBGPRINT("Greater than lambda");
                delta_budget = Tick::floor(lambda*double(period[index]));
            }
        }
        else {
            DBGPRINT("Negative part (<=0)");
        }
        wcet[index] += delta_budget;
        if (wcet[index] < 1) wcet[index] = 1;
        DBGVAR(wcet[index]);
        U[index] = (double)wcet[index]/double(period[index]);
        DBGVAR(U[index]);

        DBGVAR(last_change_time);
        
        if (last_change_time <= SIMUL.getTime()) {
            last_change_time = s->changeBudget(delta_budget + s->getBudget());
            DBGPRINT_2("in the past, new change time at ", last_change_time);
        }
        else {
            DBGPRINT("in the future"); 
            if (delta_budget < 0) {
                last_change_time = 
                    max(last_change_time, s->changeBudget(delta_budget + s->getBudget()));
                DBGPRINT_2("delta_budget < 0, new change time at ", last_change_time);
            }
            else if (delta_budget > 0) {
                ChangeBudgetEvt *e = new ChangeBudgetEvt(this, s, delta_budget + s->getBudget());
                e->post(last_change_time, true);
                DBGPRINT_2("delta_budget > 0, new change time at ", last_change_time);
            }
        }

        return delta_budget;
    }

    void SuperCBS::onChangeBudget(ChangeBudgetEvt *e)
    {
        DBGPRINT("SuperCBS::onChangeBudget(ChangeBudgetEvt *e)");
        Server *ss = e->getServer();
        ss->changeBudget(e->getBudget());
    }

    


    // Tick SchedPoint::sensitivity(const constraints
    // &exactConstraints, const row_t &U, int task){

    double SuperCBS::sensitivity(int task){

        //        DBGENTER(_SERVER_DBG_LEV);
        int nTask=U.size();
        double lambda;
        double suma=0;
        DBGPRINT("Sensitivity");
        DBGVAR(nTask);
        DBGVAR(task);
        DBGPRINT("utilization vector");
        DBGVECTOR(U);
        DBGPRINT("wcet");  
        DBGVECTOR(wcet);
        DBGPRINT("period");  
        DBGVECTOR(period);
        for (int i=0;i<nTask;i++)
            suma += U[i];
        lambda=(1-suma);
        return (lambda);
    }
    
    void SuperCBS::newRun()
    {
        last_change_time = 0;      
    }
    
    void SuperCBS::endRun()
    {
    }
    
}
