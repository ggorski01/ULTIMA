/*
Filename: IPC.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement a Round Robin scheduler mechanism.
*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifndef THREAD_STATES
#define THREAD_STATES
enum STATE
{
    READY,
    RUNNING,
    BLOCKED,
    DEAD
};
#endif

const int NO_OF_THREADS = 3;

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include "ThreadTable.h"

#include "MCB.h"
#include "IPC.h"

using namespace std;

class Scheduler
{
    private:
        int currentThread;
        long currentQuantum;
        int nextAvailableThreadID;
        ThreadTable threadTable;
        MCB *mcb;
    public:
        Scheduler();
        ~Scheduler();
        int create_task();
        void *worker(void *arguments);
        void start();
        void yield();
        void setQuantum(long quantum);
        long getQuantum();
        void setState(int task_id, STATE the_state);
        int getCurrTaskID();
        TCB* getTCB(int task_id);
        void dump();
        void wasteTime(int x);
        void destroyTask(int task_id);
        void garbageCollector();
        void setMCB(MCB *mcb);
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
Scheduler::Scheduler()
{
    currentThread = -1;//No active process.
    nextAvailableThreadID = 0;
    currentQuantum = 300; //default quantum 300ms.
}
//----------------------------------------

//Destructor
Scheduler::~Scheduler()
{
    currentThread = -1;//No active process.
    nextAvailableThreadID = 0;
    currentQuantum = 300; //default quantum 300ms.
}
//----------------------------------------

//CreateTask() -- Create an operating system process.
int Scheduler::create_task()
{
    if (nextAvailableThreadID < NO_OF_THREADS)
    {
        cout << "Creating task # " << nextAvailableThreadID << endl;
        threadTable.insertTCB(nextAvailableThreadID, clock(), READY);
        //Source: https://thispointer.com/c-how-to-pass-class-member-function-to-pthread_create/
        typedef void *(*THREADFUNCPTR)(void *);
        int err = pthread_create(threadTable.getTCBProcessID(nextAvailableThreadID), NULL, (THREADFUNCPTR)&Scheduler::worker, this);        assert(!err);
        mcb->myIPC->createMailbox(nextAvailableThreadID);
        nextAvailableThreadID++;
        return (nextAvailableThreadID - 1);
    }
    else
    {
        cout << "Create task FAILED: Number of available threads exceeded the limit.." << endl;
        return (-1);// return error in case cannot create a task.
    }
}
//----------------------------------------

//*worker() - Context swapping function.
void *Scheduler::worker(void *arguments)
{
    int mythreadNum = 0;
    //Find the TCB related to the thread running in the OS background.
    while (*threadTable.getTCBProcessID(mythreadNum) != pthread_self())
        mythreadNum++;

    //As long as the OS background running thread is not DEAD
    //and it is running do:
    while ((threadTable.getTCBState(mythreadNum) != DEAD))
    {
        if (threadTable.getTCBState(mythreadNum) == RUNNING)
        {
            //Prints the pthread_t of the running thread.
            cout << "Thread Num# :" << mythreadNum
                 << " Pthread_self() #: " << pthread_self() 
                 << endl;
            //Print all information over the Scheduler class.
            dump();
            //Call Scheduler::yield() so that current thread can
            //voluntary give up the CPU to the next ready task.
            yield();
        }
        else
        {
            //If not, just relinquish the CPU by the calling thread
            //, insuring for example that the calling thread of
            //a createTask() function call do not hold the CPU forever. 
            pthread_yield();
        }
    }
}
//----------------------------------------

//start() - Initiate scheduling mechanism by
//          setting the first task in thread table
//          to running.
void Scheduler::start()
{
    cout << "............." << endl;
    cout << "............. STARTING SCHEDULING" << endl;
    cout << ".............\n"
         << endl;
    TCB *tmp = threadTable.getTCBHead();
    //Find the first READY process
    //by jumping through DEAD processes
    while (tmp->threadState == DEAD)
        tmp = tmp->next;
    //Set the first found READY process to state RUNNING, then begin scheduling
    threadTable.setTCBStartTime(tmp->threadID, clock());
    threadTable.setTCBState(tmp->threadID, RUNNING);
    currentThread = tmp->threadID;
    setQuantum(1000 / NO_OF_THREADS); // Set quantum to 1sec/number of threads
    wasteTime(2);
}
//----------------------------------------

//yield() - runs the thread with set RUNNING state until
//          its quantum runs out. When quantum runs out,
//          set current RUNNING to READY then finds the next
//          READY thread then set its state to RUNNING. Finally,
//          puts the OS thread current running in background to sleep
//          so that the next can have its turn to run.
void Scheduler::yield()
{
    cout << "-----------------------------------------------------------\n"
         << endl;
    int counter = 0;
    cout << "Current Task # " << currentThread << " is trying to yield." << endl;
    cout << "Current Quantum : " << currentQuantum << endl;
    // calculate elapsed_time since the thread last started to run.
    clock_t elapsed_time = clock() - threadTable.getTCBStartTime(currentThread);
    cout << "Elapsed time: " << elapsed_time << endl;
    if (elapsed_time >= currentQuantum) // if quantum has run out
    {
        cout << "yielding....(Switching from task #" << currentThread << " to next ready task)." << endl;

        // if current thread is RUNNING make it READY (its quantum has run out)
        if (threadTable.getTCBState(currentThread) == RUNNING)
            threadTable.setTCBState(currentThread, READY);

        // now find the next READY thread and make it running
        // watch out for deadlocks. (no ready processes)
        currentThread = (currentThread + 1) % NO_OF_THREADS;
        if (threadTable.findTCB(currentThread) == false)
            currentThread = (currentThread + 1) % NO_OF_THREADS;

        while (threadTable.getTCBState(currentThread) != READY && counter < NO_OF_THREADS - 1)
        {
            currentThread = (currentThread + 1) % NO_OF_THREADS;
            if (threadTable.findTCB(currentThread) == false)
                currentThread = (currentThread + 1) % NO_OF_THREADS;
        }
        // if we find a READY threads re-set the quantum and set the task to running
        if (counter < NO_OF_THREADS - 1 && threadTable.getTCBState(currentThread) == READY)
        {
            threadTable.setTCBStartTime(currentThread, clock()); // restart the quantum
            threadTable.setTCBState(currentThread, RUNNING);
            cout << "Started Running task # " << currentThread << endl;
            //Sleep(1) in here, insures syncronization between the 
            //threads and the yield function.
            sleep(1);
        }
        else
            cout << "----- P O S S I B L E   D E A D   L O C K -----" << endl;
    }
    else
        cout << "---- N O   Y I E L D -----" << endl;
}
//----------------------------------------

//setQuantum(long quantum) - Assign a passed value
//                           to currentQuantum.
void Scheduler::setQuantum(long quantum)
{
    currentQuantum = quantum;
}
//----------------------------------------

//getQuantum() - Return currentQuantum value
long Scheduler::getQuantum()
{
    return (currentQuantum);
}
//----------------------------------------

//setState(int,STATE) - Set the state of a specific task to
//                      a specific state.
void Scheduler::setState(int task_id, STATE the_state)
{
    threadTable.setTCBState(task_id, the_state);
}
//----------------------------------------

//getTaskID() - Return currentThread ID.
int Scheduler::getCurrTaskID()
{
    return currentThread;
}
//----------------------------------------

//getTCB() - Return a pointer to TCB searched 
//           by threadID.
TCB *Scheduler::getTCB(int task_id)
{
    TCB *tmp = threadTable.getTCBHead();
    while (tmp)
    {
        if (task_id == tmp->threadID)
            return tmp;
        tmp = tmp->next;
    }
    cout << "getTCB - TCB not found! Return exit(-1)." << endl;
    exit(-1);
}
//----------------------------------------

//dump() - Ouput important messages over the Scheduler.
void Scheduler::dump()
{
    cout << "---------------------- PROCESS TABLE ----------------------" << endl;
    clock_t elapsed_time = clock() - threadTable.getTCBStartTime(currentThread);
    cout << "Quantum = " << currentQuantum << endl;
    TCB *tmp = threadTable.getTCBHead();
    while (tmp != NULL)
    {
        cout << " task id: " << tmp->threadID;
        cout << " process id" << tmp->processID;
        cout << " task state: ";
        switch (tmp->threadState)
        {
            case 0:
                cout << "READY";
                break;
            case 1:
                cout << "RUNNING";
                break;
            case 2:
                cout << "BLOCKED";
                break;
            case 3:
                cout << "DEAD";
                break;
        }
        if (tmp->threadID == currentThread)
            cout << " <-- CURRENT PROCESS";
        cout << endl;
        tmp = tmp->next;
    }
    cout << "-----------------------------------------------------------\n"
         << endl;
}
//----------------------------------------

//wasteTime(int) - runs a for loop for (x * 
//                 10240) amount of times.
void Scheduler::wasteTime(int x)
{
    unsigned long long Int64 = 0;
    for (unsigned short i = 0; i < 10240 * x; ++i)
    {
        for (unsigned short j = i; j > 0; --j)
            Int64 += j + i;
    }
}
//----------------------------------------

//garbageCollector() - Goes through the thread
//                     table finding all the 
//                     state DEAD threads,
//                     after finding it, removes
//                     its TCB from thread table.
void Scheduler::garbageCollector()
{
    cout << "Garbage collector " << endl;
    TCB *tmp = threadTable.getTCBHead();
    while (tmp != NULL)
    {
        if (tmp->threadState == DEAD)
        {
            threadTable.removeATCB(tmp->threadID);
            mcb->myIPC->removeAMailbox(tmp->threadID);
        }
        tmp = tmp->next;
    }
}
//----------------------------------------

//destroyTask(int) - Sets the state of a task
//                   to DEAD, then calls a pthread
//                   library function pthread_cancel
//                   to cancel the running task in the
//                   background.
void Scheduler::destroyTask(int task_id)
{
    threadTable.setTCBState(task_id, DEAD);
    pthread_cancel(*threadTable.getTCBProcessID(task_id));
}
//----------------------------------------

//setMCB(MCB) - Register the Master Control Block mechanism
//              within the scheduler so that Scheduler
//              can access other mechanisms such as IPC,Semaphore,
//              ...
void Scheduler::setMCB(MCB *mcb)
{
    this->mcb = mcb;
}
//----------------------------------------

#endif