/*
Filename: ThreadTable.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To implement a thread table structure alongside with functions to maneuver it.
*/
#ifndef THREAD_TABLE_H
#define THREAD_TABLE_H

#include <iostream>
#include <curses.h>
using namespace std;

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

struct TCB
{
    enum STATE threadState;
    int threadID;
    pthread_t processID;
    clock_t threadStartTime;
    WINDOW *threadWIN;
    TCB *next;
};

class ThreadTable
{
private:
    TCB *head;
    TCB *tail;
    int threadTableSize;
    TCB *iter;
public:
    ThreadTable();
    ~ThreadTable();
    bool empty();
    void setTCBStartTime(int mythreadID, clock_t myStartTime);
    clock_t getTCBStartTime(int mythreadID);
    void insertTCB(int mythreadID, clock_t myStartTime, STATE mythreadState, WINDOW *myThreadWIN);
    void removeATCB(int mythreadID);
    void removeLastTCB();
    void removeFirstTCB();
    bool findTCB(int mythreadID);
    TCB *getTCB(int mythreadID);
    pthread_t *getTCBProcessID(int mythreadID);
    TCB *getTCBHead();
    TCB *getTCBTail();
    int getThreadTableSize();
    void setTCBState(int mythreadID, STATE myState);
    STATE getTCBState(int mythreadID);
    void dumpThreadTable();
    WINDOW *getTCBWIN(int mythreadID);
    TCB * getTCBIter();
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor.
ThreadTable::ThreadTable()
{
    iter = head = tail = NULL;
    threadTableSize = 0;
}
//----------------------------------------

//Destructor.
ThreadTable::~ThreadTable()
{
    TCB *tmp = head;
    while (tmp)
    {
        tmp = tmp->next;
        delete (head);
        head = tmp;
    }
    iter = head = tail = NULL;
    threadTableSize = 0;
}
//----------------------------------------

//empty() - Return if thread table is empty.
bool ThreadTable::empty()
{
    if (threadTableSize == 0)
        return true;
    else
        return false;
}
//----------------------------------------

//setTCBStartTime(int,clock_t) - Set the value of the thread
//                               start time to a specified time.
void ThreadTable::setTCBStartTime(int mythreadID, clock_t myStartTime)
{
    TCB *tmp = head;
    while (tmp != NULL)
    {
        if (mythreadID == tmp->threadID)
        {
            tmp->threadStartTime = myStartTime;
            break;
        }
        tmp = tmp->next;
    }
}
//----------------------------------------

//getTCBStartTime(int) - Get the start time of a specified
//                       thread number.
clock_t ThreadTable::getTCBStartTime(int mythreadID)
{
    TCB *tmp = head;
    while (tmp != NULL)
    {
        if (mythreadID == tmp->threadID)
            return tmp->threadStartTime;
        tmp = tmp->next;
    }
}
//----------------------------------------

//insertTCB(int, clock_t, STATE) - Insert a new TCB block at
//                                 the end of thread Table.
void ThreadTable::insertTCB(int mythreadID, clock_t myStartTime, STATE mythreadState, WINDOW *myThreadWIN)
{
    TCB *newBlock = new TCB;
    newBlock->threadID = mythreadID;
    newBlock->threadStartTime = myStartTime;
    newBlock->threadState = mythreadState;
    newBlock->threadWIN = myThreadWIN;
    newBlock->next = NULL;

    if (empty())
        iter = head = tail = newBlock;
    else
    {
        tail->next = newBlock;
        tail = newBlock;
    }
    threadTableSize++;
}
//----------------------------------------

//removeATCB(int) - Remove a specific TCB of the thread table.
void ThreadTable::removeATCB(int threadID)
{
    if(head->threadID == threadID)
        removeFirstTCB();
    else if(tail->threadID == threadID)
        removeLastTCB();
    else{   
        TCB *curr = head;
        TCB *prev;
        while (curr->next && curr->threadID != threadID)
        {
            prev = curr;
            curr = curr->next;
        }
        if (curr->threadID == threadID)
        {
            prev->next = curr->next;
            delete curr;
            threadTableSize--;
        }
    }
}
//----------------------------------------

//removeLastTCB() - Remove last TCB block of thread table.
void ThreadTable::removeLastTCB()
{
    if (head->next == NULL)
        delete (head);
    else
    {
        TCB *tmp = head;
        while (tmp->next->next != NULL)
            tmp = tmp->next;
        delete (tmp->next);
        tmp->next = NULL;
        tail = tmp;
    }
    threadTableSize--;
}
//----------------------------------------

//removeFirstTCB() - Remove first TCB block of thread table.
void ThreadTable::removeFirstTCB()
{
    if(!empty())
    {
        TCB *tmp = head->next;
        delete(head);
        head = tmp;
    }   
    threadTableSize--;
}
//----------------------------------------

//findTCB(int) - Return a thread specific TCB.
bool ThreadTable::findTCB(int mythreadID)
{
    TCB *tmp = head;
    while (tmp)
    {
        if (mythreadID == tmp->threadID)
            return true;
        tmp = tmp->next;
    }
    return false;
}
//----------------------------------------

//getTCB() - Return a pointer to TCB searched by threadID.
TCB *ThreadTable::getTCB(int mythreadID)
{
    TCB *tmp = head;
    while (tmp)
    {
        if (mythreadID == tmp->threadID)
            return tmp;
        tmp = tmp->next;
    }
    cout << "getTCB - TCB not found! Return exit(-1)." << endl;
    exit(-1);
}
//----------------------------------------

//getTCBProcessID(int) - Return a process ID pthread_t of a TCB.
pthread_t *ThreadTable::getTCBProcessID(int mythreadID)
{
    TCB *tmp = head;
    while (tmp != NULL)
    {
        if (mythreadID == tmp->threadID)
            return &tmp->processID;
        tmp = tmp->next;
    }
    cout << "getTCBProcessID - ProcessID not found! Return exit(-1)." << endl;
    exit(-1);
}

//getTCBHead() - Return a pointer to first TCB in thread table.
TCB *ThreadTable::getTCBHead()
{
    return head;
}
//----------------------------------------

//getTCBTail() - Return a pointer to last TCB in thread table.
TCB *ThreadTable::getTCBTail()
{
    return tail;
}
//----------------------------------------

//getThreadTableSize() - Return thread table size.
int ThreadTable::getThreadTableSize()
{
    return threadTableSize;
}
//----------------------------------------

//setTCBState(int,STATE) - Set TCB state of a specified threadID.
void ThreadTable::setTCBState(int mythreadID, STATE myState)
{
    TCB *tmp = head;
    while (tmp != NULL)
    {
        if (mythreadID == tmp->threadID)
        {
            tmp->threadState = myState;
            break;
        }
        tmp = tmp->next;
    }
}
//----------------------------------------

//getTCBState(int) - Get a specified TCB state.
STATE ThreadTable::getTCBState(int mythreadID)
{
    TCB *tmp = head;
    while (tmp != NULL)
    {
        if (mythreadID == tmp->threadID)
        {
            return tmp->threadState;
        }
        tmp = tmp->next;
    }
}
//----------------------------------------

//dumpThreadTable() - Output everything from over thread table.
void ThreadTable::dumpThreadTable()
{
    cout << "..... START ThreadTable Dump ..... " << endl;
    TCB *tmp = head;
    if (!empty())
    {
        while (tmp != NULL)
        {
            cout << "TaskNUM: " << tmp->threadID << endl;
            cout << "TaskID: " << tmp->threadID << endl;
            cout << "TaskState: ";
            switch (tmp->threadState)
            {
            case 0:
                cout << "READY" << endl;
                break;
            case 1:
                cout << "RUNNING" << endl;
                break;
            case 2:
                cout << "BLOCKED" << endl;
                break;
            case 3:
                cout << "DEAD" << endl;
                break;
            }
            cout << "..............................." << endl;
            tmp = tmp->next;
        }
        cout << "..... END ThreadTable Dump ..... " << endl;
    }
    else
    {
        cout << "EMPTY TCB - RETURN EXIT(-1)" << endl;
        exit(-1);
    }
}
//----------------------------------------
WINDOW *ThreadTable::getTCBWIN(int mythreadID)
{
    TCB *tmp = head;
    while (tmp != NULL)
    {
        if (mythreadID == tmp->threadID)
        {
            return tmp->threadWIN;
        }
        tmp = tmp->next;
    }
    cout << "GetTCBWIN - TCB not found! Return exit(-1)." << endl;
    exit(-1);
}

//
TCB * ThreadTable::getTCBIter()
{   
    if(iter->next!=NULL)
        iter = iter->next;
    else
        iter = getTCBHead();
    return iter;
}
#endif