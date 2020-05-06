/*
Filename: MCB.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To create a Master Control Block mechanism in which registers
            other mechanisms such as IPC, Scheduler, Semaphore...
            Furthermore, its registration allows the usage of such mechanisms
            without controlling access issues.
                    PHASE II
*/
#ifndef MCB_H
#define MCB_H

#include <iostream>

#include "Scheduler.h"
#include "Semaphore.h"
#include "Window.h"

class Semaphore;
class Scheduler;
class Window;
class MemoryManager;

class MCB
{
public:
    Scheduler *mysch;
    Semaphore *mysema2; //Memory Management semaphore.
    Window *mywin;
    MemoryManager *mymemo;

    bool SUSPEND;
    MCB(Scheduler *theScheduler, Semaphore *theSemaphore2, Window *theWIN, MemoryManager *theMemo);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
MCB::MCB(Scheduler *theScheduler,Semaphore *theSemaphore2, Window *theWIN, MemoryManager *theMemo)
{
    
    this->mysch = theScheduler;
    this->mywin = theWIN;
    this->mymemo = theMemo;
    this->mysema2 = theSemaphore2; //Memory Management semaphore.
    this->SUSPEND = false;
}
//----------------------------------------

//----------------------------------------
//----    G L O B A L  O B J E C T    ----
//----------------------------------------

MCB *mcb;
//----------------------------------------

#endif