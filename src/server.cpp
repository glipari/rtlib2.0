#include <memory>
#include <cassert>

#include <factory.hpp>
#include <server.hpp>
#include <kernel.hpp>
#include <strtoken.hpp>


namespace RTSim {
    using namespace MetaSim;
    using namespace std;
    using namespace parse_util;

    string Server::status_string[] = {"IDLE", 
                                      "READY", 
                                      "EXECUTING",
                                      "RELEASING", 
                                      "RECHARGING"};
    
    Server::Server(const string &name, const string &s) :
        Entity(name),
        arr(0),
        last_arr(0),
        status(IDLE),
        dline(0),
        abs_dline(0),
        tasks(),
        last_exec_time(0),
        kernel(0),
        sched_(0),
        currExe_(0),
        _bandExEvt(this, &Server::onBudgetExhausted, Event::_DEFAULT_PRIORITY + 4),
        _dlineMissEvt(this, &Server::onDlineMiss, Event::_DEFAULT_PRIORITY + 6),
        _rechargingEvt(this, &Server::onRecharging, Event::_DEFAULT_PRIORITY - 1),
	_schedEvt(this, &Server::onSched),
	_deschedEvt(this, &Server::onDesched),
	_dispatchEvt(this, &Server::onDispatch, Event::_DEFAULT_PRIORITY + 5)
    {
        DBGENTER(_SERVER_DBG_LEV);
        string s_name = parse_util::get_token(s);
        // only for passing to the factory
        vector<string> p = parse_util::split_param(parse_util::get_param(s));
        // create the scheduler

        DBGPRINT_2("SCHEDULER: ", s_name);
        DBGPRINT("PARAMETERS: ");
        for (int i=0; i<p.size(); ++i) DBGPRINT(p[i]);

        auto_ptr<Scheduler> curr = FACT(Scheduler).create(s_name, p);
        sched_ = curr.release();
        if (!sched_) throw ParseExc("Server::Server()", s);

        sched_->setKernel(this);

        // register all event handlers
        //register_handler(_bandExEvt, this, &Server::onBudgetExhausted); 
        //register_handler(_dlineMissEvt, this, &Server::onDlineMiss);
        //register_handler(_rechargingEvt, this, &Server::onRecharging); 
        //register_handler(_schedEvt, this, &Server::onSched); 
        //register_handler(_deschedEvt, this, &Server::onDesched); 
        //register_handler(_dispatchEvt, this, &Server::onDispatch); 
    }

    Server::~Server()
    {
        delete sched_;
    }

    void Server::addTask(AbsRTTask &task, const string &params)
    {
        DBGENTER(_SERVER_DBG_LEV);
        task.setKernel(this);
        tasks.push_back(&task);
        DBGPRINT_2("Calling sched->addTask, with params = ", params);
        sched_->addTask(&task, params);
    }

    // Task interface
    void Server::schedule()
    {
        _schedEvt.process();
    }

    void Server::deschedule()
    {
        _deschedEvt.process();
    }

    Tick Server::getArrival() const
    {
        return arr;
    }

    Tick Server::getLastArrival() const
    {
        return last_arr;
    }

    void Server::setKernel(AbsKernel *k)
    {
        kernel = k;
    }

    Tick Server::getDeadline() const
    {
        return abs_dline;
    }

    Tick Server::getRelDline() const
    {
        return dline;
    }
        
    void Server::activate(AbsRTTask *task) 
    {
        DBGENTER(_SERVER_DBG_LEV);

        sched_->insert(task);
    }

    void Server::suspend(AbsRTTask *task)
    {
        DBGENTER(_SERVER_DBG_LEV);
        sched_->extract(task);
	
        if (getProcessor(task) != NULL) {
            task->deschedule();
            currExe_ = NULL;
            sched_->notify(NULL);
        }
    }

    void Server::dispatch()
    {
        DBGENTER(_SERVER_DBG_LEV);
	_dispatchEvt.drop();
	_dispatchEvt.post(SIMUL.getTime());
    }
        
    CPU *Server::getProcessor(const AbsRTTask *) const
    {
        return kernel->getProcessor(this);
    }

    CPU *Server::getOldProcessor(const AbsRTTask *) const
    {
        return kernel->getOldProcessor(this);
    }
        
    void Server::onArrival(AbsRTTask *t)
    {
        DBGENTER(_SERVER_DBG_LEV);

        sched_->insert(t);

        if (status == IDLE) {
            idle_ready();
            kernel->onArrival(this);
        }
        else if (status == EXECUTING) {
            dispatch();
        }
        else if (status == RELEASING) {
            releasing_ready();
            kernel->onArrival(this);
        }
        else if (status == RECHARGING || status == RELEASING) {
            DBGPRINT("Server is RECHARGING or READY, waiting");
        }
    }

    void Server::onEnd(AbsRTTask *t)
    {
        DBGENTER(_SERVER_DBG_LEV);

        assert(status == EXECUTING);
        sched_->extract(t);
        currExe_ = NULL;
        sched_->notify(NULL); // round robin case
        dispatch();
    }
        
    void Server::onBudgetExhausted(Event *)
    {
        DBGENTER(_SERVER_DBG_LEV);

        assert(status == EXECUTING);

        _dispatchEvt.drop();

	if(currExe_ != NULL){
            currExe_->deschedule();
            currExe_ = NULL;
        }

        kernel->suspend(this);
        sched_->notify(NULL);
        kernel->dispatch();

        executing_recharging();

        if (status == READY) kernel->onArrival(this);
    }

        
    void Server::onSched(Event *)
    {
        DBGENTER(_SERVER_DBG_LEV);

        assert(status == READY);

        ready_executing();
        dispatch();
    }
                
    void Server::onDesched(Event *)
    {
        DBGENTER(_SERVER_DBG_LEV);

        // I cannot assume it is still executing, maybe it is already
        // in recharging status (bacause of previous onEnd(task))
        if (status == EXECUTING) {
            executing_ready();
            // signal the task
            currExe_->deschedule();
            currExe_ = NULL;
            sched_->notify(NULL);
        }   
    }

    void Server::onDlineMiss(Event *)
    {
    }

    void Server::onRecharging(Event *)
    {
        DBGENTER(_SERVER_DBG_LEV);

        assert(status == RECHARGING);

        if (sched_->getFirst() != NULL) {
            recharging_ready();
            kernel->onArrival(this);
        }
        else {
            recharging_idle();
            currExe_ = NULL;
            sched_->notify(NULL);
        }
    }

    void Server::newRun()
    {
	arr = 0;
	last_arr=0;
	status = IDLE;
	dline = 0;
	abs_dline = 0;
	last_exec_time = 0;
	_bandExEvt.drop();
        _dlineMissEvt.drop();
        _rechargingEvt.drop();

        _schedEvt.drop();
        _deschedEvt.drop();
        _dispatchEvt.drop();
	currExe_ = NULL;

    }

    void Server::endRun()
    {
    }

    void Server::onDispatch(Event *e)
    {
        DBGENTER(_SERVER_DBG_LEV);

        AbsRTTask *newExe = sched_->getFirst();

        DBGPRINT("Current situation");
        DBGPRINT_2("newExe: ", taskname(newExe));
        DBGPRINT_2("currExe_: ", taskname(currExe_));
        
        if (newExe != currExe_) {
            if (currExe_ != NULL) currExe_->deschedule();
            currExe_ = newExe;
            if (newExe != NULL) newExe->schedule();
            //sched_->notify(newExe);
        }

        DBGPRINT_2("Now Running: ", taskname(newExe));

        if (currExe_ == NULL) {
            sched_->notify(NULL);
            executing_releasing();
            kernel->suspend(this);
            kernel->dispatch();
        }
    }

}


