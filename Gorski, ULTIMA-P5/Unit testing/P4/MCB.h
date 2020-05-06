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
class Ufs;

class MCB
{
public:
    Scheduler *mysch;
    Semaphore *mysema3; //Filesystem semaphore.
    Window *mywin;
    Ufs *myufs;
    bool SUSPEND;
    MCB(Scheduler *theScheduler, Semaphore *theSemaphore3, Window *theWIN, Ufs *theUfs);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
MCB::MCB(Scheduler *theScheduler, Semaphore *theSemaphore3, Window *theWIN,Ufs *theUfs)
{
    this->mysch = theScheduler;
    this->mywin = theWIN;
    this->myufs = theUfs;
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