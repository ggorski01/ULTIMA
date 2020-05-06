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
class IPC;
class Window;
class MemoryManager;
class Ufs;

class MCB
{
public:
    Scheduler *mysch;
    Semaphore *mysema;  //Regular semaphore.
    Semaphore *mysema0; //IPC semaphore.
    Semaphore *mysema1; //IPC semaphore.
    Semaphore *mysema2; //Memory Management semaphore.
    Semaphore *mysema3; //Filesystem semaphore.
    IPC *myIPC;
    Window *mywin;
    MemoryManager *mymemo;
    Ufs *myufs;
    bool SUSPEND;
    MCB(Scheduler *theScheduler, Semaphore *theSemaphore, Semaphore *theSemaphore0, Semaphore *theSemaphore1,Semaphore *theSemaphore2, Semaphore *theSemaphore3, IPC *theIPC, Window *theWIN, MemoryManager *theMemo, Ufs *theUfs);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
MCB::MCB(Scheduler *theScheduler, Semaphore *theSemaphore, Semaphore *theSemaphore0, Semaphore *theSemaphore1,Semaphore *theSemaphore2, Semaphore *theSemaphore3, IPC *theIPC, Window *theWIN, MemoryManager *theMemo, Ufs *theUfs)
{
    this->myIPC = theIPC;
    this->mysch = theScheduler;
    this->mywin = theWIN;
    this->mymemo = theMemo;
    this->myufs = theUfs;
    this->mysema = theSemaphore; //Regular semaphore.
    this->mysema0 = theSemaphore0; //Regular semaphore.
    this->mysema1 = theSemaphore1; //IPC semaphore.
    this->mysema2 = theSemaphore2; //Memory Management semaphore.
    this->mysema3 = theSemaphore3; //Filesystem semaphore.
    this->SUSPEND = false;
}
//----------------------------------------

//----------------------------------------
//----    G L O B A L  O B J E C T    ----
//----------------------------------------

MCB *mcb;
//----------------------------------------

#endif