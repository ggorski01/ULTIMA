/*
Filename: Semaphore.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement a binary semaphore.
*/
#ifndef Semaphore_H
#define Semaphore_H

#include <iostream>
#include <string>
using namespace std;

#include "Queue.h"
#include "Scheduler.h"


class Semaphore
{
    private:
        string resourceName; // The name of the resource being managed.
        int semaValue;       // 0 or 1 in the case of a binary Semaphore.
        int luckyTask;       // Preserve the task-id of the task that got the resource (for debbugging).
        pthread_mutex_t mux;  // Critical section lock mechanism.
        Queue<int> semaQueue;
        Scheduler *schedPtr;
    public:
        Semaphore(int starting_value, string name, Scheduler *theScheduler);
        ~Semaphore();
        void down(int taskID);
        void up();
        void dump(int level);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
Semaphore::Semaphore(int starting_value, string name, Scheduler *theScheduler)
{
    semaValue = starting_value;
    resourceName = name;
    luckyTask = -1;
    schedPtr = theScheduler;
    pthread_mutex_init(&mux,NULL);
}
//---------------------------------------------

//Destructor
Semaphore::~Semaphore()
{
    while(semaQueue.size() > 0)
        semaQueue.dequeue();
    semaValue = -99;
    resourceName = "";
    luckyTask = -1;
    pthread_mutex_destroy(&mux);
}
//---------------------------------------------

//down() - Get the hold of the resource or get queued.
void Semaphore::down(int taskID)
{
    pthread_mutex_lock(&mux);
    if (taskID == luckyTask)
    {
        cout << "Task # " << luckyTask << " already has the resource! Ignore request." << endl;
        dump(1);
    }
    else
    {
        if (semaValue >= 1)
        {
            semaValue--;
            luckyTask = taskID; // preserve the task-id who got the resource
            dump(1);
        }
        else
        {
            semaQueue.enqueue(taskID);
            schedPtr->setState(taskID, BLOCKED);
            dump(1);
            schedPtr->yield();
            dump(1);
        }
    }
    pthread_mutex_unlock(&mux);
}
//---------------------------------------------

//up() - Release the hold of the resource.
void Semaphore::up()
{
    pthread_mutex_lock(&mux);
    int task_id;

    cout << "TaskID : " << schedPtr->getCurrTaskID() << ", LuckyID : " << luckyTask << endl;
    if (schedPtr->getCurrTaskID() == luckyTask) // check to see if the correct task is doing the up()
    {
        if (semaQueue.empty())
        {
            semaValue++;
            luckyTask = -1;
            dump(1);
        }
        else
        {
            task_id = semaQueue.top()->data; // Remove from queue and unblock
            semaQueue.dequeue();
            schedPtr->setState(task_id, READY); // set the task to READY
            cout << "UnBlock : " << task_id << " and release from the queue." << endl;
            luckyTask = task_id; //
            cout << "Luck Task = " << luckyTask << endl;
            dump(1);
            schedPtr->yield();
            dump(1);
        }
    }
    else
    {
        cout << "Invalid Semaphore UP(). TaskID : " << schedPtr->getCurrTaskID() << " does not own the resource." << endl;
        dump(1);
    }
    pthread_mutex_unlock(&mux);
}
//---------------------------------------------

//dump() - Output information over the Semaphore.
void Semaphore::dump(int level)
{
    cout << "-------Semaphore DUMP-------" << endl;
    switch (level)
    {
    case 0:
        cout << "Sema_Value: " << semaValue << endl;
        cout << "Sema_Name : " << resourceName << endl;
        cout << "Obtained by Task-ID: " << luckyTask << endl;
        break;
    case 1:
        cout << "Sema_Value: " << semaValue << endl;
        cout << "Sema_Name : " << resourceName << endl;
        cout << "Obtained by Task-ID: " << luckyTask << endl;
        cout << "Sema-Queue: ";
        semaQueue.dumpQueue();
        break;
    default:
        cout << "ERROR in Semaphore DUMP level";
    }
    cout << "----------------------------" << endl;
}

#endif