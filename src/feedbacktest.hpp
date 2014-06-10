#ifndef __FEEDBACKTEST_H__
#define __FEEDBACKTEST_H__

#include <vector>
#include <feedback.hpp>

/// to be generalized
#include <sparepot.hpp>

namespace RTSim {

    using namespace MetaSim;

    /**
       This is an abstract class that represents all feedback
       scheduling modules.
     */
    class FeedbackTestModule : public AbstractFeedbackModule {
        std::vector<int> deltas;
        int index;
        Supervisor *sp;
        SporadicServer *ss;
    public:
        FeedbackTestModule(const std::string &name);
        ~FeedbackTestModule();

        void setSupervisor(Supervisor *s, SporadicServer *s2);

        void addSample(int d);

        virtual void notify(const Tick &exec_time);
        virtual void newRun();
        virtual void endRun();
    };
}

#endif
