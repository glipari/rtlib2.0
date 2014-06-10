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
#ifndef __FCFSRESMANAGER_HPP__
#define __FCFSRESMANAGER_HPP__

#include <deque>
#include <map>

#include <resmanager.hpp>

#define _FCFS_RES_MAN_DBG_LEV "FCFSResManager"

namespace RTSim {

    class AbsRTTask;
    class Task;

    /**
       \ingroup resman
       Simple Resource manager which implements a FCFS strategy 
       for a single resource
       @ see Resource 
    */
    class FCFSResManager : public ResManager {
    public:
        /** Constructor of FCFSResManager
         * 
         * @param n is the resource manager name
         */
        FCFSResManager(const string &n = "");
 
        virtual void newRun();
        virtual void endRun();
 
    protected:
        virtual bool request(AbsRTTask*, Resource*, int n=1);
        virtual void release(AbsRTTask*, Resource*, int n=1); 
    private:  
        map<Resource *, AbsRTTask *> _resAndCurrUsers;
        typedef deque<AbsRTTask *> BLOCKED_QUEUE;
        map<Resource *, BLOCKED_QUEUE> _blocked;
    };

} // namespace RTSim 

#endif
