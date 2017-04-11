#include "catch.hpp"
#include <rttask.hpp>
#include <pwcet.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>

using namespace MetaSim;
using namespace RTSim;

TEST_CASE("PWCET 1", "[pwcet, test1]")
{
    EDFScheduler sched;
    RTKernel kern(&sched);

    NPReclaimingServer server("nps", true);

    PeriodicTask t1(12, 12, 0, "TaskA");
    t1.insertCode("raise_thres(1);pwcet(nps, 5);lower_thres(1);");

    PeriodicTask t2(15, 15, 0, "TaskB");
    t2.insertCode("raise_thres(1);pwcet(nps, 6);lower_thres(1);");

    REQUIRE(server.get_budget(&t1) == 5);
    REQUIRE(server.get_budget(&t2) == 6);
}
