/*The CBS is hard reservation or not depending on the bool variable true or false. */

#include <cstdlib>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>
#include <cstring>
#include <unistd.h>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <rmsched.hpp>
#include <edfsched.hpp>
#include <rrsched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <sporadicserver.hpp>
#include <feedback.hpp>
#include <feedbacktest.hpp>
#include <schedpoints.hpp>
#include <taskstat.hpp>
#include <feedbackarsim.hpp>
#include <strtoken.hpp>
#include <supercbs.hpp>
using namespace MetaSim;
using namespace RTSim;

class System {
 public:

	EDFScheduler sched;
	RTKernel kern;
	JavaTrace jtrace;

	 vector < PeriodicTask * >mm;
	 vector < CBServer * >ss;
	 vector < MissPercentage * >miss;

	 vector < TardinessStat < StatMean > *>tstat;

	 System();

	void addMultimediaTask(const string & name, Tick b, Tick p,
			       const string & trace, int prio);

	void addLoadTask(const string & name, Tick b, Tick p, int prio);

};

 System::System():
sched(), kern(&sched), jtrace("trace.trc"), mm(), ss(), miss(), tstat()
{
}

void System::addMultimediaTask(const string & name,
			       Tick b, Tick p, const string & tfile, int prio)
{
	// creating and inserting task
	PeriodicTask *m1 = new PeriodicTask(p, p, 0, name);
	m1->insertCode("delay(trace(" + tfile + "));");
	mm.push_back(m1);

	// creating and inserting server
	CBServer *s =
	    new CBServer(b, p, p, true, "Server_" + name, "FIFOSched");
	s->addTask(*m1);
	ss.push_back(s);

	stringstream mystr;
	mystr << prio;
	kern.addTask(*s, mystr.str());

	MissPercentage *mp = new MissPercentage("miss_" + name);
	mp->attachToTask(m1);
	miss.push_back(mp);

	TardinessStat < StatMean > *ts =
	    new TardinessStat < StatMean > ("tard_" + name);
	ts->attachToTask(m1);
	tstat.push_back(ts);
	m1->setTrace(&jtrace);
}

void System::addLoadTask(const string & name, Tick b, Tick p, int prio)
{
	PeriodicTask *m1 = new PeriodicTask(p, p, 0, name);

	stringstream mystr;
	mystr << "fixed(" << (int)b << ");";
	cout << "Instruction: " << mystr.str() << endl;

	m1->insertCode(mystr.str());
	mm.push_back(m1);

	CBServer *s1 = new CBServer(b, p, p, true, "Server_" + name,
				    "FIFOSched");

	s1->addTask(*m1);
	ss.push_back(s1);

	stringstream mystr2;
	mystr2 << prio;

	kern.addTask(*s1, mystr2.str());
	m1->setTrace(&jtrace);
}

vector < string > get_tokens(ifstream & f, int &count)
{
	string line;
	vector < string > tokens;
	try {
		do {
			getline(f, line);
			count++;
			if (f.eof())
				return tokens;
			line = parse_util::remove_spaces(line);
		}
		while (line[0] == '#' || line.size() < 1);	// skip comment and empty lines

		tokens = parse_util::split(line, ";");
		//  cout << "Line parsed with " << tokens.size() << " tokens" << endl;
		//for (int i=0; i<tokens.size(); i++) cout << tokens[i] << "; ";
		//cout << endl;
	}
	catch(exception & e) {
		cerr << "Error in parsing file: " << endl;
		cerr << e.what();
		exit(0);
	}
	return tokens;
}

void read_tasks(const string & data_file, System & s)
{
	int count = 0;
	ifstream f(data_file.c_str());

	if (!f.is_open()) {
		cerr << "Can't open " << data_file << endl;
		exit(-1);
	}

	do {
		vector < string > tokens = get_tokens(f, count);
		if (tokens.size() == 0)
			break;

		if (tokens.size() < 5) {
			cout << "Error, wrong number of tokens at line" << count
			    << endl;
			exit(0);
		}

		double avg;
		double period;
		double prio;
		string unit;
		try {
			parse_util::parse_double(tokens[2], avg, unit);
			parse_util::parse_double(tokens[3], period, unit);
			parse_util::parse_double(tokens[4], prio, unit);
		} catch(exception & e) {
			cerr << "Error in parsing" << endl;
			cerr << e.what() << endl;
			exit(0);
		}

		if (tokens[1] == "m") {
			if (tokens.size() == 6) {
				//      cout << "Creating a multimedia task with " 
				//     << avg << ", " << period << ", " << prio << ", " 
				//     << tokens[5] << endl;

				s.addMultimediaTask(tokens[0], Tick(avg),
						    Tick(period), tokens[5],
						    int (prio));
			} else {
				cerr << "Wrong number of tokes at line: " <<
				    count << endl;
				exit(0);
			}
		} else if (tokens[1] == "l") {
			// cout << "Creating a load task with " 
			//     << avg << ", " << period << ", " << prio << endl;

			s.addLoadTask(tokens[0], Tick(avg), Tick(period),
				      int (prio));
		}
	}
	while (!f.eof());
}

void create_feedback(const string & fback_file, Supervisor * super)
{
	string line;
	int count = 0;
	ifstream f(fback_file.c_str());

	if (!f.is_open()) {
		cerr << "Can't open " << fback_file << endl;
		exit(-1);
	}

	try {
		do {
			// DBGENTER(_SERVER_DBG_LEV);
			vector < string > tokens = get_tokens(f, count);
			if (tokens.size() == 0)
				break;

			if (tokens.size() < 2) {
				cout << "Error, wrong number of tokens at line"
				    << count << endl;
				exit(0);
			}
			string task_name = tokens[0];
			string controller_params = tokens[1];

			PeriodicTask *mytask =
			    dynamic_cast <
			    PeriodicTask * >(Entity::_find(task_name));
			if (mytask == 0) {
				cerr << "Error: task " << task_name <<
				    " not found" << endl;
				exit(0);
			}

			Server *server =
			    dynamic_cast <
			    Server * >(Entity::_find("Server_" + task_name));
			if (server == 0) {
				cerr << "Error: server Server_" << task_name <<
				    " not found" << endl;
				exit(0);
			}
			FeedbackModuleARSim *ftm1 =
			    new FeedbackModuleARSim(task_name + "_FTM");
			//      DBGPRINT("Setting controller params: " << controller_params);
			ftm1->setControllerParams(controller_params);
			ftm1->setSupervisor(super, server);
			ftm1->setTask(mytask);
			mytask->setFeedbackModule(ftm1);
		}
		while (!f.eof());
	}
	catch(ParseExc & e) {
		cerr << e.what() << endl;
	}
}

int main(int argc, char *argv[])
{
	System sys;
	FILE *fid_1, *fid_2;
	int rta, budget, temp, period, i, j;
	bool schedulable;
	SIMUL.dbg.enable(_SERVER_DBG_LEV);
	SIMUL.dbg.enable(_ARSIM_DBG_LEV);
	SIMUL.dbg.enable(_TASK_DBG_LEV);
	SIMUL.dbg.enable(_KERNEL_DBG_LEV);

	if (argc < 4) {
		cout << "Usage: " << argv[0] <<
		    " [sim_time] [file name] <fix|schedp|sparepot|cbs> [feedback params]"
		    << endl;
		exit(0);
	}

	read_tasks(argv[2], sys);

	cout << "NOW SELECTING SIMULATION TYPE (sys.ss.size()=" << sys.ss.size() << ")" << endl;

	if (strcmp("fix", argv[3]) == 0) {
		cout << "Fixed server simulation" << endl;
	} else if (strcmp("schedp", argv[3]) == 0 ||
		   strcmp("sparepot", argv[3]) == 0) {
		if (argc < 5) {
			cerr << "Missing the feedback parameters file" << endl;
			exit(-1);
		}
		if (strcmp("schedp", argv[3]) == 0) {
			cout << "SCHED_POINTS" << endl;
			SchedPoint *super_schedp;
			super_schedp = new SchedPoint("SchedPoint");

			cout << "Now adding the servers" << endl;
			for (i = 0; i < sys.ss.size(); i++) {
				cout << "Adding server " << i << endl;
				super_schedp->addServer(sys.ss[i]);
			}

			cout << "Building constraints" << endl;
			super_schedp->buildconstraints();

			cout << "Creating feedback" << endl;
			create_feedback(argv[4], super_schedp);
		} else if (strcmp("sparepot", argv[3]) == 0) {
			cout << "SPARE_POT" << endl;
			SparePot *super_sparep;
			super_sparep = new SparePot("SparePot");

			for (i = 0; i < sys.ss.size(); i++)
				super_sparep->addServer(sys.ss[i]);
	    /**********************************************************************************************/
			/*Added by Rodrigo. Computes the budget and period for the spare pot */

			schedulable = true;
			budget = 0;
			period = sys.ss[0]->getPeriod();
			while (schedulable) {
				budget++;
				for (i = 0; i < sys.ss.size(); i++) {
					rta = 0;
					temp = sys.ss[i]->getBudget();
					while (rta != temp) {
						rta = temp;
						temp = 0;
						for (j = 0; j < i; j++) {
							temp =
							    temp +
							    double (sys.ss[j]->
								    getBudget())
							    *
							    ceil(double (rta) /
								 double (sys.
									 ss[j]->
									 getPeriod
									 ()));
						}
						temp =
						    temp +
						    double (sys.ss[i]->
							    getBudget()) +
						    double (budget) *
						    ceil(double (rta) /
							 double (period));
					}
					if (temp > sys.ss[i]->getPeriod()) {
						schedulable = false;
						break;
					}
				}
			}
			budget--;
			super_sparep->compute_matrix(Tick(budget),
						     Tick(period));
			cout << "budget " << budget << " Period " << period <<
			    endl;
			create_feedback(argv[4], super_sparep);
		}
	}
	else if (strcmp("cbs", argv[3]) == 0) {
		//////////// new cbs 
		cout << "CBS-EDF" << endl;
		SuperCBS *feedback;
		//SparePot *super_sparep;
		feedback = new SuperCBS("CBS");
		for (i = 0; i < sys.ss.size(); i++) {
			printf("ADDING SERVER");
			feedback->addServer(sys.ss[i]);
		}

		create_feedback(argv[4], feedback);
	} else {
		cerr << "ERROR: wrong argument" << endl;
		exit(-1);
	}

	Tick sim_time = atoi(argv[1]);

	cout << "Running simulation for " << sim_time << endl;
	try {
		SIMUL.run(sim_time);
	}
	catch(BaseExc & e) {
		cout << e.what() << endl;
	}

	cout << "Miss size: " << sys.miss.size() << endl;

	for (int i = 0; i < sys.miss.size(); i++) {
		cout << "Percentage of dline misses of i= " << i << " : "
		    << sys.miss[i]->getLastValue()
		    << " over " << sys.miss[i]->getNumSamples() << endl;
	}

	for (int i = 0; i < sys.tstat.size(); i++) {
		cout << "Tardiness of i= " << i << " : "
		    << sys.tstat[i]->getLastValue() << endl;
	}

	fid_1 = fopen("deadlinesmiss_matrix_hcbs.txt", "a");
	fid_2 = fopen("tardiness_matrix_hcbs.txt", "a");
	for (int i = 0; i < sys.miss.size(); i++) {
		fprintf(fid_1, "%f\t%i\t", sys.miss[i]->getLastValue(),
			sys.miss[i]->getNumSamples());
	}
	fprintf(fid_1, "\n");
	for (int i = 0; i < sys.tstat.size(); i++) {
		fprintf(fid_2, "%f\t", sys.tstat[i]->getLastValue());

	}
	fprintf(fid_2, "\n");
	fclose(fid_1);
	fclose(fid_2);

//     if (argc < 6) {
//         cout << "Usage: " << argv[0] << " <fix|schedp|sparep> " 
//              << "<budget 1> <budget 2> <tracefile 1> <tracefile 2>" << endl;
//         exit(0);
//     }

//     try {
//         JavaTrace jtrace("trace.trc");
//         TextTrace ttrace("trace.txt");
//         Tick b1, b2;
//         string tracefile1, tracefile2;

//         b1 = atoi(argv[2]);
//         b2 = atoi(argv[3]);

//         tracefile1 = string(argv[4]);
//         tracefile2 = string(argv[5]);

//         SIMUL.dbg.enable(_EVENT_DBG_LEV); 
//         System sys(b1, b2, tracefile1, tracefile2);

//      sys.mm1.setTrace(&jtrace);
//         sys.mm2.setTrace(&jtrace);
//      //        sys.ss1.setTrace(&jtrace);
//         //sys.ss2.setTrace(&jtrace);

// //sys.tl.setTrace(&jtrace);

//         if (string(argv[1]) == "fix") {
//             prepare_sporadic(sys);
//         }
//         else if (string(argv[1]) == "fback") {
//             prepare_feedback(sys); 
//         }
//         else {
//             cout << "Usage: " << argv[0] << " <fix|fback> " 
//                  << "<budget 1> <budget 2> <tracefile 1> <tracefile 2>" << endl;
//             exit(0);
//         }

//      SchedPoint schpt("SchedPoint");

//         schpt.addServer(&sys.ss1);
//         schpt.addServer(&sys.ss2);

//         schpt.buildconstraints();

//         FeedbackModuleARSim ftm1("FTM1");
//         ftm1.setSupervisor(&schpt,&sys.ss1);
//         ftm1.setTask(&sys.mm1);
//         sys.mm1.setFeedbackModule(&ftm1);

//         FeedbackModuleARSim ftm2("FTM2");
//         ftm2.setSupervisor(&schpt,&sys.ss2);
//         ftm2.setTask(&sys.mm2);
//         sys.mm2.setFeedbackModule(&ftm2);

//          ftm1.setControllerParams("-s sdb -T 400 -B 0.5");
//          ftm2.setControllerParams("-s sdb -T 1000 -B 0.5");
//       //        ftm1.setControllerParams("-s fs -T 400 -B 0.01");   // budget=4
//       //ftm2.setControllerParams("-s fs -T 1000 -B 0.01");  // budget=10

//      //ttrace.attachToTask(&sys.ss1);
//      //ttrace.attachToTask(&sys.ss2);

//         SIMUL.run(3000);

//         cout << "Percentage of deadline misses for task mm1: " 
//              << sys.miss1.getLastValue() << endl;
//         cout << "Percentage of deadline misses for task mm2: " << 
//             sys.miss2.getLastValue() << endl;

//         cout << "Tardiness for task mm1: " 
//              << sys.ts1.getLastValue() << endl;
//         cout << "Tardiness for task mm2: " << 
//             sys.ts2.getLastValue() << endl;

//     } catch (BaseExc &e) {
//         cout << e.what() << endl;
//     } catch (parse_util::ParseExc &e2) {
//         cout << e2.what() << endl;
//     }
}
