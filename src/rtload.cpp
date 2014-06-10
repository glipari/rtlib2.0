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
#include <simul.hpp>

#include <rtload.hpp>

namespace RTSim {

  using namespace MetaSim;

  const Tick SporadicDTGen::DMIN_DEF = 1;
  const double SporadicDTGen::DELTA_DEF = 0.1;

  extern long int my_round(double);

  /* The Real-Time task set:
   * It needs another sub-generator to define the deadlines
   */

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
      va[i] = my_round(x.get() * iatGen->getMin(i));
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
      va[i] = my_round(r.get()) * _step;
    }

    return true;
  }

  RandomTaskSetFactory::RandomRTTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
						 DTGen *g3, double minU, bool doIt, bool preciseU)
    throw (Exc) : 
    RandomTaskSetFactory(n,u,g1,g2,minU,false,preciseU),
    dtGen(g3),
    offGen(0)
  {
    dtGen->bind(this,g1,g2);
    if (doIt) {
      if (generate()) sinthesize();
      else throw Exc(Exc::_SET_GEN);
    }  
  }

  RandomTaskSetFactory::RandomRTTaskSetFactory(int n, double u, IATGen *g1, CTGen *g2, 
						 DTGen *g3, OffsetGen *g4, double minU, bool doIt, bool preciseU)
    throw (Exc) : 
    RandomTaskSetFactory(n,u,g1,g2,minU,false,preciseU),
    dtGen(g3),
    offGen(g4)
  {
    dtGen->bind(this,g1,g2);
    offGen->bind(this,g1);
    if (doIt) {
      if (generate()) sinthesize();
      else throw Exc(Exc::_SET_GEN);
    }  
  }



  RandomTaskSetFactory::~RandomRTTaskSetFactory()
  {
    delete dtGen;
    if(offGen)
      delete offGen;
  }

  bool RandomTaskSetFactory::generate()
  {
    bool isOk;
    do {
      isOk = RandomTaskSetFactory::generate();
      if (isOk) {
	isOk = dtGen->generate();
      } else if (_count <= GENERATE_LIMIT) _count++;
    } while(!isOk && _count <= GENERATE_LIMIT);

    if (offGen) do {
      if (isOk) {
	isOk = offGen->generate();
      } else if (_count <= GENERATE_LIMIT) _count++;
    } while(!isOk && _count <= GENERATE_LIMIT);
  
    return isOk;
  }

  Task *RandomTaskSetFactory::sinthesize(int i)
  {
    Task *t;
    if (offGen) 
      t = new Task(iatGen->get(i),dtGen->get(i),offGen->get(i));
    else 
      t = new Task(iatGen->get(i),dtGen->get(i));

    t->setAbort(false);

    ctGen->sinthesize(t,i);
    return t;
  }

  void RandomTaskSetFactory::print(int i)
  {
    RandomTaskSetFactory::print(i);
    cout << "D   = " << dtGen->getAvg(i) << endl;
    if (offGen) cout << "O   = " << offGen->get(i) << endl;

  }

}
