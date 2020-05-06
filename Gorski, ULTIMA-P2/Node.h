/*
Filename: Node.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To define the structure of a Node for a linked list, or queue.
*/
#ifndef NODE_H
#define NODE_H

#include <iostream>
using namespace std;

template <class T>
struct Node
{
    T data;
    int num;
    Node <T> *next;
};

#endif