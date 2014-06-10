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
#include <cmath>

#include <exeinstr.hpp>
#include <load.hpp>

namespace RTSim {

    using namespace MetaSim;

    const char* const RandomTaskSetFactory::Exc::_SET_GEN =
        "The set was already generated.";
    const char* const RandomTaskSetFactory::Exc::_LOAD_GEN =
        "Couldn't generate the loads.";
    const char* const RandomTaskSetFactory::Exc::_UNV_IND =
        "Index out of range";

    const int RandomTaskSetFactory::GENERATE_LIMIT = 100;
    const double RandomTaskSetFactory::NEWTON_PREC = 0.001;
    const double RandomTaskSetFactory::UMIN_DEF = 0.001;

    const Tick IATGen::GCD_DEF = 1;
    const Tick IATGen::TMIN_DEF = 1;
    const Tick CTGen::CMIN_DEF = 1;
    const double UniformIATGen::DELTA_DEF = 0.1;
    const double UniformCTGen::DELTA_DEF = 0.1;
    const Tick SporadicDTGen::DMIN_DEF = 1;
    const double SporadicDTGen::DELTA_DEF = 0.1;

//     long int my_round(double d) 
//     {
//         if ((d - (long int)d) < .5) return (long int)d;
//         else return (long int)d + 1;
//     }

    Tick RandomTaskSetFactory::getMaxCT(int i) const
    {
        chk(i);
        return ctGen->getMax(i);
    }

    Tick RandomTaskSetFactory::getAvgCT(int i) const
    {
        chk(i);
        return ctGen->getAvg(i);
    }

    Tick RandomTaskSetFactory::getMinCT(int i) const
    {
        chk(i);
        return ctGen->getMin(i);
    }

    Tick RandomTaskSetFactory::getMaxIAT(int i) const
    {
        chk(i);
        return iatGen->getMax(i);
    }

    Tick RandomTaskSetFactory::getAvgIAT(int i) const
    {
        chk(i);
        return iatGen->getAvg(i);
    }

    Tick RandomTaskSetFactory::getMinIAT(int i) const
    {
        chk(i);
        return iatGen->getMin(i);
    }

    Tick RandomTaskSetFactory::getOffset(int i) const
    {
        chk(i);
        if (offGen) return offGen->getAvg(i); else return 0;
    }

    Tick RandomTaskSetFactory::getDeadline(int i) const
    { 
        chk(i);
        return dtGen->getAvg(i); 
    }


    // IATGen definition: 
    // the first step (mean value generation) is performed by 
    // IATGen::generate(); specific sub generators will do the second step
    bool IATGen::generate()
    {
        UniformVar r(tMin,tMax);
        for (register int i = 0; i < taskSet->size(); i++) {
            avg[i] = Tick::round(r.get()) * tGCD;
            if (getMin(i) < minT) return false;
        }
        return true;
    }

    // Default behavior is Constant Value!
    Tick IATGen::getAvg(int i) const { return avg[i]; }
    Tick IATGen::getMin(int i) const { return getAvg(i); }
    Tick IATGen::getMax(int i) const { return getAvg(i); }

    // RandomTaskSetFactory <-> IATGen binding
    void IATGen::bind(RandomTaskSetFactory *ts)
    { 
        if (!taskSet) {
            AbstractGen<RandomVar*>::bind(ts);
            for(int i=0;i<taskSet->size();i++)
                va[i]=0;
            avg.resize(ts->size());
        }
    }  

    // RandomTaskSetFactory <-> CTGen binding
    void CTGen::bind(RandomTaskSetFactory *ts, IATGen *i)
    { 
        AbstractGen<RandomVar*>::bind(ts);
        for(int j=0;j<taskSet->size();j++)
            va[j]=0;
        if (!iatGen) {
            iatGen = i;
        }
    }  

    // Default behavior is to add an ExecInstr(va[i]) to the task
    // when the task is deleted, the pseudo-instr gets automagically 
    // deleted by the task destructor. 
    void CTGen::sinthesize(Task *t, int i)
    {
        t->addInstr(new ExecInstr(t, get(i)));
    }

    void CTGen::rebuild()
    {
        cleanVector<RandomVar>(va);
        if (!generate()) {
            cerr << "Errore nella generazione!!" << endl;
            exit(-1);
        }
    }


    Tick CTGen::getAvg(int i) const 
    {
        double d1 = iatGen->getAvg(i);
        double d2 = taskSet->getLoad(i);
        Tick t = Tick::round(d1*d2);
        return t;
    }

    // Default behavior is Constant Value!
    Tick CTGen::getMin(int i) const { return getAvg(i); }
    Tick CTGen::getMax(int i) const { return getAvg(i); }

    // Now let's start with concrete sub-generators:
    bool ConstIATGen::generate() 
    { 
        if (IATGen::generate()) {
            for (register int i = 0; i < taskSet->size(); i++) {
                if(va[i]!=0)
                    delete va[i];
                va[i] = new DeltaVar(getAvg(i));
            }
            return true;
        } 
        return false;
    }

    Tick UniformIATGen::getMin(int i) const 
    {
        double t = getAvg(i);
        t *= (1-delta);
        return Tick(floor(t));
    }

    Tick UniformIATGen::getMax(int i) const 
    {
        double t = getAvg(i);
        t *= (1+delta);
        return Tick(ceil(t));
    }

    bool UniformIATGen::generate()
    {
        if (IATGen::generate()) {
            for (register int i = 0; i < taskSet->size(); i++) {
                if(va[i]!=0)
                    delete va[i];
                va[i] = new UniformVar(getMin(i),getMax(i));
            }
            return true;
        } 
        return false;
    }

    // ... and eventually the concrete CTGen:
    bool ConstCTGen::generate() 
    { 
        for (register int i = 0; i < taskSet->size(); i++) {
            if (getAvg(i) < minC) 
                return false;
            else{
                if(va[i]!=0)
                    delete va[i];
                va[i] = new DeltaVar(getAvg(i)); 
            }
        }
        return true;
    }

    void ConstCTGen::sinthesize(Task *t, int i)
    {
        t->addInstr(new ExecInstr(t, new DeltaVar(* dynamic_cast< DeltaVar* >(get(i)))));
    }

    Tick UniformCTGen::getMin(int i) const 
    {
        double t = getAvg(i);
        t *= (1-delta);
        return Tick(floor(t));
    }

    Tick UniformCTGen::getMax(int i) const 
    {
        double t = getAvg(i);
        t *= (1+delta);
        return Tick(ceil(t));
    }

    bool UniformCTGen::generate() 
    { 
        for (register int i = 0; i < taskSet->size(); i++) 
            if (getMin(i) < minC) return false;
        // If we reach this, then ALL the For all i : getMin(i) >= minC
        // We can build the vector of Random Vars
        for (register int i = 0; i < taskSet->size(); i++){
            if(va[i]!=0)
                delete va[i];
            va[i] = new UniformVar(getMin(i),getMax(i));
        }
        return true;
    }

    UniformCTGen::~UniformCTGen()
    {  for (register int i = 0; i < taskSet->size(); i++){
            va[i] = 0;}
    }


    // RandomTaskSetFactory <-> IATGen binding
    void DTGen::bind(RandomTaskSetFactory *ts, IATGen *i, CTGen *c)
    { 
        AbstractGen<Tick>::bind(ts);

        if (!iatGen) {
            iatGen = i;
        }
        if (!ctGen) {
            ctGen = c;
        }
    }  

    Tick DTGen::getMin(int i) const { return getAvg(i); };
    Tick DTGen::getMax(int i) const { return getAvg(i); };
    Tick DTGen::getAvg(int i) const { return va[i]; };

    bool DlineEquPeriodDTGen::generate()
    {
        for (register int i = 0; i < taskSet->size(); i++)
            va[i] = iatGen->getAvg(i);
        return true;
    }

    bool SporadicDTGen::generate()
    {
        for (register int i = 0; i < taskSet->size(); i++) {
            va[i] = iatGen->getMin(i);
        }
        return true;
    }

    bool RandomDTGen::generate()
    {
        for (register int i = 0; i < taskSet->size(); i++) {
            UniformVar x(_min, _max);
            va[i] = Tick::round(x.get() * double(iatGen->getMin(i)));
            va[i] = max(ctGen->getMax(i), va[i]);
        }
        return true;
    }

    void OffsetGen::bind(RandomTaskSetFactory *ts, IATGen *i)
    { 
        AbstractGen<Tick>::bind(ts);
        if (!iatGen) {
            iatGen = i;
        }
    }  

    Tick OffsetGen::getMin(int i) const { return getAvg(i); };
    Tick OffsetGen::getMax(int i) const { return getAvg(i); };
    Tick OffsetGen::getAvg(int i) const { return va[i]; };

    bool RandomOffsetGen::generate()
    {
        for (register int i = 0; i < taskSet->size(); i++) { 
            int mylimit = (int) (iatGen->getMin(i)/_step);
            if (_islimit) mylimit = min(mylimit, _limit/_step);
            UniformVar r(0,mylimit);
            va[i] = Tick::round(r.get()) * _step;
        }

        return true;
    }


    // RandomTaskSetFactory methods
    // ---------------
    // Generate a normalized load vector: when the function returns true
    // this means that:
    // 1) for all i : load[i] >= minU
    // 2) Sum{load[i]} = u
    bool RandomTaskSetFactory::generateLoad() 
    {
        UniformVar r(0,1);
        register int i;
        double tot = 0.0;

        load.clear();
        if(!_preciseU){
            for (i = 0; i < _size; i++) {
                double l = r.get(); 
                load.push_back(l);
                tot += l;
            }
            for (i = 0; i < _size; i++) {
                load[i] = (load[i] / tot) * _u;
                if (load[i] < _minU) return false;
            }
        }
        else{
            for(i = 0; i <= _size - 2; i++){
                double l = r.get();
                load.push_back(newton(i + 1, l));
            }
            tot = load[0];
            load[0] *= _u;
            for(i = 1; i <= _size - 2; i++){
                load[i] *= (1 - tot);
                tot += load[i];
                load[i] *= _u;
                if(load[i] < _minU) return false;
            }
            if((1 - tot) * _u < _minU) return false;
            load.push_back((1 - tot) * _u);
        }
        dtGen->generate();
        if (offGen) offGen->generate();

        return true;
    }

    // Main generation step; it is iterated until it produces the desired load
    bool RandomTaskSetFactory::generate()
    {
        bool isOk;
        do {
            // We may try this until we get a suitable load or we
            // exceed the maximum number of trials!
            isOk = generateLoad();
            if (isOk) {
                isOk = iatGen->generate();
                if (isOk) {
#ifdef __DEBUG__
                    cout << "calling ctGen::generate trial=" << _count << endl;    
#endif
                    isOk = ctGen->generate();
                } else _count++;
            } else _count++;
        } while (!isOk && _count <= GENERATE_LIMIT);
        return isOk;
    }

    // This function is essentially a virtual task constructor
    // it is used in the main build() to create each task
    // If we use a different task model, this is the function to modify!
    Task *RandomTaskSetFactory::sinthesize(int i)
    {
        Task *t;
        if (offGen) 
            t = new Task(iatGen->get(i),dtGen->get(i),offGen->get(i));
        else 
            t = new Task(iatGen->get(i),dtGen->get(i));
        
        t->setAbort(false);
        
        ctGen->sinthesize(t,i);

//         = new Task(iatGen->get(i));
//         ctGen->sinthesize(t,i);
        return t;
    }

    // Main construction loop!
    void RandomTaskSetFactory::sinthesize()
    {
        // Now build the task set!
        for (register int i = 0; i < _size; i++) {
            Task *t = sinthesize(i);
            tasks.push_back(t);
        }
    }


    /*----------------------------------------------------------------------------*/
    // constructors

    RandomTaskSetFactory::RandomTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
                                               DTGen *g3, double minU, 
                                               bool preciseU)
        throw (Exc) : 
        _count(0),
        _size(n),
        _u(u),
        _minU(minU),
        _preciseU(preciseU),
        tasks(), 
        load(),  
        iatGen(g1),
        ctGen(g2),
        dtGen(g3),
        offGen(0)
    {
        load.reserve(_size);
        tasks.reserve(_size);
        iatGen->bind(this);
        ctGen->bind(this,iatGen);        
        dtGen->bind(this,g1,g2);
        if (generate()) sinthesize();
        else throw Exc(Exc::_SET_GEN);
    }


    RandomTaskSetFactory::RandomTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
                                               DTGen *g3, OffsetGen *g4, 
                                               double minU, bool preciseU)
        throw (Exc) : 
        _count(0),
        _size(n),
        _u(u),
        _minU(minU),
        _preciseU(preciseU),
        tasks(), 
        load(),  
        iatGen(g1),
        ctGen(g2),
        dtGen(g3),
        offGen(g4)
    {
        load.reserve(_size);
        tasks.reserve(_size);
        iatGen->bind(this);
        ctGen->bind(this,iatGen);        
        
        dtGen->bind(this,g1,g2);
        offGen->bind(this,g1);
        if (generate()) sinthesize();
        else throw Exc(Exc::_SET_GEN);
    }
    


    RandomTaskSetFactory::~RandomTaskSetFactory()
    {
        cleanVector<Task>(tasks);
        delete iatGen;
        delete ctGen;
        delete dtGen;
        delete offGen;
    }

//     RandomTaskSetFactory::RandomTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
//                                                double minU, bool doIt, bool preciseU) 
//         throw(Exc) :
//         _count(0),
//         _size(n),
//         _u(u),
//         _minU(minU),
//         _preciseU(preciseU),
//         tasks(), 
//         load(),  
//         iatGen(g1),
//         ctGen(g2)
//     {
//         // Setup the arrays
//         load.reserve(_size);
//         tasks.reserve(_size);
//         iatGen->bind(this);
//         ctGen->bind(this,iatGen);
//         if (doIt) {
//             if (generate()) sinthesize();
//             else throw Exc(Exc::_SET_GEN);
//         }
//     }

    double RandomTaskSetFactory::newton(int i, double l)
    {
        double x, x1 = 0.0;
        do {
            x = x1;
            double p=pow(1 - x, _size - i - 1);
            x1 = x - (1 - p * (1 - x) - l) / ((_size - i) * p);
        } while(x1 - x > NEWTON_PREC / _size);
        return x1;
    }

    void RandomTaskSetFactory::renormalize(double newload)
    {
        double tot;
        int i;

        // First, we delete all the pseudo-instructions
        for (i = 0; i < _size; i++) {
            tasks[i]->discardInstrs(true);
        }

        // compute the old load
        tot = getLoad();

        // renormalize load
        for (i = 0; i < _size; i++)
            load[i] = (load[i] / tot) * newload;
  
        _u = newload;

        // rebuild the vars...
        ctGen->rebuild();
  
        for (i = 0; i < _size; i++)
            ctGen->sinthesize(tasks[i], i);
    }


    double RandomTaskSetFactory::getAvgLoad(int i)  const
    {
        chk(i); 
        double u = double(ctGen->getAvg(i))/double(iatGen->getAvg(i)); 
        return u;
    }

    double RandomTaskSetFactory::getAvgLoad()  const
    {
        double r = 0.0;
        for (register int i = 0; i < _size; i++) 
            r += getAvgLoad(i);    
        return r;
    }

    double RandomTaskSetFactory::getMaxLoad(int i)  const
    { 
        chk(i); 
        double u = double(ctGen->getMax(i))/double(iatGen->getMin(i)); 
        return u;
    }

    double RandomTaskSetFactory::getMaxLoad()  const
    {
        double r = 0.0;
        for (register int i = 0; i < _size; i++) 
            r += getMaxLoad(i);
        return r;
    }

    double RandomTaskSetFactory::getMinLoad(int i)  const
    { 
        chk(i);
        double u = double(ctGen->getMin(i))/double(iatGen->getMax(i)); 
        return u;
    }

    double RandomTaskSetFactory::getMinLoad()  const
    {
        double r = 0.0;
        for (register int i = 0; i < _size; i++) 
            r += getMinLoad(i);
        return r;
    }

//     RandomTaskSetFactory::~RandomTaskSetFactory() 
//     { 
//         // First, we delete all the pseudo-instructions
//         // it is not necessary anymore, as the instructions are deleted 
//         // by the task.

//         //for (register unsigned int i = 0; i < tasks.size(); i++) {
//         //  tasks[i]->discardInstrs(true);
//         //}

//         // Then we delete each single task we created!
//         cleanVector<Task>(tasks);
//         delete iatGen;
//         delete ctGen;
//         //dbg << "~RandomTaskSetFactory() has completed!" << endl;
//     }

    void RandomTaskSetFactory::print(int i)
    {
        cout << "task[" << i << "]" << endl;
        cout << "U   = "  
             << getMinLoad(i) << "/" 
             << getAvgLoad(i) << "/" 
             << getMaxLoad(i) << "/" << getLoad(i) << endl;
        cout << "C   = " << getMinCT(i) << "/"
             << getAvgCT(i) << "/" << getMaxCT(i) << endl;
        cout << "IAT = " << getMinIAT(i) << "/"
             << getAvgIAT(i) << "/" << getMaxIAT(i) << endl;

    }

    void RandomTaskSetFactory::print()
    {
        //dbg.enable("__tset__");
        //dbg.enter("__tset__");
        cout << "Task Set" << endl;
        for (register int i = 0; i < _size; i++) {
            print(i);
            cout  << "------------------------------------------------" 
                  << endl;
        }
  
        cout << "Real U = " << getMinLoad() << "/" << getAvgLoad() << "/" 
             << getMaxLoad() << endl;
        cout << "Wanted U = " << getLoad() << endl << endl;
  
        //dbg.exit();
        //dbg.disable("__tset__");
    }

}
