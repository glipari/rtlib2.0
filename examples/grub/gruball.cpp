#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <kernel.hpp>
#include <edfsched.hpp>
#include <fifosched.hpp>
#include <jtrace.hpp>
#include <texttrace.hpp>
#include <rttask.hpp>
#include <grubserver.hpp>
#include <load.hpp>
#include <taskstat.hpp>

#include <boost/program_options.hpp>

#include "sim_structs.hpp"

using namespace MetaSim;
using namespace RTSim;

using namespace std;

#define N_HIGH_TASKS    4
#define N_LOW_TASKS     4

double highUtil;
double lowHighRatio_Min;
double lowHighRatio_Max;
int High_Min_Period;
int High_Max_Period;
int Low_Min_Period;
int Low_Max_Period;
int period_gcd;
int NRep;
double RStep;

int SimLength;

bool SingleServer = false;
string base_name;

bool ConstantUtil = false;

int main(int argc, char*argv[])
{
    namespace po = boost::program_options; 
    
    po::options_description desc("Options"); 
    desc.add_options() 
	("help", "Print help messages") 
	("high_u", po::value<double>(&highUtil)->default_value(.5), "Min utilization of HI-tasks") 
	("rstep", po::value<double>(&RStep)->default_value(.05), "Step of ratio") 
	("ratio_min", po::value<double>(&lowHighRatio_Min)->default_value(.75), "Min Ratio between utilization in Low Critical mode and High Critical mode of HI-tasks") 
	("ratio_max", po::value<double>(&lowHighRatio_Max)->default_value(.75), "Max Ratio between utilization in Low Critical mode and High Critical mode of HI-tasks") 
	("high_min_p", po::value<int>(&High_Min_Period)->required(), "Minimum period of High-Tasks") 
	("high_max_p", po::value<int>(&High_Max_Period)->required(), "Maximum period of High-Tasks") 
	("low_min_p", po::value<int>(&Low_Min_Period)->required(), "Minimum period of Low-Tasks") 
	("low_max_p", po::value<int>(&Low_Max_Period)->required(), "Maximum period of Low-Tasks") 
	("gcd", po::value<int>(&period_gcd)->default_value(100), "GCD of periods") 
	("rep", po::value<int>(&NRep)->default_value(10), "Number of repetitions")
	("constant", po::value<bool>(&ConstantUtil)->default_value(false), "Constant utilization of low tasks")
	("single", po::value<bool>(&SingleServer)->default_value(false), "True is all LO-tasks go in a single server")
	("len", po::value<int>(&SimLength)->default_value(10000000), "Lenght of each simulation")
	("output", po::value<string>(&base_name)->default_value("st_"), "base name of the output files"); 
 
    po::variables_map vm; 
    try 
    { 
	po::store(po::parse_command_line(argc, argv, desc),  
		  vm); // can throw 
	
	/** --help option 
	 */ 
	if ( vm.count("help")  ) 
	{ 
	    std::cout << "Basic Command Line Parameter App" << std::endl 
		      << desc << std::endl; 
	    return 0; 
	} 
	
	po::notify(vm); // throws on error, so do after help in case 
	// there are any problems 
    } catch(po::error& e) { 
	std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
	std::cerr << desc << std::endl; 
	return -1; 
    } catch(std::exception& e) { 
	std::cerr << "Unhandled Exception reached the top of main: " 
		  << e.what() << ", application will now exit" << std::endl; 
	return -2; 
    } 
    
    try {
	double u_step = .05;
	int i = 0;
	for (double ratio=lowHighRatio_Min ; ratio < lowHighRatio_Max; ratio+=RStep) {
	    cout << "Ratio: " << ratio << endl;
	    // create the scheduler and the kernel
	    Stats mystats;
	    for (int k=0; k<NRep; k++) {
		MySim sim(highUtil, ratio);

		//preparing the HIGH CRITICAL TASKS
		RandomTaskSetFactory ts(N_HIGH_TASKS, highUtil * ratio,  
					new ConstIATGen(High_Min_Period, High_Max_Period, High_Min_Period, period_gcd), 
					new ConstCTGen,
					new DlineEquPeriodDTGen);
		
		vector<Grub *> servers;
		for (unsigned j=0; j<ts.size(); j++) {
		    ts.getTask(j)->setRelDline(ts.getMaxIAT(j));
		    cout << "Created task: (" 
			 << ts.getMaxCT(j) << " , " << ts.getMaxIAT(j) << "," 
			 << ts.getTask(j)->getRelDline() << ")" <<  endl;
		    sim.create_high_server(*ts.getTask(j));
		}
		
		// generating the LOW CRITICALITY task set
		// I generate an utilization which is 
		double lu;
		if (ConstantUtil) lu = .99 - highUtil * lowHighRatio_Max;
		else lu = .99 - highUtil * ratio;
		
		RandomTaskSetFactory ts2(N_LOW_TASKS, lu,  
					 new ConstIATGen(Low_Min_Period, Low_Max_Period, Low_Min_Period, period_gcd), 
					 new ConstCTGen,
					 new DlineEquPeriodDTGen);
		for (unsigned j=0; j<ts2.size(); j++) {
		    ts2.getTask(j)->setRelDline(ts2.getMaxIAT(j));
		    cout << "Created task: (" <<
			ts2.getMaxCT(j) << " , " << 
			ts2.getMaxIAT(j) << "," << 
			ts2.getTask(j)->getRelDline() << ")" <<  endl;
		    // create a server for a LOW CRITICALITY TASK
		    // I have to allocate less bandwidth. 
		    // The actual available bandwidth is  (1 - HighUtil)
		    // I need to allocate (1 - HighUtil) / (1 - HighLowUtil)
		    
		    if (SingleServer) {
			sim.create_single_server(*ts2.getTask(j), period_gcd);
		    } else {
			sim.create_low_server(*ts2.getTask(j), lu);
		    }
		    
		    mystats.miss_count.attachToTask(ts2.getTask(j));
		    mystats.miss_perc.attachToTask(ts2.getTask(j));
		    mystats.tard.attachToTask(ts2.getTask(j));
		}
		cout << "DONE! Now start simulation" << endl;
		int n = 1;
		if (k == 0)  n = -NRep;
		else if (k == NRep-1) n = 0;
		else n = -1;
		SIMUL.run(SimLength, n);
	    }
	    mystats.dump(ratio, base_name);
	}
    } catch (BaseExc &e) {
        cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }        
}
