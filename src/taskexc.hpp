#ifndef __TASKEXC_HPP__
#define __TASKEXC_HPP__

#include <baseexc.hpp>

namespace RTSim {

    /* Exceptions */
    class EmptyTask : public BaseExc {
    public:
        EmptyTask()
            :BaseExc("Task witout instructions in queue", "Task", "task.cpp") {}
    };
    class KernAlreadySet: public BaseExc {
    public:
        KernAlreadySet()
            :BaseExc("Kernel already set for this task", "Task", "task.cpp") {}
    };
    class TaskAlreadyActive: public BaseExc {
    public:
        TaskAlreadyActive()
            :BaseExc("Trying to activate an already active task", "Task", "task.cpp")
            {}
    };
    class NoSuchInstr : public BaseExc {
    public:
        NoSuchInstr()
            :BaseExc("Trying to remove a non existent instruction", "Task",
                     "task.cpp") {}
    };
    class TaskNotActive : public BaseExc {
    public:
        TaskNotActive(string msg) :BaseExc(msg, "Task", "task.cpp") {}
    };

    class TaskNotExecuting : public BaseExc {
    public:
        TaskNotExecuting(string msg) :BaseExc(msg, "Task", "task.cpp") {}
    };
  
    class TaskAlreadyExecuting : public BaseExc {
    public:
        TaskAlreadyExecuting()
            :BaseExc("Trying to schedule an already executing task...", "Task",
                     "task.cpp") {}
    };

}

#endif
