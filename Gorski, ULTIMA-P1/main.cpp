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


//StartMCB() starts the Master Control Block
//object by registering its variables to the
//correspondent class objects.

int main()
{
    int t_id;
    Scheduler *mysch = new Scheduler();
    Semaphore *mysema = new Semaphore(1, "resource1", mysch);
    
        t_id = mysch->create_task();
        t_id = mysch->create_task();
        t_id = mysch->create_task();
        t_id = mysch->create_task();

    //Uncomment the below statements to
    //test the Scheduler::destroyTask(int)
    //and garbageCollector() functions.
    //mysch->destroyTask(2);
    //mysch->garbageCollector();
    mysch->start();
 
    //Binary Semaphore
    //---------- DOWN
    t_id = mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mysema->down(t_id); // hold on to the resource
    mysch->wasteTime(3);
    mysch->yield();

    t_id = mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mysema->down(t_id); // hold on to the resource
    mysch->wasteTime(3);
    mysch->yield();

    t_id = mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mysema->down(t_id); // hold on to the resource
    mysch->wasteTime(3);
    mysch->yield();

    //---------- UP
    cout << "START OF MY UP-----------------------" << endl<< endl;
    t_id = mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mysema->up(); // hold on to the resource
    mysch->wasteTime(3);
    mysch->yield();

    t_id = mysch->getCurrTaskID();

    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mysema->up(); // hold on to the resource
    mysch->wasteTime(3);
    mysch->yield();

    t_id = mysch->getCurrTaskID();
    cout << "Task " << t_id << " is trying to obtain the semaphore (Resource1)" << endl;
    mysema->up(); // hold on to the resource
    mysch->wasteTime(3);
    mysch->yield();

    return 0;
}
