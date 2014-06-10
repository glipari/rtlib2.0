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
#ifndef __JTRACE_HPP__
#define __JTRACE_HPP__
 
#include <fstream>
#include <iostream>
#include <string>
 
#include <baseexc.hpp>
#include <basetype.hpp>
#include <event.hpp>
#include <trace.hpp>

#include <traceevent.hpp>
 
#define _JTRACE_DBG_LEV "JavaTracer"

namespace RTSim {

  using namespace std;
  using namespace MetaSim;

  /* 
     \ingroup util

     The Trace Class!!
     This is really a basic one, more complicated traces may be generated
     if it is necessary; the trace file is coded so that it can be directly
     fed into a Java interface, with no problems deriving from the
     Big/Little endian issue
   */
  class JavaTrace: public virtual Trace {
  public:
    typedef enum {TRACE_UNKNOWN_ENDIAN,
		  TRACE_BIG_ENDIAN,
		  TRACE_LITTLE_ENDIAN} TRACE_ENDIANESS;
 
    static TRACE_ENDIANESS endianess;
    static void probeEndianess(void);

    static string version;
 
  protected:
    vector<TraceEvent*> data;

    // Output file
    int filenum;

    // The number of the traced events
    unsigned long int counter, fileLimit;

    vector<int> taskList;

  public:
    JavaTrace(const char *name, bool tof = true,
	      unsigned long int limit = 1000000);
    virtual ~JavaTrace();

    virtual void close();

    vector<TraceEvent*> getData() {return data;}

    // The Little/Big Endian coding functions!
    virtual void record(Event *e);
  };

} // namespace RTSim  

#endif                    
