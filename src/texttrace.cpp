#include <texttrace.hpp>

namespace RTSim {
        using namespace std;
        using namespace MetaSim;

        TextTrace::TextTrace(const string& name)
		{
			fd.open(name.c_str());
		}

		TextTrace::~TextTrace()
		{
			fd.close();
		}

		void TextTrace::probe(ArrEvt& e)
		{
			Task* tt = e.getTask();
            fd << "[Time:" << SIMUL.getTime() << "]\t";  
            fd << tt->getName() << " arrived at " 
            << tt->getArrival() << endl;                
		}

		void TextTrace::probe(EndEvt& e)
		{
			Task* tt = e.getTask();
			fd << "[Time:" << SIMUL.getTime() << "]\t";
			fd << tt->getName()<<" ended, its arrival was " 
				<< tt->getArrival() << endl;
		}

		void TextTrace::probe(SchedEvt& e)
		{
			Task* tt = e.getTask();
			fd << "[Time:" << SIMUL.getTime() << "]\t";  
			fd << tt->getName()<<" scheduled its arrival was " 
				<< tt->getArrival() << endl; 
		}

		void TextTrace::probe(DeschedEvt& e)
		{
			Task* tt = e.getTask();
			fd << "[Time:" << SIMUL.getTime() << "]\t";  
			fd << tt->getName()<<" descheduled its arrival was " 
				<< tt->getArrival() << endl;
		}

		void TextTrace::probe(DeadEvt& e)
		{
			Task* tt = e.getTask();
			fd << "[Time:" << SIMUL.getTime() << "]\t";  
			fd << tt->getName()<<" missed its arrival was " 
				<< tt->getArrival() << endl;
		}

		void TextTrace::attachToTask(Task* t)
		{
			new Particle<ArrEvt, TextTrace>(&t->arrEvt, this);
			new Particle<EndEvt, TextTrace>(&t->endEvt, this);
			new Particle<SchedEvt, TextTrace>(&t->schedEvt, this);
			new Particle<DeschedEvt, TextTrace>(&t->deschedEvt, this);
			new Particle<DeadEvt, TextTrace>(&t->deadEvt, this);
		}
    
        VirtualTrace::VirtualTrace(map<string, int> *r)
        {
            results = r;
        }
        
        VirtualTrace::~VirtualTrace()
        {
            
        }
    
        void VirtualTrace::probe(EndEvt& e)
        {
            Task* tt = e.getTask();
            auto tmp_wcrt = SIMUL.getTime() - tt->getArrival();
            
            if ((*results)[tt->getName()] < tmp_wcrt)
            {
                (*results)[tt->getName()] = tmp_wcrt;
            }
        }

        void VirtualTrace::attachToTask(Task* t)
        {
            new Particle<EndEvt, VirtualTrace>(&t->endEvt, this);
        }
    
};