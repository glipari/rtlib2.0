#include <feedbackarsim.hpp>

#include <arsim/TaskPredictor.hpp>
#include <arsim/FileUtil.hpp>

#include <string.h>
#include <assert.h>

namespace RTSim {
    FeedbackModuleARSim::FeedbackModuleARSim(const std::string &name) :
        AbstractFeedbackModule(name), sp(0), ss(0), p_ctrl(0),
        _sched_err(0), _required_bw(0), _granted_bw(0)
    {
    }

    FeedbackModuleARSim::~FeedbackModuleARSim()
    {
    }

    void FeedbackModuleARSim::setSupervisor(Supervisor *s, Server *s2)
    {
        sp = s;
        ss = s2;
    }

    void FeedbackModuleARSim::notify(const Tick &exec_time)
    {
        onJobEnd(exec_time);
    }

    void FeedbackModuleARSim::newRun()
    {
    }

    void FeedbackModuleARSim::endRun()
    {
    }
    /** @TODO Check why a ARSim::Controller needs to know its
        supervisor (a.k.a., ResourceManager) and associated Task */
    bool FeedbackModuleARSim::parseArg(int& argc, char **&argv)
    {
        //DBGENTER(_ARSIM_DBG_LEV);
        if (strcmp(*argv, "-s") == 0) {
            CHECK(argc > 1, "Option requires an argument");
            argv++;  argc--;
            p_ctrl = Controller::getInstance(*argv);
            CHECK(p_ctrl != NULL, "Wrong scheduler type");
            DBGPRINT("# Controller: " << *argv); 
        } else if (p_ctrl != 0) {
            DBGPRINT("Controller is set, now parsing controller arguments");
            return p_ctrl->parseArg(argc, argv);
        } else {
            DBGPRINT("Controller is not set, returning false");
            return false;
        }
        return true;
    }

    void FeedbackModuleARSim::parseArgs(int argc, char **argv)
    {
        //DBGENTER(_ARSIM_DBG_LEV);
        while (argc > 0) {
            if (parseArg(argc, argv)) {
                argv++;  argc--;
            } else {
                printf("Unknown option: %s\n", *argv);
                exit(-1);
            }
        }
    }

    void FeedbackModuleARSim::setControllerParams(const std::string & params)
    {
        char *c_str = strdup(params.c_str());
        assert(c_str != NULL);
        char *argv[80];
        int argc = stringToArgv(c_str, argv, sizeof(argv) / sizeof(argv[0]));
        parseArgs(argc, argv);
    }

    void FeedbackModuleARSim::onJobEnd(const Tick &exec_time) //, const Tick &sched_err)
    {
        DBGENTER(_SERVER_DBG_LEV);
        Tick delta_budget;
        p_ctrl->getTaskPredictor()->addSample(double(exec_time));
	if (ss != 0) {
            _sched_err = double(ss->getVirtualTime() - task->getDeadline());
	    DBGVAR(_sched_err);
	    double start_err = std::max<double>(_sched_err, 0.0);
	    DBGVAR(start_err);
            _required_bw = p_ctrl->calcBandwidth(_sched_err, start_err);
	    DBGVAR(_required_bw);
	    //MODIFICHE DI RODRIGO
	    //@TODO convert to assert
	    _required_bw = std::max(_required_bw, 0.0);
            DBGVAR(_sched_err);
            DBGVAR(_required_bw);
            delta_budget = -(Tick)floor( (double)ss->getBudget()-(double)ss->getPeriod()*(double)_required_bw );
            DBGVAR(delta_budget);
	    Tick old_budget = ss->getBudget();
	    Tick granted_delta_budget = sp->changeBudget(ss, delta_budget);
	    Tick new_budget = old_budget + granted_delta_budget;
	    DBGVAR(new_budget);
	    _granted_bw = double(new_budget) / (double)ss->getPeriod();
            DBGVAR(_granted_bw);
	}
    }

    /** @TODO Supervisor from ARSim, or from RTLib ? */
    void FeedbackModuleARSim::onJobStart(const Tick &start_error)
    {
        double start_err = double(start_error);
        _required_bw = p_ctrl->calcBandwidth(_sched_err, start_err);
        _granted_bw = _required_bw;
    }

    Tick FeedbackModuleARSim::getRequiredBudget() const
    {
        return Tick(_required_bw * p_ctrl->getTask()->getPeriod());
    }

    /** @TODO Transform bandwidth into budget */
    Tick FeedbackModuleARSim::getGrantedBudget() const
    {
        return Tick(_granted_bw * p_ctrl->getTask()->getPeriod());
    }

}
