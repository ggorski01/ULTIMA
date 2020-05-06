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

#include<iostream>

#include "Scheduler.h"
#include "Semaphore.h"
#include "IPC.h"

class Semaphore;
class Scheduler;
class IPC;

class MCB
{
    public:
        Scheduler *mysch;
        Semaphore *mysema;
        IPC *myIPC;
        MCB(Scheduler *theScheduler, Semaphore *theSemaphore, IPC *theIPC);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
MCB::MCB(Scheduler *theScheduler, Semaphore *theSemaphore, IPC *theIPC)
{
    this->myIPC = theIPC;
    this->mysch = theScheduler;
    this->mysema = theSemaphore;
}
//----------------------------------------

//----------------------------------------
//----    G L O B A L  O B J E C T    ----
//----------------------------------------

MCB *mcb;
//----------------------------------------

#endif