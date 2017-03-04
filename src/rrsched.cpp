#include <rrsched.hpp>
#include <task.hpp>
#include <cassert>
#include <sstream>

namespace RTSim {
    using namespace MetaSim;
    using namespace std;
    
    bool RRScheduler::RRModel::isRoundExpired()
    {
        Task* t = dynamic_cast<Task*>(_rtTask);
        
        assert(t != NULL);

        Tick last = t->getLastSched();
        if (_rrSlice > 0 && (SIMUL.getTime() - last) >= _rrSlice) 
            return true;
        return false;
    }
    
    Tick RRScheduler::RRModel::getPriority() const
    {
        return 1;
    }

    void RRScheduler::RRModel::changePriority(Tick p)
    {
        throw RRSchedExc("Cannot change priority in RRSched!");
    }

    RRScheduler::RRScheduler(int defSlice) : 
        Scheduler(), defaultSlice(defSlice), _rrEvt(this, &RRScheduler::round)
    {
        DBGENTER(_RR_SCHED_DBG_LEV);
        DBGPRINT_2("DEFAULT SLICE = ", defaultSlice);
    }


    
    void RRScheduler::setRRSlice(AbsRTTask* task, Tick slice)
    {
        RRModel* model = dynamic_cast<RRModel *>(find(task));
        if (model == 0) throw RRSchedExc("Cannot find task");
        model->setRRSlice(slice);
    }

    void RRScheduler::notify(AbsRTTask* task)
    {
        DBGENTER(_RR_SCHED_DBG_LEV);
        _rrEvt.drop();

        if (task != NULL) {
            RRModel* model = dynamic_cast<RRModel *>(find(task));
            if (model == 0) throw RRSchedExc("Cannot find task");
            if (model->getRRSlice() > 0) {
                _rrEvt.post(SIMUL.getTime() + model->getRRSlice());
                DBGPRINT_2("rrEvt post at time ", 
                           SIMUL.getTime() + model->getRRSlice());
            }
        }
      
        
    }

    void RRScheduler::round(Event *)
    {
        DBGENTER(_RR_SCHED_DBG_LEV);
        RRModel* model = dynamic_cast<RRModel *>(_queue.front());
        if (model == 0) throw RRSchedExc("Cannot find task");

        if (model->isRoundExpired()) {
            DBGPRINT("Round expired");
            _queue.erase(model);
            if (model->isActive()) {
                model->setInsertTime(SIMUL.getTime());
                _queue.insert(model);
            }
        }
        
//         if it is not active... ?
        RRModel* first = dynamic_cast<RRModel *>(_queue.front());
//         if (first == 0) throw RRSchedExc("Cannot find task");

        if (first != 0) {
            Tick slice = first->getRRSlice();
            if (slice == 0) _rrEvt.drop();
            else {
                if (first != model || first->isRoundExpired()) {
                    _rrEvt.drop();
                    _rrEvt.post(SIMUL.getTime() + slice);
                }
            }
        }
        if (_kernel) {
            DBGPRINT("informing the kernel");
            _kernel->dispatch();
        }
        
    }

    void RRScheduler::addTask(AbsRTTask *task) throw(RRSchedExc)
    {
        DBGENTER(_RR_SCHED_DBG_LEV);

        RRModel *model = new RRModel(task); 	

        if (find(task) != NULL) 
            throw RRSchedExc("Element already present");
	
        _tasks[task] = model;
        
        model->setRRSlice(defaultSlice);
        DBGPRINT_2("Default slice set: ", defaultSlice); 
        
    }

    void RRScheduler::addTask(AbsRTTask* task, const std::string &p)
    {
        DBGENTER(_RR_SCHED_DBG_LEV);
        AbsRTTask *mytask = dynamic_cast<AbsRTTask *>(task);

        if (mytask == 0) 
            throw RRSchedExc("Cannot add a AbsRTTask to RR");

        RRModel *model = new RRModel(mytask); 	

        if (find(task) != NULL) 
            throw RRSchedExc("Element already present");
	
        _tasks[task] = model;
        
        int slice = 0;
        
        DBGPRINT_2("Slice parameter: ", p); 
        if (p == "") slice = defaultSlice;
        else {
            stringstream ss(p);
            ss >> slice;
        }

        DBGPRINT_2("Slice set to: ", slice); 

        model->setRRSlice(slice);
        
    }

    RRScheduler *RRScheduler::createInstance(vector<string> &par)    
    {
        int slice;
        stringstream ss(par[0]);
        ss >> slice;
        return new RRScheduler(slice);
    }
}
