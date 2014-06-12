#include <assert.h>
#include <grubserver.hpp>
#include <iostream>

using namespace std;

namespace RTSim {
    
    GrubSupervisor::GrubSupervisor(const std::string &name) : 
	Entity(name),
	servers(),
	total_u(0),
	residual_capacity(0),
	active_u(0) {}

    GrubSupervisor::~GrubSupervisor() {}

    bool GrubSupervisor::addGrub(Grub *g) 
    {
	if ( (total_u + g->getUtil()) > 1) 
	    return false;
	servers.push_back(g);
	g->set_supervisor(this);
	total_u += g->getUtil();
	return true;
    }

    void GrubSupervisor::set_active(Grub *g) 
    {
	for (auto sp = servers.begin();
	        sp != servers.end();
	            ++sp) 
	    (*sp)->updateBudget();

	active_u += g->getUtil();

	for (auto sp = servers.begin();
	        sp != servers.end();
	            ++sp) 
	    (*sp)->startAccounting();
    }

    void GrubSupervisor::set_idle(Grub *g) 
    {
	for (auto sp = servers.begin();
	        sp != servers.end();
	            ++sp) 
	    (*sp)->updateBudget();

	active_u -= g->getUtil();

	for (auto sp = servers.begin();
	        sp != servers.end();
	            ++sp) 
	    (*sp)->startAccounting();
    }

    Tick GrubSupervisor::get_capacity()
    { 
	Tick c = residual_capacity; 
	residual_capacity = 0;
	return c;
    }

    void GrubSupervisor::newRun() 
    {
	active_u = 0;
	residual_capacity=0;
	//cout << "NEW RUN" << endl;
    }

    void GrubSupervisor::endRun() 
    {
    }

    /*----------------------------------------------------*/

    Grub::Grub(Tick q, Tick p, const std::string &name, const std::string &sched) :
	Server(name, sched),
	Q(q),
	P(p),
	d(0),
	util(double(Q)/double(P)),
	recharging_time(0),
	cap(),
	vtime(),
	supervisor(0), 
	_idleEvt(this, &Grub::onIdle)
    {
	//register_handler(_idleEvt, this, &Grub::onIdle); 
    }

    Grub::~Grub() {}

    Tick Grub::getBudget() const { return Q; }
    Tick Grub::getPeriod() const { return P; }
    double Grub::getUtil() const { return util; }

    void Grub::updateBudget() 
    {
	if (status == EXECUTING) {
	    vtime.stop();
	    cap.stop();
	    _bandExEvt.drop();
	}
    }

    void Grub::startAccounting()
    {
	if (status == EXECUTING) {
	    vtime.start(supervisor->getActiveUtilization()/getUtil());
	    cap.start(-supervisor->getActiveUtilization());
	    Tick delta = cap.get_intercept(0);
	    if (delta < 0) {
		cout << "Task: " << dynamic_cast<Task *>(tasks[0])->getName() << endl;
		cout << "Time: " << SIMUL.getTime() << endl;
		cout << "Status: " << status << " -- intercept: " << delta << endl;
		cout << "capacity: " << cap.get_value() << endl;
		cout << "Supervisor utilization: " << supervisor->getActiveUtilization() << endl;
		assert(0);
	    }
	    _bandExEvt.post(SIMUL.getTime() + cap.get_intercept(0));
	}
    }
    
    void Grub::onIdle(Event *evt)
    {
	assert(status == RELEASING);
	releasing_idle();
    }

    void Grub::idle_ready()
    {
	//cout << "IDLE-READY" << endl;
	DBGENTER(_SERVER_DBG_LEV);
        assert (status == IDLE);
        status = READY;
        cap.set_value(Q);
	d = SIMUL.getTime() + P;
	DBGPRINT_2("new deadline ",d);
        setAbsDead(d);
	vtime.set_value(SIMUL.getTime());
	supervisor->set_active(this);
        DBGPRINT_2("Going to READY at ", SIMUL.getTime());
	//cout << "IDLE-READY end" << endl;
    }
    
    void Grub::releasing_ready()
    {
	DBGENTER(_SERVER_DBG_LEV);
	status = READY;
        _idleEvt.drop();
        DBGPRINT("FROM RELEASING TO READY");
    }
    
    void Grub::ready_executing()
    {
	DBGENTER(_SERVER_DBG_LEV);
        status = EXECUTING;
	Tick extra = supervisor->get_capacity();
	//cout << "Extra: " << extra << endl;
	cap.set_value(cap.get_value() + extra);
	startAccounting();
    }
    
    void Grub::executing_ready()
    {
	DBGENTER(_SERVER_DBG_LEV);
	updateBudget();
	status = READY;
    }
    
    void Grub::executing_releasing()
    {
	DBGENTER(_SERVER_DBG_LEV);
	updateBudget();
	status = RELEASING;
	if (SIMUL.getTime() < Tick(vtime.get_value())) 
	    _idleEvt.post(Tick(vtime.get_value()));
	else _idleEvt.post(SIMUL.getTime());
    }
    
    void Grub::releasing_idle()
    {
	DBGENTER(_SERVER_DBG_LEV);
	if (SIMUL.getTime() > vtime.get_value()) {
	    Tick extra = Tick(getUtil() * double(SIMUL.getTime() - vtime.get_value()));
	    supervisor->set_capacity(extra);
	}
	status = IDLE;
	supervisor->set_idle(this);
    }
    
    void Grub::executing_recharging()
    {
	DBGENTER(_SERVER_DBG_LEV);
	updateBudget();
	status = RECHARGING;
	if (getDeadline() < SIMUL.getTime()) 
	    _rechargingEvt.post(SIMUL.getTime()); 
	else 
	    _rechargingEvt.post(getDeadline());
    }
    
    void Grub::recharging_ready()
    {
	DBGENTER(_SERVER_DBG_LEV);
	cap.set_value(Q);
	d = SIMUL.getTime() + P;
	setAbsDead(d);
	vtime.set_value(SIMUL.getTime());
	status = READY;
    }

    void Grub::recharging_idle()
    {
	// no way
	//assert(0);
	//cout << "RECHARGING - IDLE!" << endl;
	releasing_idle();
    }
    
    Tick Grub::changeBudget(const Tick &new_budget)
    {
	// nothing to do now
	assert(0);
	return 0;
    }
    
    void Grub::set_supervisor(GrubSupervisor *s)
    {
	supervisor = s;
    }   

    void Grub::newRun()
    {
	Server::newRun();
	d = 0;
	recharging_time = 0;
    }

    void Grub::endRun()
    {
	Server::endRun();
    }

}
