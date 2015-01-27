/***************************************************************************
begin                : Mon Nov 3 15:54:58 CEST 2014
copyright            : (C) 2014 Simoncelli Stefano
email                : simoncelli.stefano@hotmail.it
***************************************************************************/


#include <task.hpp>
#include <basestat.hpp>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <functional>
#include <regvar.hpp>
#include <factory.hpp>
#include <simul.hpp>
#include <abskernel.hpp>
#include <instr.hpp>
#include <time.h>

#define _USE_MATH_DEFINES 
#include <math.h>

namespace RTSim {

	using namespace std;
	using namespace MetaSim;
	using namespace parse_util;

	class WrongParameterSize : public BaseExc {
	public:
		WrongParameterSize(string msg) :BaseExc(msg, "AVRTask", "AVRTask.cpp") {}
	};

	class ModeOutOfIndex : public BaseExc {
	public:
		ModeOutOfIndex(string msg) :BaseExc(msg, "AVRTask", "AVRTask.cpp") {}
	};

	class AVRTask : public Task{

	protected:

		double AngularPeriod;
		double AngularPhase;
		double AngularDl;

		//current mode
		int mode;

		//Buffered values passed by activate(mode,dl)
		//For each job the relative deadline must be computed based on the current engine velocity (the same for the mode index)
		//they are buffered in order to be used also in overload conditions
		vector<Tick> BufferedDeadlines;
		vector<int> BufferedModes;

		//one vector<Instr*> for each mode
		//built by constructor or updated by changeStatuts()
		//!!!!this is not the task instruction queue, but all the possible instruction queues (one for each mode)
		vector<vector<Instr*>> myInstr;

		//These vectors are used to calculate the mode index (performed in another entity)
		//but they are parameters strictly related to the AVRTask itself
		vector<double> OmegaMinus;
		vector<double> OmegaPlus;

	
	public:

		virtual ~AVRTask();

		//Omega values to be passed in RPM
		AVRTask(double angPeriod, double angPhase, double angDl, vector<string> instr, vector<double> Omegaplus, 
			vector<double> Omegaminus, const std::string &name) throw(WrongParameterSize);
		
		virtual void newRun();
		
		//Pop RelDline and Mode values and updates the task instruction queue correctly 
		//(selecting instructions corresponding to the mode value)
		virtual void handleArrival(Tick arr);

		//post arrEvt for this task with mode and rdl values.
		virtual void activate(int mode, Tick rdl) throw (ModeOutOfIndex);

		//Updates his own Instr matrix with values passed by param
		void buildInstr(vector<string> param)  throw(ParseExc);

		//Updates task parameters
		//Only when the task is not active to be called before activate();
		void changeStatus(double angper, double angphase, double angdl, vector<string> instr, vector<double> OmegaM, vector<double> OmegaP) throw (TaskAlreadyActive);

		double getAngularPhase(){
			return AngularPhase;
		}
		
		double getAngularPeriod(){
			return AngularPeriod;
		}

		double getAngularDl(){
			return AngularDl;
		}

		vector<double> getOmegaPlus(){
			return OmegaPlus;
		}

		vector<double> getOmegaMinus(){
			return OmegaMinus;
		}

		virtual Tick getWCET(int index) throw (ModeOutOfIndex);

		static AVRTask* createInstance(vector<string>& par);

	};

};
