/*
Filename: Queue.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To create a queue data structure.
*/
#ifndef QUEUE_LIST_H
#define QUEUE_LIST_H

#include "LinkedList.h"
#include <iostream>
using namespace std;

#include "Node.h"

template <class T>
class Queue
{
    private:
        int queueSize;
        Node <T> *head;
    public:
        Queue();
        ~Queue();
        void enqueue(T data, int num = 0);
        void dequeue();
        bool empty();
        Node<T>* top();
        int size();
        void dumpQueue();
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor
template <class T>
Queue<T>::Queue()
{
    head = NULL;
    queueSize = 0;
}
//----------------------------------------

//Destructor
template <class T>
Queue<T>::~Queue()
{
    Node<T> *tmp = head;
    while(tmp)
    {
        tmp = tmp->next;
        delete(head);
        head = tmp;
    }
    head = NULL;
    queueSize = 0;
}
//----------------------------------------

//empty() - Returns if Queue is empty or not.
template <class T>
bool Queue<T>::empty()
{
    if(size() > 0) return false;
    else return true;
}
//----------------------------------------

//enqueue(T,int) - Insert node at the begin of 
//                 the queue list.
template <class T>
void Queue<T>::enqueue(T data, int num)
{
    Node<T> *newNode = new Node<T>;
    newNode->data = data;
    newNode->num = num;
    newNode->next = NULL;

    if(empty())
    {
        head = newNode;
    }
    else
    {
        newNode->next = head;
        head = newNode;
    }
    queueSize++;
}
//----------------------------------------

//dequeue() - Remove node at the begin of the list.
template <class T>
void Queue<T>::dequeue()
{
        if(!empty())
    {
        Node<T> *tmp = head->next;
        delete(head);
        head = tmp;
    }
    queueSize--;
}
//----------------------------------------

//top() - Return the last node of the linked list.
template <class T>
Node<T>* Queue<T>::top()
{
    if(head)
        return head;
    else
    {
        cout << "Not found!" << endl;
    }
    
}
//----------------------------------------

//size() - Return size of linked list.
template <class T>
int Queue<T>::size()
{
    return queueSize;
}
//----------------------------------------

//dumpQueue() - Output all node in Queue.
template <class T>
void Queue<T>::dumpQueue()
{
    Node<T> *tmp = head;
    if(!empty())
    {
        while(tmp!= NULL)
        {
            cout << tmp->data << " | " ;
            tmp = tmp->next;
        }
        cout << endl;
    }else cout << "---- EMPTY ----" << endl;
}
//----------------------------------------

#endif