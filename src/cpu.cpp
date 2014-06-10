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
#include <cpu.hpp>

namespace RTSim {
  
    CPU::CPU(const std::string &name): Entity(name), frequencySwitching(0),
                                       index(0)
    {
        cpuName = name;
        PowerSaving = false;
    }
  
  
    CPU::CPU(const std::string &name, int num_levels, double V[], int F[]) : 
        Entity(name), frequencySwitching(0), index(0)
    {
        cpuName = name;
    
        // Setting voltages and frequencies
        for (int i = 0; i < num_levels; i ++) {
            cpulevel cl;
            cl.voltage = V[i];
            cl.frequency = F[i];
            steps.push_back(cl);
        }
    
        // Setting speeds (basing upon frequencies)
        for (vector<cpulevel>::iterator iter = steps.begin(); 
             iter != steps.end(); iter++)
            (*iter).speed = ((double) (*iter).frequency) / 
                ((double)F[num_levels -1]);
    
        currentLevel = num_levels - 1;
        PowerSaving = true;
    }
  
  
    CPU::~CPU()
    {
        steps.clear();
    }
  
  
    int CPU::getCurrentLevel()
    {
        if (PowerSaving) 
            return currentLevel; 
        else 
            return 0;
    }
  
    double CPU::getMaxPowerConsumption()
    {
        int numlevels = steps.size();
        if (PowerSaving) 
            return (steps[numlevels-1].frequency)*(steps[numlevels-1].voltage)*(steps[numlevels-1].voltage);
        else
            return 0;
    } 
  
    double CPU::getCurrentPowerConsumption()
    {
        if (PowerSaving) 
            return (steps[currentLevel].frequency)*(steps[currentLevel].voltage)*(steps[currentLevel].voltage);
        else
            return 0;
    }
  
    double CPU::getCurrentPowerSaving()
    {
        if (PowerSaving) { 
            long double maxPowerConsumption = getMaxPowerConsumption(); 
            long double saved = maxPowerConsumption - getCurrentPowerConsumption();
            return (double) (saved/maxPowerConsumption);
        }
        else
            return 0;   
    }
  
  
    double CPU::setSpeed(double newLoad)
    {
        DBGENTER(_KERNEL_DBG_LEV);
        DBGPRINT("pwr: setting speed in CPU::setSpeed()");
        DBGPRINT("pwr: New load is " << newLoad);
        if (PowerSaving) { 
            DBGPRINT("pwr: PowerSaving=on");
            DBGPRINT("pwr: currentLevel=" << currentLevel);
            for (int i=0; i < (int) steps.size(); i++) 
                if (steps[i].speed >= newLoad) {
                    if (i != currentLevel) 
                        frequencySwitching++;
                    currentLevel = i;
                    DBGPRINT("pwr: New Level=" << currentLevel <<" New Speed=" << steps[currentLevel].speed);
                    
                    return steps[i].speed; //It returns the new speed
                }
        }
        else 
            DBGPRINT("pwr: PowerSaving=off => Can't set a new speed!");
        
        return 1; // An error occurred or PowerSaving is not enabled
    }
  
  
    double CPU::getSpeed() 
    {
        if (PowerSaving)  
            return steps[currentLevel].speed;
        else
            return 1;
    }
  
  
    double CPU::getSpeed (int level)
    {
        int numlevels = steps.size();
        if ( (!PowerSaving) || (level > (numlevels - 1)) )  
            return 1;
        else
            return steps[level].speed;
    }
  
    unsigned long int CPU::getFrequencySwitching() 
    {
        DBGENTER(_KERNEL_DBG_LEV);
        DBGPRINT("frequencySwitching=" << frequencySwitching);
        	   
        return frequencySwitching;
    }
  
  
  
  
    void CPU::check(){
        cout << "Checking CPU:" << cpuName << endl;;
        cout << "Max Power Consumption is :" << getMaxPowerConsumption() << endl;
        for (vector<cpulevel>::iterator iter = steps.begin(); iter != steps.end(); iter++){
            cout << "-Level-" << endl;
            cout << "\tFrequency:" << (*iter).frequency << endl;
            cout << "\tVoltage:" << (*iter).voltage << endl;
            cout << "\tSpeed:" << (*iter).speed << endl;
        }
        for (int i=0; i < (int) steps.size(); i++)
            cout << "Speed level" << getSpeed(i) << endl;
        for (vector<cpulevel>::iterator iter = steps.begin(); iter != steps.end(); iter++){
            cout << "Setting speed to " << (*iter).speed << endl;
            setSpeed((*iter).speed);
            cout << "New speed is  " << getSpeed() << endl;
            cout << "Current level is  " << getCurrentLevel() << endl;
            cout << "Current Power Consumption is  " << getCurrentPowerConsumption() << endl;
            cout << "Current Power Saving is  " << getCurrentPowerSaving() << endl;
        } 
    }
  
  
  
    TracePowerConsumption::TracePowerConsumption(CPU* c, Tick period, char* filename) : 
        PeriodicTimer(period), TraceAscii(filename), counter(0),totalPowerSaved(0),totalPowerConsumed(0) 
    {
        cpu = c;
    }   
  
    TracePowerConsumption::~TracePowerConsumption()
    {
        cpu = NULL;
    }
  
    long double TracePowerConsumption::getAveragePowerSaving() 
    {
        long double TPS = (totalPowerSaved) / ((long double) counter ); 
        if (counter > 0) return TPS;
        return 0;
    }
  
    long double TracePowerConsumption::getAveragePowerConsumption() 
    {
        long double TPC = (totalPowerConsumed) / ((long double) counter ); 
        if (counter > 0) return TPC/(cpu->getMaxPowerConsumption());
        return 0;
    }
  
  
    void TracePowerConsumption::action()
    {
        /* It periodically updates the variables: */
        double currentPowerConsumption = cpu->getCurrentPowerConsumption();
        totalPowerConsumed += currentPowerConsumption;
        counter ++;
    
        long double TPC = getAveragePowerConsumption();
        record("Average Power Consumption:");
        record(TPC);
    }
  
  
    uniformCPUFactory::uniformCPUFactory() 
    { 
        _curr=0; 
        _n=0;
        index = 0;
    }
  
  
    uniformCPUFactory::uniformCPUFactory(char* names[], int n) 
    {
        _n=n;
        _names = new char*[n];
        for (int i=0; i<n; i++) {
            _names[i]=names[i];
        }
        _curr=0;
        index = 0;
    }
  
  
    CPU* uniformCPUFactory::createCPU(const string &name, int num_levels, double V[], int F[]) 
    { 
        CPU *c;
        if (_curr==_n)
            if (num_levels==1)
                // Creates a CPU without Power Saving: 
                c =  new CPU(name);
            else
                // Creates a CPU with Power Saving: 
                c = new CPU(name, num_levels, V, F);
        else
            if (num_levels==1) 
                // Creates a CPU without Power Saving: 
                c =  new CPU(_names[_curr++]); 
            else
                // Creates a CPU with Power Saving:
                c = new CPU(_names[_curr++], num_levels, V, F);

        c->setIndex(index++);
        return c;
    }
}
