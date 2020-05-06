/*
Filename: main.cpp
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement the main class for Ultima 2.0.Furthermore,
            this main should call some implemented functions in Phase II
            so that its functionality is demonstrated.
*/
#include <iostream>
#include <sched.h>
#include <time.h>
#include <pthread.h>

#include "Semaphore.h"
#include "Scheduler.h"

using namespace std;

#include "MCB.h"

//StartMCB() starts the Master Control Block
//object by registering its variables to the
//correspondent class objects.
void startMCB()
{
    Scheduler *newScheduler = new Scheduler();
    Semaphore *newSemaphore = new Semaphore(1,"resource1");
    IPC *newIPC = new IPC();
    mcb = new MCB(newScheduler,newSemaphore,newIPC);
    mcb->mysch->setMCB(mcb);
    mcb->mysema->setMCB(mcb);
}

int main()
{
    int t_id;

    startMCB();
        t_id = mcb->mysch->create_task();
        t_id = mcb->mysch->create_task();
        t_id = mcb->mysch->create_task();
        t_id = mcb->mysch->create_task();

    //Uncomment the below statements to
    //test the Scheduler::destroyTask(int)
    //and garbageCollector() functions.
    //mcb->mysch->destroyTask(2);
    //mcb->mysch->garbageCollector();
    mcb->mysch->start();
 
    cout << endl << endl;
    //Task 0 sends a message to Task 1
    int err = mcb->myIPC->sendMessage(0,1,TEXT,"Testing my main...");
    if(err == 0)
        cout << "Sending message from Task 0 to Task 1" << endl;
    else
        cout << "Impossible to send the message!" << endl;
    cout << endl << endl;
    //Below statements test the implemented class in Phase I: the
    //Binary Semaphore
    //---------- DOWN
    t_id = mcb->mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mcb->mysema->down(t_id); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    //---------- UP
    cout << "START OF MY UP-----------------------" << endl<< endl;
    t_id = mcb->mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    t_id = mcb->mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();

    //Uncomment the below statements so that a seconde message is
    //sent by Task 0 to Task 2
    //mcb->myIPC->receiveMessage(1);
    //mcb->myIPC->sendMessage(0,2,TEXT,"Testing my main2...");


    t_id = mcb->mysch->getCurrTaskID();
    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mcb->mysema->up(); // hold on to the resource
    mcb->mysch->wasteTime(3);
    mcb->mysch->yield();
    
    //Dumping entire IPC();
    mcb->myIPC->dumpIPC();
    
    //Uncomment the below statement to
    //dump an specific mailbox
    //mcb->myIPC->dumpMailbox(1);

    return 0;
}
