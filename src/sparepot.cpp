#include <cassert>
#include <algorithm>
#include <sparepot.hpp> 

namespace RTSim {
    using namespace std;
    SparePot::SparePot(const string &name) : Entity(name),
                                             server_vector(),
                                             servers(),
                                             counter(0),
                                             pi(),
                                             delta(),
                                             coeff(),
                                             last_change_time(0),
                                             spare_budget(0)
    {
    }
    
    SparePot::~SparePot()
    {
    }

    void SparePot::addServer(Server *s)
    {
        // stores the server 
        server_struct server;
        server.s = dynamic_cast<SporadicServer *>(s);
        server.wcet = s->getBudget();
        server.period = s->getPeriod();
        
        server_vector.push_back(server);    
    }
    
    void SparePot::compute_matrix(Tick b, Tick p)
    {
        DBGENTER(_SPARE_POT_DBG_LEV);

        server_struct serv_s;
        serv_s.s = 0;
        serv_s.period = p;
        serv_s.wcet = b;

        server_vector.push_back(serv_s);

        // computes the m_i,j, then calls addServer(s, m)
        // first sort by increasing period (rate monotonic)
        DBGPRINT("Sorting the vector of size " << server_vector.size());

        sort(server_vector.begin(), server_vector.end());

        DBGPRINT("Now the vector is sorted, compute response times");
        // then compute response times
        vector<double> response_time(server_vector.size());
        response_time[0] = server_vector[0].wcet;
        
        DBGVAR(response_time[0]);

        for (int i=1; i<server_vector.size(); i++) {
            DBGVAR(i);
            double rtime = server_vector[i].wcet;
            double old_rtime = 0;
            while (old_rtime != rtime) {
                old_rtime = rtime;
                rtime = server_vector[i].wcet;
                for (int j=0; j<i; j++) 
                    rtime += ceil(old_rtime / (double)server_vector[j].period) * (double)server_vector[j].wcet;

                if (rtime > (double)server_vector[i].period) {
                    cerr << "Response time greater than period" << endl;
                    exit(-1);
                }
                DBGVAR(rtime);
            }
            response_time[i] = rtime;
            DBGVAR(response_time[i]);
        }

        DBGPRINT("Now computing the eta");

        // now compute the eta_i,j (with j < i)
        vector< vector<double> > eta;
        for (int i=0; i<server_vector.size(); i++) {
            vector<double> myeta_i;
            DBGPRINT("For task i= " << i);
            for (int j=0; j<i; j++) {
                double e = ceil(response_time[i]/(double)server_vector[j].period);
                myeta_i.push_back(e);
                DBGVAR(e);
            }
            eta.push_back(myeta_i);
        }

        setSpare(b, p);

        DBGPRINT("Now computing the m_i,i");

        // now compute the m_i,j and add the server to the algorithm
        for (int i=1; i<server_vector.size(); i++) {
            DBGVAR(i);
            row_t m;
            for (int j=0; j<i; j++) {
                DBGPRINT("eta["<< i << "][" << j << "]: " << eta[i][j]); 
                double rate = eta[i][j];
                DBGVAR(rate);
                for (int h=i+1; h<server_vector.size(); h++) { 
                    rate = min(rate, eta[h][j]/eta[h][i]);
                    DBGVAR(eta[h][j]/eta[h][i]);
                    DBGVAR(rate);
                }
                m.push_back(rate);
            }
            DBGVAR(m.size());
            addMyServer(server_vector[i].s, m);
        }        
    }

    
    void SparePot::addMyServer(SporadicServer *s, const row_t &m)
    {
        servers[s] = counter;
        counter ++;
        
        row_t myrow;
        for (int i=0; i<counter; i++) myrow.push_back(m[i]);

        coeff.push_back(myrow);
        delta.push_back(0);
        
        //row_t myrow(counter);
        for(int i=0; i<counter; i++) myrow[i] = 0;

        pi.push_back(myrow);
        
        for (int i=0; i<counter-1; i++) {
            pi[i].push_back(0);
        }
    }

    void SparePot::setSpare(const Tick &budget, const Tick &period)
    {
        if (counter != 0) 
            throw SparePotExc("setSpare() must be called first!");
        
        servers[0] = 0;
        counter++;

        spare_budget = budget;
        
        row_t myrow;
        coeff.push_back(myrow);
        delta.push_back(budget);

        myrow.push_back(-budget);
        pi.push_back(myrow);
    }

    
    Tick SparePot::changeBudget(Server *ss, Tick db)
    {
        DBGENTER(_SPARE_POT_DBG_LEV);

	SporadicServer *s = (SporadicServer *) ss;

        int index = servers[s];
        double ret = 0;
        double delta_budget = double(db);

        DBGVAR(index);
        DBGVAR(delta_budget);

        if (delta_budget > 0) {
            DBGPRINT("Positive part");
            for (int j=index; j>=0 && delta_budget>0; j--) {
                if (delta[j] > 0) {
                    DBGVAR(coeff[index][j]);
                    double x = min(delta_budget, coeff[index][j]*delta[j]);
                    pi[index][j] = pi[index][j] + x;
                    pi[j][index] = pi[j][index] + x/coeff[index][j];

                    delta[index] = delta[index] - x;
                    delta[j] = delta[j] - x/coeff[index][j];

                    delta_budget = delta_budget - x;
                    ret = ret + x;
                    
                    DBGPRINT_4("j=", j, " x = ", x); 
                    DBGPRINT_4("pi[index][j] = ", pi[index][j],
                               " pi[j][index] = ", pi[j][index]);
                    DBGPRINT_4("delta[index] = ", delta[index],
                               " delta[j] = ", delta[j]);
                    DBGPRINT_2("delta_budget = ", delta_budget);

                }
            }
        }
        else if (delta_budget < 0) {
            DBGPRINT("Negative part");
            for (int j=0; j<index && delta_budget < 0; j++) {
                double x = min(pi[index][j], -delta_budget);
                pi[index][j] = pi[index][j] - x;
                pi[j][index] = pi[j][index] - x/coeff[index][j];

                delta[index] = delta[index] + x;
                delta[j] = delta[j] + x/coeff[index][j];

                delta_budget = delta_budget + x;
                ret = ret - x;

                DBGPRINT_4("j=", j, " x = ", x); 
                DBGPRINT_4("pi[index][j] = ", pi[index][j],
                           " pi[j][index] = ", pi[j][index]);
                DBGPRINT_4("delta[index] = ", delta[index],
                           " delta[j] = ", delta[j]);
                DBGPRINT_2("delta_budget = ", delta_budget);

            }
            if (delta_budget < 0) {
                DBGPRINT_2("delta_budget is still ", delta_budget);
                DBGVAR(delta[index]);

                pi[index][index] = pi[index][index] + delta_budget;

                //assert(delta[index] == 0);

                delta[index] = delta[index] - delta_budget;

                DBGPRINT_2("delta[index] = ", delta[index]);
                DBGPRINT_2("pi[index][index] = ", pi[index][index]);

                ret = ret + delta_budget;
            }
        }

        DBGVAR(last_change_time);
        if (last_change_time <= SIMUL.getTime()) {
            last_change_time = s->changeBudget(Tick::ceil(ret)+s->getBudget());
            DBGPRINT_2("in the past, new change time at ", last_change_time);
        }
        else {
            DBGPRINT("in the future"); 
            if (ret < 0) {
                last_change_time = max(last_change_time, 
                                       s->changeBudget(Tick::ceil(ret)+s->getBudget()));
                DBGPRINT_2("ret < 0, new change time at ", last_change_time);
            }
            else if (ret > 0) {
                ChangeBudgetEvt *e = new ChangeBudgetEvt(this, s, Tick::ceil(ret)+s->getBudget());
                e->post(last_change_time, true);
                DBGPRINT_2("ret > 0, new change time at ", last_change_time);
            }
        }
            
        return Tick::floor(ret);
    }

    void SparePot::onChangeBudget(ChangeBudgetEvt *e)
    {
        DBGENTER(_SPARE_POT_DBG_LEV);
        SporadicServer *ss = e->getServer();
        ss->changeBudget(Tick::floor(e->getBudget()));
    }

    void SparePot::newRun()
    {
        DBGENTER(_SPARE_POT_DBG_LEV);
        last_change_time = 0;
        
        DBGVAR(delta.size());
        delta[0] = spare_budget;
        
        DBGVAR(pi.size());
        pi[0][0] = -spare_budget;

        for (int j=1; j<counter; j++) pi[0][j] = 0;

        for (int i=1; i<counter; i++) {
            delta[i] = 0;
            for (int j=0; j<counter; j++) pi[i][j] = 0;
        }
    }

    void SparePot::endRun()
    {
    }
  
}
