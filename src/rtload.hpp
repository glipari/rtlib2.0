/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __RTLOAD_HPP__
#define __RTLOAD_HPP__

#include <load.hpp>
#include <rttask.hpp>

namespace RTSim {

  //const Tick SporadicDTGen::DMIN_DEF = 1;
  //const double SporadicDTGen::DELTA_DEF = 0.1;


  /**
     \ingroup util

     This is the abstract class for generating the relative deadline.
     The other concrete generators are derived from here. It defines
     a standard bind() function, the getMin, getAvg and getMax values.
  */
  class DTGen : public AbstractGen<Tick> {
  protected:
    IATGen *iatGen;
    CTGen *ctGen;
  public:
    DTGen() : iatGen(0), ctGen(0) {};
    virtual ~DTGen() {};
    void bind(RandomTaskSetFactory *ts, IATGen *i, CTGen *c);
    virtual Tick getMin(int i) const; 
    virtual Tick getAvg(int i) const; 
    virtual Tick getMax(int i) const; 
  };


  /**
     \ingroup util

     This class assigns deadlines equal to the period. To this end, it 
     reads the period value from the IATGen, by calling the IATGen::getAvg().
   */
  class DlineEquPeriodDTGen : public DTGen {
  public:
    DlineEquPeriodDTGen() : DTGen() {};
    virtual ~DlineEquPeriodDTGen() {};
    virtual bool generate();
  };


  /**
     \ingroup util

     This class sets the deadline equal to the minimum interarrival time,
     and it is useful for sporadic tasks. It calls the IAT::getMin();
   */
  class SporadicDTGen : public DTGen {
    Tick minD;
    double delta;
  public:
    static const Tick DMIN_DEF;
    static const double DELTA_DEF;
    SporadicDTGen(Tick d = DMIN_DEF, double dl = DELTA_DEF) : 
      DTGen(), minD(d),delta(dl) {};
    virtual ~SporadicDTGen() {};
    virtual bool generate();
  };

  /**
     \ingroup util
     This class sets the relative deadline with an uniform distribution. 
     The user selects a number x between 0 and 1: the relative deadline is 
     set equal to a random number uniformuely distributed between [xp,p], where
     p is the minimum interarrival time.
   */
  class RandomDTGen : public DTGen {
    double _min;
    double _max;
  public:
    RandomDTGen(double x, double y=1) : DTGen(), _min(x), _max(y) {};
    virtual ~RandomDTGen() {};
    virtual bool generate();
  };

  /**
     \ingroup util

     This is used for generating the task offset. It is an abstract class,
     the offset generators are derived from this.
   */
  class OffsetGen : public AbstractGen<Tick> {
  protected:
    IATGen *iatGen;
  public:
    OffsetGen() : iatGen(0) {};
    virtual ~OffsetGen() {};
    void bind(RandomTaskSetFactory *ts, IATGen *i);
    virtual Tick getMin(int i) const; 
    virtual Tick getAvg(int i) const; 
    virtual Tick getMax(int i) const; 
  };

  /**
     \ingroup util
     
     Generates an offset at random between 0 and the task period. For
     doing this, it reads the IAT::getAvg().
   */
  class RandomOffsetGen : public OffsetGen {
    int _step;
    int _limit;
    bool _islimit;
  public:
    RandomOffsetGen(int step=1, bool islimit=false, int limit=1) : 
      OffsetGen(), _step(step), _limit(limit), _islimit(islimit) {};
    virtual ~RandomOffsetGen() {};
    virtual bool generate();
  };


  /**
     The RT Task Set factory. 
     For a Real-Time Task Set we need to update/modify the
     main generation loop, because we need to generate ALSO
     the deadline and the virtual task constructor
  
     It has two constructors: one for tasks without offset (i.e. with
     offset 0), and the other for task with offset. In the first case,
     in addition to the computation time generator and the interarrival
     time generator, the user must specify the relative deadline
     generator. In the second case, the user must also specify the
     offset generator.

     @todo It should become a factory only!!  The real task set should
     be in another class that is used for other purpouses. In this
     way, I could build other kinds of factories.
   */
  class RandomRTTaskSetFactory : public RandomTaskSetFactory {
  protected:
    DTGen *dtGen;
    OffsetGen *offGen;
  public:
    /**
       Constructor. It takes several arguments:
       
       @param n   number of tasks
       @param u   desired utilization
       @param g1  interarrival time generator (see before)
       @param g2  computation time generator
       @param g3  relative deadline generator
       @param minU minumum utilizazione for a task
       @param doIt if true it creates the tasks. If false, tasks 
                   must be created with a call to synthesize.
       @param preciseU if true, the U is adjusted, so that the final 
                       sum is precisely the desired one. If false, 
                       no adjustment is done. By default, it is false.
     */
    RandomRTTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
			   DTGen *g3, double minU = UMIN_DEF, bool doIt = true, bool preciseU = false) throw (Exc);

    RandomRTTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
			   DTGen *g3, OffsetGen *g4, double minU = UMIN_DEF, 
			   bool doit = true, bool preciseU = false) throw (Exc);

    virtual ~RandomRTTaskSetFactory();
    RTTask *getTask(int i) { chk(i); return (RTTask*)(tasks[i]); };
    virtual bool generate();
    virtual Task *sinthesize(int i);
    virtual void print(int i);

    Tick getOffset(int i) const
    {
      if (offGen) return offGen->getAvg(i); else return 0;
    }
    Tick getDeadline(int i) const
    { 
      return dtGen->getAvg(i); 
    }

    // Oddities: these functions are virtual & overloaded: the compiler
    // messes up and wants them redefined even if they are unmodified
    virtual void print() { RandomTaskSetFactory::print(); };
    virtual void sinthesize() { RandomTaskSetFactory::sinthesize(); };
  };
} // namespace RTSim 

#endif
