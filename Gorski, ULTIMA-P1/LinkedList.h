/*
Filename: LinkedList.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To create a linked list data structure.
*/
#ifndef LINK_LIST_H
#define LINK_LIST_H

#include <iostream>
using namespace std;

#include "Node.h"

template <class T>
class LinkedList
{
private:
    Node<T> *head;
    Node<T> *tail;
    int listSize;

public:
    LinkedList();
    ~LinkedList();
    void insertFront(T data, int num = 0);
    void insertBack(T data, int num = 0);
    void removeFront();
    void removeBack();
    void removeNode(T data);
    void removeNode(int num);
    Node<T> *front();
    Node<T> *back();
    int size();
    bool empty();
    Node<T> *searchNode(T data);
    Node<T> *searchNode(int data);
    bool isNode(int data);
    void dumpList();
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor.
template <class T>
LinkedList<T>::LinkedList()
{
    head = tail = NULL;
    listSize = 0;
}
//----------------------------------------

//Destructor.
template <class T>
LinkedList<T>::~LinkedList()
{
    Node<T> *tmp = head;
    while (tmp)
    {
        tmp = tmp->next;
        delete (head);
        head = tmp;
    }
    head = tail = NULL;
    listSize = 0;
}
//----------------------------------------

//insertFront(T, int) - Insert node at the begin 
//                      of the linked list.
template <class T>
void LinkedList<T>::insertFront(T data, int num)
{
    Node<T> *newNode = new Node<T>;
    newNode->data = data;
    newNode->num = num;
    newNode->next = NULL;

    if (empty())
    {
        head = tail = newNode;
    }
    else
    {
        newNode->next = head;
        head = newNode;
    }
    listSize++;
}
//----------------------------------------

//insertBack(T,int) - Insert node at the end of linked list.
template <class T>
void LinkedList<T>::insertBack(T data, int num)
{
    Node<T> *newNode = new Node<T>;
    newNode->data = data;
    newNode->num = num;
    newNode->next = NULL;

    if (empty())
        head = tail = newNode;
    else
    {
        tail->next = newNode;
        tail = newNode;
    }
    listSize++;
}
//----------------------------------------

//removeFront() - Remove node at the begin of the list.
template <class T>
void LinkedList<T>::removeFront()
{
    if (!empty())
    {
        Node<T> *tmp = head->next;
        delete (head);
        head = tmp;
    }
    listSize--;
}
//----------------------------------------

//removeBack() - Remove node at the end of the list.
template <class T>
void LinkedList<T>::removeBack()
{
    if (head->next == NULL)
        delete (head);
    else
    {
        Node<T> *tmp = head;
        while (tmp->next->next != NULL)
            tmp = tmp->next;
        delete (tmp->next);
        tmp->next = NULL;
        tail = tmp;
    }
    listSize--;
}
//----------------------------------------

//removeNode() - Remove node by specification type T.
template <class T>
void LinkedList<T>::removeNode(T data)
{
    if (head->data == data)
        removeFront();
    else if (tail->data == data)
        removeBack();
    else
    {
        Node<T> *tmp = head;
        Node<T> *prev;
        while (tmp->next && tmp->data != data)
        {
            prev = tmp;
            tmp = tmp->next;
        }
        if (tmp->data == data)
        {
            prev->next = tmp->next;
            delete tmp;
            listSize--;
        }
        else
            exit(-1);
    }
}
//----------------------------------------

//removeNode(int) - Remove node by specification type int.
template <class T>
void LinkedList<T>::removeNode(int num)
{
    if (num == 0)
        removeFront();
    else if (listSize == num)
        removeBack();
    else
    {
        Node<T> *tmp = head;
        Node<T> *prev;
        int count = 0;
        while (tmp->next && count != num)
        {
            prev = tmp;
            tmp = tmp->next;
            count++;
        }
        if (count == num)
        {
            prev->next = tmp->next;
            delete tmp;
            listSize--;
        }
        else
            exit(-1);
    }
}
//----------------------------------------

//front() - Return head/first node of the linked list.
template <class T>
Node<T>*LinkedList<T>::front()
{
    return head;
}

//back() - Return the last node of the linked list.
template <class T>
Node<T> *LinkedList<T>::back()
{
    return tail;
}
//----------------------------------------

//size() - Return size of linked list.
template <class T>
int LinkedList<T>::size()
{
    return listSize;
}
//----------------------------------------

//empty() - Return if linked list is empty or not.
template <class T>
bool LinkedList<T>::empty()
{
    if (listSize > 0)
        return false;
    else
        return true;
}
//----------------------------------------

//searchNode(T) - Search node by specification type T.
template <class T>
Node<T> *LinkedList<T>::searchNode(T data)
{
    Node<T> *tmp = new Node<T>;
    tmp = head;
    while (tmp)
    {
        if (data == tmp->data)
            return tmp;
        tmp = tmp->next;
    }
    exit(-1);
}
//----------------------------------------

//searchNode(int) - Search node by specification type int.
template <class T>
Node<T> *LinkedList<T>::searchNode(int data)
{
    Node<T> *tmp = new Node<T>;
    tmp = head;
    while (tmp)
    {
        if (data == tmp->num)
            return tmp;
        tmp = tmp->next;
    }
    exit(-1);
}
//----------------------------------------

//isNode(int) - Return if there is such a node in list.
template <class T>
bool LinkedList<T>::isNode(int data)
{
    Node<T> *tmp = new Node<T>;
    tmp = head;
    while (tmp)
    {
        if (data == tmp->num)
            return true;
        tmp = tmp->next;
    }
    return false;
}
//----------------------------------------

//dumpList() - Output the linked list if there is one,
//             if not exit with error.
template <class T>
void LinkedList<T>::dumpList()
{
    Node<T> *tmp = head;
    if (!empty())
    {
        while (tmp != NULL)
        {
            cout << tmp->data << "  ";
            tmp = tmp->next;
        }
        cout << endl;
    }
    else
        cout << "---- EMPTY ----" << endl;
}
//----------------------------------------
#endif