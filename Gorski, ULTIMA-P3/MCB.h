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
*/
#ifndef MCB_H
#define MCB_H

#include <iostream>

#include "Scheduler.h"
#include "Semaphore.h"
#include "Window.h"

class Semaphore;
class Scheduler;
class IPC;
class Window;
class MemoryManager;

class MCB
{
public:
    Scheduler *mysch;
    Semaphore *mysema;
    Semaphore *mysema2;
    IPC *myIPC;
    Window *mywin;
    MemoryManager *mymemo;
    MCB(Scheduler *theScheduler, Semaphore *theSemaphore, Semaphore *theSemaphore2, IPC *theIPC, Window *theWIN, MemoryManager *theMemo);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
MCB::MCB(Scheduler *theScheduler, Semaphore *theSemaphore, Semaphore *theSemaphore2, IPC *theIPC, Window *theWIN, MemoryManager *theMemo)
{
    this->myIPC = theIPC;
    this->mysch = theScheduler;
    this->mysema = theSemaphore;
    this->mywin = theWIN;
    this->mymemo = theMemo;
    this->mysema2 = theSemaphore2;
}
//----------------------------------------

//----------------------------------------
//----    G L O B A L  O B J E C T    ----
//----------------------------------------

MCB *mcb;
//----------------------------------------

#endif