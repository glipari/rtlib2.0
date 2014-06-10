#ifndef __FEEDBACKARSIM_H__
#define __FEEDBACKARSIM_H__

#include <vector>
#include <feedback.hpp>
#include <server.hpp>

/// ARSim includes
#include <arsim/Controller.hpp>
/// to be generalized
#include <sparepot.hpp>

#define _ARSIM_DBG_LEV "arsim"

namespace RTSim {

    using namespace MetaSim;

    /**
       This is an abstract class that represents all feedback
       scheduling modules.
     */
    class FeedbackModuleARSim : public AbstractFeedbackModule {
//         std::vector<int> deltas;
//         int index;
        Supervisor *sp;
        Server *ss;
        Controller *p_ctrl;
        double _sched_err, _required_bw, _granted_bw;

    public:

        FeedbackModuleARSim(const std::string &name);
        ~FeedbackModuleARSim();

        void setSupervisor(Supervisor *s, Server *s2);

//         void addSample(int d);

        virtual void notify(const Tick &exec_time);
        virtual void newRun();
        virtual void endRun();

       /** Called at end of job for updating predictor
        **
        ** @param exec_time The execution time of the just finished
        **   job
	**
	** @param sched_err The scheduling error (negative if job
	**   finished early, positive if job was late)
        **/
        virtual void onJobEnd(const Tick &exec_time);//, const Tick &sched_err);

        /** Called at beginning of job for updating feedback
	 **
	 ** @param start_error The start-time of the job relative to
	 **   the ideal one (i.e., how much late this job is starting)
	 **/
        virtual void onJobStart(const Tick &start_error);

        /** Retrieve the maximum budget as required by the controller */
        virtual Tick getRequiredBudget() const;

        /** Retrieve the maximum budget as granted by the supervisor */
        virtual Tick getGrantedBudget() const;

        /** Configure controller options
	 **
	 ** @param params is a string in "command-line" format (e.g.,
	 ** "-s sdb -e 0.0" for setting a StochasticDeadBeat with a
	 ** target of 0.0 -- type arsim -h for more options)
	 **/
        void setControllerParams(const std::string & params);

    private:

        /** Parse the next argument and advance argc/argv accordingly.
	 **
	 ** This method supports an argc/argv based configuration,
	 ** a'la ARSim.  If the option in *arv is recognized and it
	 ** does not need any arguments, then simply return true and
	 ** argc/argv are untouched. If, on the other hand, it
	 ** requires one (or more) arguments (e.g., -x 1), then argc
	 ** is accordingly decreased, and argv moved forward.
	 ** 
	 ** @return true if argument in *argv, along with possible
	 ** following options, was recognized and correctly parsed,
	 ** false otherwise.
	 **/
        bool parseArg(int& argc, char **&argv);

        /** Parse all the arguments supplied by argc/argv.
	 ** 
	 ** This method supports an argc/argv based configuration,
	 ** a'la ARSim
	 **/
        void parseArgs(int argc, char **argv);

    };
}

#endif
