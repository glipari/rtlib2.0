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
/*
 * $Id: resource.hpp,v 1.4 2005/04/28 01:34:47 cesare Exp $
 *
 * $Log: resource.hpp,v $
 * Revision 1.4  2005/04/28 01:34:47  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.3  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#ifndef __RESOURCE_HPP__
#define __RESOURCE_HPP__

#include <string>

#include <simul.hpp>

namespace RTSim {

    using namespace MetaSim;

    class AbsRTTask;

    /** 
        \ingroup resman

        Very simple class that models a resource shared by two or more
        tasks. Every task should use this resource only through a critical
        section surrounded by wait and signal instructions.
    
        @see ResManager, WaitInstr, SignalInstr
    */
    class Resource: public Entity
    {
    protected:
        int _total;
        int _available;

        AbsRTTask* _owner;

    public:
  
        /// simple constructor
        Resource(const string& n, int nr = 1);
        
        /// copy constructor
        Resource(const Resource &r);

        /// lock the resource
        void lock(AbsRTTask *owner, int n = 1);

        /// unlock the resource
        void unlock(int n = 1);

        /// checks if the resource is locked
        bool isLocked() const;
        
        /// returns how many resource units are available
        int available() const;

        /// maximum number of resource units
        int total() const;

        /// returns the resource owner
        AbsRTTask* getOwner() const;

        void newRun();
        void endRun();

    };

} // namespace RTSim

#endif
