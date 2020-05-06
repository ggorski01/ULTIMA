/*
Filename: Inode.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To create a inode structure.
                    Phase IV
*/
#ifndef INODE_H
#define INODE_H

#include<iostream>
using namespace std;


class inode
{
    public:
        char filename[8]; //Name of the file created.
        int owner_task_id;  //Task ID which created the file.
        int starting_block; //Block address in which the filename data starts.
        int size; //Total filesize.
        char permission[4]; //permission[4] = {user_read, user_write, others_read, others_write}
        int blocks[4]; //Max file size = 4 blocks of 128 bytes each.
        int handle;
        int next_block; //Indicate the next block related to inode.
};

#endif
