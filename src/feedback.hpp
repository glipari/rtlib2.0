#ifndef __FEEDBACK_H__
#define __FEEDBACK_H__

#include <entity.hpp>
//#include <task.hpp>

namespace RTSim {

    using namespace MetaSim;

    class Task;

    /**
       This is an abstract class that represents all feedback
       scheduling modules.
     */
    class AbstractFeedbackModule : public Entity {
    protected:
        Task *task;
    public:
        AbstractFeedbackModule(const std::string &name);
        virtual ~AbstractFeedbackModule();

        void setTask(Task *t);

        virtual void notify(const Tick &exec_time) = 0;

        virtual void newRun() = 0;
        virtual void endRun() = 0;
    };
}

#endif
