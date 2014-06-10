#include <schedpoints.hpp>
#include <cassert>
#include <math.h>
#include <sporadicserver.hpp>
#include <algorithm>

namespace RTSim {

    using namespace std;

    void SchedPoint::addServer(Server *s)
    {
        DBGENTER(_SERVER_DBG_LEV);
        servers[s] = counter;
        counter++;
	Tick cc=s->getBudget();
	Tick pp=s->getPeriod();
        DBGVAR(cc); DBGVAR(pp);
	period.push_back(pp);
        wcet.push_back(cc);
        U.push_back(double(cc)/double(pp));
    }

    SchedPoint::SchedPoint(const string &name) : 
        Entity(name), counter(0), 
        last_change_time(0), servers(), period(), wcet(), lambdas(),
        OneTaskConstraints(), exactConstraints(), schedpoints(), U(), task(0) 
    {    
    }
    
    SchedPoint::~SchedPoint()
    {
    }
        
    Tick SchedPoint::changeBudget(Server *s, Tick db)
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

    void SchedPoint::onChangeBudget(ChangeBudgetEvt *e)
    {
        DBGPRINT("SchedPoint::onChangeBudget(ChangeBudgetEvt *e)");
        Server *ss = e->getServer();
        ss->changeBudget(e->getBudget());
    }

    
    SchedPoint::row_t SchedPoint::SetP(int D, const row_t &period, int task)
    {

        //task is the task being analysed
        int n = task;
        row_t schp, schP;
        row_t  aux;
	schp.clear();
        schp.push_back(D);

	DBGPRINT("Computing scheduling points" << n);
        
	for (int i=n-1;i>-1;i--) {
          // for (int i=0;i<n;i++){  
            DBGPRINT("Computing aux for i=" << i); 
            for (int j=0;j<(int)schp.size();j++) {
                Tick p = Tick::floor(floor(double(schp[j])/double(period[i]))*
                                     double(period[i]));
                if (p>0)
                    aux.push_back(p);
            }

            DBGVECTOR(aux);

            for (int j=0;j<(int)aux.size();j++) 
                schp.push_back(aux[j]);

            aux.clear();

        } 
	  DBGPRINT("Schedp son");   
	 DBGVECTOR(schp);

	sort(schp.begin(), schp.end());
	schP.push_back(schp[0]);

	for (int h=1;h<int(schp.size());h++)
            if (schp[h]!=schp[h-1])
	    	schP.push_back(schp[h]);
		DBGPRINT("Schedp ordenados y sin repeticion son");  
	 DBGVECTOR(schP);
        return schP;
    }

    SchedPoint::constraints SchedPoint::buildconstraints()
    {    
      //DBGENTER(_SERVER_DBG_LEV);
	DBGPRINT("Buildconstraints");
        int  ntasks=period.size();
	u_row_t curCoef;

        DBGVAR(ntasks);

        DBGVECTOR(period);

        exactConstraints.clear();
        for (int curTask=0;curTask<ntasks;curTask++){
            row_t schedP;
            // compute the scheduling points
            schedP=SetP(period[curTask], period, curTask);
 
            // initialize the empty matrix
            OneTaskConstraints.clear();
             DBGVAR(curTask);
             DBGVAR(OneTaskConstraints.size());
             DBGVAR(schedP.size());

            for (int curPoint=0; curPoint<(int)schedP.size(); curPoint++){
                curCoef.clear();
                // the current scheduling point
                double t = schedP[curPoint];
                DBGPRINT("Current scheduling point: " << t);
                DBGPRINT("Now computing the coefficient");
                //compute the coefficients
                for (int j=0;j <= curTask-1;j++) { 
                    DBGPRINT("PASA: j=" << j);
                    curCoef.push_back(ceil((double(t)/double(period[j]))));
		    DBGVAR(ceil((double(t)/double(period[j]))));
                }
                DBGPRINT("Now filling the rest of the vector");
		curCoef.push_back(1);
                for (int j=0;j<ntasks-curTask-1;j++) curCoef.push_back(0);
	        
                DBGPRINT("Before normalization: ");
                DBGVECTOR(curCoef);
                DBGPRINT("Now normalizing");
                for (int i=0;i<(int)curCoef.size();i++) {
                    curCoef[i]=double(curCoef[i])*double(period[i])/double(t);
                }
                DBGVECTOR(curCoef);

                // store the normalized coefficients
                OneTaskConstraints.push_back(curCoef);
            }  
            DBGPRINT("Finished computing the OneTaskConstraints");
            DBGVAR(OneTaskConstraints.size());
            //store all the coefficients for curTask schedulability
            exactConstraints.push_back(OneTaskConstraints);
        }
        return exactConstraints;
    }

    // Tick SchedPoint::sensitivity(const constraints
    // &exactConstraints, const row_t &U, int task){

    double SchedPoint::sensitivity(int task){

      //        DBGENTER(_SERVER_DBG_LEV);
        int nTask=U.size();
        u_row_t lambdaVec;
        matriz curConstraint;
        u_row_t product;
        double suma;
	DBGPRINT("Sensitivity");
        DBGVAR(nTask);
        DBGVAR(exactConstraints.size());
        DBGVAR(task);
	DBGPRINT("utilization vector");
	DBGVECTOR(U);
        DBGPRINT("wcet");  
	DBGVECTOR(wcet);
        DBGPRINT("period");  
	DBGVECTOR(period);
        for (int i=0; i<nTask; i++) {
            curConstraint = exactConstraints[i];
            DBGPRINT("curConstraints");
            for (unsigned row=0;row<curConstraint.size();row++){
                DBGVAR(row);
                DBGVECTOR(curConstraint[row]);
            }
            for (unsigned row=0;row<curConstraint.size();row++) {
                suma=0;
                for (int col=0;col<nTask;col++) {  
                    suma += curConstraint[row][col]*U[col];
                }
                product.push_back(suma);
            }
            for (unsigned col=0;col<product.size();col++){ 
                product[col]=1-product[col];
            }
            for (unsigned row=0;row<product.size();row++){ 
                if (curConstraint[row][task]==0)
                    product[row]=100000000;
                else
                    product[row] = product[row]/curConstraint[row][task];
            }
            DBGPRINT("Product after dividing by curConstraint");
            DBGVECTOR(product);
            double maximo=0;
            for(int i=0;i<(int)product.size();i++) {
                if (maximo < product[i])
                    maximo = product[i];
            }
            lambdaVec.push_back(maximo);
            //DBGVAR(maximo);
            product.clear();
        }
        double minimo = lambdaVec[0];
        DBGPRINT("lambdaVec");
        DBGVECTOR(lambdaVec);
        for(int i=0;i<(int)lambdaVec.size();i++) {
            if (minimo > lambdaVec[i])
                minimo = lambdaVec[i];
            // DBGVAR(minimo);
        }
        //DBGVAR(minimo);
        return (minimo);
    }
    
    void SchedPoint::updateU(int task,Tick req)
    {
        wcet[task]=req;
        U[task]=double (double(wcet[task])/double(period[task]));
    }
    void SchedPoint::newRun()
    {
        last_change_time = 0;      
    }
    
    void SchedPoint::endRun()
    {
    }
    
}
