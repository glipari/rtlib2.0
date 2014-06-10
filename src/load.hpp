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
#ifndef __LOAD_HPP__
#define __LOAD_HPP__

#include <cstdlib>
#include <vector>

#include <randomvar.hpp>
#include <task.hpp>

namespace RTSim {

    /** 
        \ingroup util

        I slightly restructured Peppe's design trying to make it more modular
        and easy to extend:
        Let me first introduce some issues:
        -# The RandomTaskSetFactory is (from Design Pattern Perspective)
        a sort of Abstract Factory pattern; its aim is that of
        generating a bunch of task given some parameters, such as the
        desired load, the task number, the minimum & maximum values for
        interarrival times;
        -# The RandomTaskSetFactory needs to be fairly general, so to allow 
        the generation of different task sets; namely, we would like to set 
        individual aspects of the set, such as: fixed/random interarrival time,
        fixed/random computation time, different computation structures
        (i.e. with/without resources, with/without message sending,...),
        deadline equal to the period or random.

        It goes without saying that we can only obtain this degree of
        flexibility thorugh composition/delegation. Peppe used a global
        RandomTaskSetFactory class which acts as Abstract Factory, generates 
        the U_i (load) for each single task, and then it relies on some 
        sub-generators to build the specific task parameter, respecting the 
        load specs
      
        Peppe technique used delegation, expressed using multiple
        inheritance (MI) and templates to build different task set; this
        produces different interfaces for the sub-generators, which are
        eventually merged using MI.
      
        I tried to use a different approach, based on virtualization and
        containment: the basic difference relys in this: Instead of using

        \code
        RandomRTTaskSetFactory<ConstCompGen, ConstArrGen, SporadicDlineGen> ts();
      
        ts.setMinT(t_min);
        ts.setMinD(d_min);
        ts.generate(N,U,T_min,T_max);
        \endcode
      
        We will use this:
      
        \code
        RandomRTTaskSetFactory ts(N,U,new ConstCTGen, 
        new ConstIATGen(t_min,t_max,delta),
        new DlineEquPeriodDTGen(d_min));
        \endcode
    
        The task set may be immediately used.  The advantages in doing so
        is that we can hide the interface of the sub-generators;
        sub-generator specific parameters are declared in their
        constructors; sub-generators are deleted on RandomTaskSetFactory
        destruction. We don't need to raise an exception if one is misusing
        the RandomTaskSetFactory, as in Peppe system, because it's
        impossible to do so!
      
        To propagate some information from the RandomTaskSetFactory class
        to the sub-generators, a bind() method is needed; we can't bind
        anything during construction because sub-generators have to be
        created BEFORE the RandomTaskSetFactory is created; the
        relationship between the sub-generators (contained) and the
        RandomTaskSetFactory (container); so, during construction of
        RandomTaskSetFactory, bind() is called on the sub-generator objects
      
        As a first step, I was still using templates + containment, but
        this solution doesn't give the expected results; using templates
        allows to specify at object construction what kind of sub-generator
        to use not the parameters for them; sub-generators specific
        interface have to be exposed again!
      
        Another big modification with respect Peppe implementation is the
        separation of the data generation process from the task synthesis;
        we use the generateLoad() as first step, to generate task
        individual loads; then generateTasks() is reiterated (up to
        GENERATE_LIMIT) to obtain the task parameters; eventually task are
        sinthesized using the syntethize(int i) virtual Task constructors.
        This cope with different Task Models Instructions are added as a
        part of the synthsis phase, by the CT object which exactly knows
        what to do with the generated computation time
      
        @todo save/load a TaskSet (Persistence) 
    */

    class IATGen;
    class CTGen;
    class DTGen;
    class OffsetGen;

    /** 
        \ingroup util
      
        The main RandomTaskSetFactory class. This is used for generating
        random task sets, by usign the previuosly defined generators
    */
    class RandomTaskSetFactory {
    public:
        class Exc : public BaseExc {    
        public :
            static const char* const _SET_GEN;
            static const char* const _LOAD_GEN;
            static const char* const _UNV_IND;
            Exc(string ms, 
                string cl = "RandomTaskSetFactory", 
                string md = "load.hpp") :
                BaseExc(ms, cl, md) {}
        };
    protected:
        static const int GENERATE_LIMIT;
        static const double NEWTON_PREC;
        int    _count;      // Generation trials
        int    _size;       // Task set size
        double _u;          // The desired load of the task set
        double _minU;       // Minimul load for each single tas
        bool _preciseU;     // Alternate load generation
        // Set implemented as a vector
        vector <Task *> tasks;
        vector <double> load;

        // Sub-generators handle
        IATGen *iatGen;
        CTGen *ctGen;
        DTGen *dtGen;
        OffsetGen *offGen;

        // Helpers
        bool normalizeLoad();
        bool generateLoad();
        void chk(int i) const throw(Exc)
            {
#ifdef __DEBUG__
                if (i < 0 || i > _size) throw Exc(Exc::_UNV_IND);
#endif
            }

        // helper function
        double newton(int i, double l);

        // This is the main interface:
        // generate() is used in a loop to generate the task set
        virtual bool generate();
        // syntethize(int i) is the virtual task constructor
        virtual Task *sinthesize(int i);  
        virtual void sinthesize();
    public:
        static const double UMIN_DEF;

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
        RandomTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
                             DTGen *g3, double minU = UMIN_DEF,
                             bool preciseU = false) throw (Exc);
        
        RandomTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
                             DTGen *g3, OffsetGen *g4, 
                             double minU = UMIN_DEF, bool preciseU = false) throw (Exc);

        virtual ~RandomTaskSetFactory();
  
        // Accessors
        int size() const { return _size; }
        Task *getTask(int i) const { chk(i);  return tasks[i]; }

        void renormalize(double newload);

        double getAvgLoad(int i) const;
        double getAvgLoad() const;
        double getMinLoad(int i) const;
        double getMinLoad() const;
        double getMaxLoad(int i) const;
        double getMaxLoad() const;
        double getLoad(int i) const { return load[i]; }
        double getLoad() const { return _u; }

        Tick getMaxCT(int i) const;
        Tick getAvgCT(int i) const;
        Tick getMinCT(int i) const;
        Tick getMaxIAT(int i) const;
        Tick getAvgIAT(int i) const;
        Tick getMinIAT(int i) const;
        // from old rtload.hpp
        Tick getOffset(int i) const;
        Tick getDeadline(int i) const;

        // Debugging function
        virtual void print();
        virtual void print(int i);
    };

    /**
       \ingroup util
       
       The Generator Interface provides a vector-like interface with each
       element generated according to a given paradigm; the actual
       paradigm is specified by subclassing I didn't provide any
       protection against human errors, since these class are always used
       as components for the RandomTaskSetFactory; all checks are left to
       the container class
    */

    /** Helper function for clearing a vector of objects */
    template<class R>
    inline void cleanVector(vector<R> &v) { v.clear(); }

    /** 
        \ingroup util
        
        Helper function for clearing a vector of POINTERS to objects 
    */
    template<class R>
    inline void cleanVector(vector<R *> &v)
    {
        while (!v.empty()) {    
            R *r = v.back();
            v.pop_back();
            if(r!=0)
                delete r;
        }
    }

    /**
       \ingroup util

       The generator main class, from which all generators are derived.
    */
    template <class VA = RandomVar *, class T = Tick>
    class AbstractGen {
    protected:
        vector<VA> va;
        RandomTaskSetFactory *taskSet;
    public:
        AbstractGen() : va(), taskSet(0) {}
        virtual ~AbstractGen() { cleanVector(va); }
        VA get(int i)  { return va[i]; }
        void bind(RandomTaskSetFactory *ts) {
            if (!taskSet) {
                taskSet = ts;
                va.resize(ts->size());
            }
        }
        virtual bool generate() = 0;
        virtual T getAvg(int i) const = 0;
        virtual T getMin(int i) const = 0;
        virtual T getMax(int i) const = 0;
    };

    /** 
        \ingroup util

        The Inter-Arrival Time Sub Generator Model generates a set of random
        variables (which kind is specified by subclassing); the Random Vars
        are generated using a two-steps procedure; first, the mean values
        are generated, sampling a uniform random var distrubuted between [t1,t2]
        Then the actual variables are generated!
        The sub-classes of IATGen defines the actual kind of the random var
        and consequently the getMin() & getMax() functions */
    class IATGen : public AbstractGen<RandomVar*> {
    protected:
        vector<Tick> avg;
        Tick tMin,tMax,tGCD;
        Tick minT;
    public:
        static const Tick GCD_DEF;
        static const Tick TMIN_DEF;

        IATGen(Tick t1, Tick t2, Tick m = TMIN_DEF, Tick gcd = GCD_DEF)
            : avg(), tMin(max(t1,m)), tMax(t2),  
              tGCD(gcd), minT(m) {}
        virtual ~IATGen() {}
        void bind(RandomTaskSetFactory *ts);
        virtual Tick getMin(int i) const;
        virtual Tick getAvg(int i) const;
        virtual Tick getMax(int i) const;
        virtual bool generate();
    };
  
    /** 
        \ingroup util

        The Computation Time Sub Generator Model generates and sinthesize
        accordingly the task behavior (Pseudo Instructions).
        It relies on the existence of an IAT Sub-Generator and
        RandomTaskSetFactory to derive from T_i and U_i the required c_i
        The basic sinthesize() behavior is just to add an ExecInstr(va[i])...
     */
    class CTGen : public AbstractGen<RandomVar*> {
    protected:
        IATGen *iatGen;
        Tick minC;
    public:
        static const Tick CMIN_DEF;

        CTGen(Tick c = CMIN_DEF) : iatGen(0), minC(c) {}
        virtual ~CTGen() {}
        void bind(RandomTaskSetFactory *ts, IATGen *i);
        virtual Tick getMin(int i) const;
        virtual Tick getAvg(int i) const;
        virtual Tick getMax(int i) const;
        virtual void sinthesize(Task *t, int i);
        virtual void rebuild();
    };
  
    /**
       \ingroup util

       This generators produces constant interarrival times, and it is useful
       for generating periodic tasks with random periods.
     */
    class ConstIATGen : public IATGen {
    public:
        /** @param t1 minimum IAT
            @param t2 maximum IAT
            @param m (optional) minimum possible IAT
            @param gcd (optional) Greatest common divisor: By default is 1 */
        ConstIATGen(Tick t1, Tick t2, Tick m = TMIN_DEF,
                    Tick gcd = GCD_DEF) :
            IATGen(t1,t2,m,gcd) {}
        virtual  ~ConstIATGen() {}
        virtual bool generate();
    };

    /**
        \ingroup util

        This generators produces interarrival times according to a uniform
        distribution. The distribution is of the form [x(1-d), x(1+d)]
        where x is a random variable between t1 and t2, and d is the
        percentage of variation. See the constructor for more details.
     */
    class UniformIATGen : public IATGen {
    protected:
        double delta;
    public:
        static const double DELTA_DEF;

        /** @param t1 minimum IAT
            @param t2 maximum IAT
            @param m (optional) minimum possible IAT
            @param gcd (optional) Greatest common divisor: By default is 1
            @param d (optional) the percentage of variation. By defaul, is 0.1 */ 
        UniformIATGen(Tick t1, Tick t2, Tick m = TMIN_DEF,
                      Tick gcd = GCD_DEF,
                      double d = DELTA_DEF) : 
            IATGen(t1,t2,m,gcd), delta(d) {}
        virtual ~UniformIATGen() {}
        virtual bool generate();
        virtual Tick getMin(int i)  const;
        virtual Tick getMax(int i)  const;
    };

    /**
       \ingroup util


       It is similar to the ConstIATGen, but here the distribution is of
       the form [x(1-d), x] where x is a random variable between t1
       and t2, and d is the percentage of variation. See the constructor
       for more details.
     */
    class MaxIATGen : public UniformIATGen {
    public:

        /** @param t1 minimum IAT
            @param t2 maximum IAT
            @param m (optional) minimum possible IAT
            @param gcd (optional) Greatest common divisor: By default is 1
            @param d (optional) the percentage of variation. By defaul, is 0.1 */ 
        MaxIATGen(Tick t1, Tick t2, Tick m = TMIN_DEF,
                  Tick gcd = GCD_DEF,
                  double d = DELTA_DEF) :
            UniformIATGen(t1,t2,m,gcd,d) {}
        virtual Tick getMax(int i) const { return getAvg(i); }
    };

    /**
       \ingroup util

       It is similar to the ConstIATGen, but here the distribution is
       of the form [x, x(1+d)] where x is a random variable between t1
       and t2, and d is the percentage of variation. See the
       constructor for more details.
     */
    class MinIATGen : public UniformIATGen {
    public:
        /** @param t1 minimum IAT
            @param t2 maximum IAT
            @param m (optional) minimum possible IAT
            @param gcd (optional) Greatest common divisor: By default is 1
            @param d (optional) the percentage of variation. By defaul, is 0.1 */ 
        MinIATGen(Tick t1, Tick t2, Tick m = TMIN_DEF,
                  Tick gcd = GCD_DEF,
                  double d = DELTA_DEF) :
            UniformIATGen(t1,t2,m,gcd,d) {}
        virtual Tick getMin(int i) const { return getAvg(i); }
    };

    /** Generates constant computation times */
    class ConstCTGen : public CTGen {
    public:  
        ConstCTGen(Tick c = CMIN_DEF) : CTGen(c) {}
        virtual bool generate();
        virtual void sinthesize(Task *t, int i);
    };

    /** 
        \ingroup util

        Generates variable computation times, uniformely distributed
     * between [c(1-d), c(1+d)].
     */
    class UniformCTGen : public CTGen {
    protected:
        double delta;
    public:
        static const double DELTA_DEF;

        /** The computation times are variable, within [max(cmin, c*(1-d), c*(1+d)]
         *  and c is computed from the interarrival times.
         *  @param cmin (optional) minimum possible computation time
         *  by default is 1
         *  @param d (optional) the percentage of variation. By default, is 0.1 
         */ 
        UniformCTGen(Tick cmin = CMIN_DEF, double d = DELTA_DEF) :
            CTGen(cmin), delta(d) {}
        virtual ~UniformCTGen();
        virtual bool generate();
        virtual Tick getMin(int i) const;
        virtual Tick getMax(int i) const;
    };

    /** 
        \ingroup util

        Generates variable computation times, uniformely distributed
     *  between [c(1-d), c].
     */
    class MaxCTGen : public UniformCTGen {
    public:  
        /** @param c (optional) minimum possible computation time, by default is 1
         *  @param d (optional) the percentage of variation. By default, is 0.1
         */ 
        MaxCTGen(Tick c = CMIN_DEF, double d = DELTA_DEF) :
            UniformCTGen(c,d) {}
        virtual Tick getMax(int i) const { return getAvg(i); }
    };

    /** 
        \ingroup util

        Generates variable computation times, uniformely distributed
     *  between [c, c(1+d)].
     */
    class MinCTGen : public UniformCTGen {
    public:  
        /** @param c (optional) minimum possible computation time, by default is 1
         *  @param d (optional) the percentage of variation. By default, is 0.1
         */ 
        MinCTGen(Tick c = CMIN_DEF, double d = DELTA_DEF) :
            UniformCTGen(c,d) {}
        virtual Tick getMin(int i) const { return getAvg(i); }
    };


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
        DTGen() : iatGen(0), ctGen(0) {}
        virtual ~DTGen() {}
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
        DlineEquPeriodDTGen() : DTGen() {}
        virtual ~DlineEquPeriodDTGen() {}
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
            DTGen(), minD(d),delta(dl) {}
        virtual ~SporadicDTGen() {}
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
        RandomDTGen(double x, double y=1) : DTGen(), _min(x), _max(y) {}
        virtual ~RandomDTGen() {}
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
        OffsetGen() : iatGen(0) {}
        virtual ~OffsetGen() {}
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
            OffsetGen(), _step(step), _limit(limit), _islimit(islimit) {}
        virtual ~RandomOffsetGen() {}
        virtual bool generate();
    };

} // namespace RTsim 

#endif
