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
#ifndef __ABSRESMANAGER_HPP__
#define __ABSRESMANAGER_HPP__

#include <set>

#include <entity.hpp>

#include <resource.hpp>

namespace RTSim {

  class AbsTask;

  /**
     \ingroup util

     Abtract Resource Manager  interface.
     It defines the minimal functionality that should be implemented by 
     a resource manager. A  resource manager is a generic object
     that handles interactions between tasks and Resources.

     @See AbsTask and Resource.
   */
  class AbsResManager {
  protected:
    set<Resource *> _res;
  public:

    AbsResManager() {};

    virtual ~AbsResManager() {};

    /**
     * Adds the resource to the set of resources managed by the 
     * Resource Manager.
     * Should check if the resource is already present in such set
     */
    virtual void addResource(char *name="", int n=1, void* p=NULL);

    /**
     * Function called by a task instr to perform an access 
     * request to a specific resource.
     * That access could be granted  or the task could be suspended.
     * Should check if the resource is among the ones handled by 
     * this manager 
     */
    virtual bool request(AbsTask *t, char *name, int n=1);

    /**
     * Function called by a task instr to perform the release of a 
     * specific resource.
     * The consequence of this call could be the reactivation of one
     * or more suspended tasks.
     * Should check if the resource is among the ones handled by 
     * this manager
     */
    virtual void release(AbsTask *t, char *name, int n=1);

    virtual void addUser(AbsTask *t, char *name, int n=1) = 0;

    // the int parameter will be read by multiple resources managers 
    virtual map<char *,int> * getLResourceNames(void *p = NULL);
    virtual map<char *,int> * getGResourceNames() { return NULL; }

  private:
    virtual bool request(AbsTask *t, Resource *r, int n=1) = 0;
    virtual void release(AbsTask *t, Resource *r, int n=1) = 0;

  };

} // namespace RTSim

#endif
