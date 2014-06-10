#include <feedbacktest.hpp>

namespace RTSim {
    FeedbackTestModule::FeedbackTestModule(const std::string &name) :
        AbstractFeedbackModule(name), sp(0)
    {
        
    }

    FeedbackTestModule::~FeedbackTestModule()
    {
    }

    void FeedbackTestModule::setSupervisor(Supervisor *s, SporadicServer *s2)
    {
        sp = s;
        ss = s2;
    }

    void FeedbackTestModule::addSample(int d)
    {
        deltas.push_back(d);
    }


    void FeedbackTestModule::notify(const Tick &exec_time)
    {
        sp->changeBudget(ss, deltas[index]);
        index = (index + 1) % deltas.size();
    }

    void FeedbackTestModule::newRun()
    {
        index = 0;
    }

    void FeedbackTestModule::endRun()
    {
    }

}
