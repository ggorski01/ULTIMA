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


class MCB
{
public:
    Scheduler *mysch;
    Semaphore *mysema;  //Regular semaphore.
    Semaphore *mysema0; //Regular semaphore.

    Window *mywin;

    bool SUSPEND;
    MCB(Scheduler *theScheduler, Semaphore *theSemaphore, Semaphore *theSemaphore0,  Window *theWIN);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
MCB::MCB(Scheduler *theScheduler, Semaphore *theSemaphore, Semaphore *theSemaphore0,  Window *theWIN)
{
    this->mysch = theScheduler;
    this->mywin = theWIN;
    this->mysema = theSemaphore; //Regular semaphore.
    this->mysema0 = theSemaphore0; //Regular semaphore.
    this->SUSPEND = false;
}
//----------------------------------------

//----------------------------------------
//----    G L O B A L  O B J E C T    ----
//----------------------------------------

MCB *mcb;
//----------------------------------------

#endif