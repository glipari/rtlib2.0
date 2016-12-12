#include <json_trace.hpp>


namespace RTSim {

    using namespace std;
    using namespace MetaSim;
    

    JSONTrace::JSONTrace(const string& name)
    {
        fd.open(name.c_str());
        fd << "{" << endl;
        fd << "    \"events\" : \[" << endl; 
        first_event = true;
    }

    JSONTrace::~JSONTrace() {
        fd << "] }" << endl;
        fd.close();
    }

    void JSONTrace::writeTaskEvent(const Task &tt, const std::string &evt_name)
    {
        if (!first_event) 
            fd << "," << endl;
        else
            first_event = false;
        fd << "{ " ;
        fd << "\"time\" : \"" << SIMUL.getTime() << "\", ";  
        fd << "\"event_type\" : \"" << evt_name << "\", ";
        fd << "\"task_name\" : \"" <<  tt.getName() << "\",";
        fd << "\"arrival_time\" : \"" << tt.getArrival() << "\"}";
    }

    void JSONTrace::probe(ArrEvt& e)
    {
        Task& tt = *(e.getTask());
        writeTaskEvent(tt, "arrival");
    }
    
    void JSONTrace::probe(EndEvt& e)
    {
        Task& tt = *(e.getTask());
        writeTaskEvent(tt, "end_instance");
    }
    
    void JSONTrace::probe(SchedEvt& e)
    {
        Task& tt = *(e.getTask());
        writeTaskEvent(tt, "scheduled");
    }
    
    void JSONTrace::probe(DeschedEvt& e)
    {
        Task& tt = *(e.getTask());
        writeTaskEvent(tt, "descheduled");
    }
    
    void JSONTrace::probe(DeadEvt& e)
    {
        Task& tt = *(e.getTask());
        writeTaskEvent(tt, "dline_miss");
    }
    
    void JSONTrace::attachToTask(Task& t)
    {
        // new Particle<ArrEvt, JSONTrace>(&t->arrEvt, this);
        // new Particle<EndEvt, JSONTrace>(&t->endEvt, this);
        // new Particle<SchedEvt, JSONTrace>(&t->schedEvt, this);
        // new Particle<DeschedEvt, JSONTrace>(&t->deschedEvt, this);
        // new Particle<DeadEvt, JSONTrace>(&t->deadEvt, this);

        attach_stat(*this, t.arrEvt);
        attach_stat(*this, t.endEvt);
        attach_stat(*this, t.schedEvt);
        attach_stat(*this, t.deschedEvt);
        attach_stat(*this, t.deadEvt);
    }
}
