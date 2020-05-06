/*
Filename: Ufs.h
Author: Giovanna Gorski
Course: C435 - Operating Systems
Instructor: Dr. Hossein Hakimzadeh
Spring 2020
Purpose:    To create a file system mechanism.
                    Phase IV.
*/
#ifndef UFS_H
#define UFS_H

#include<iostream>
#include<fstream>
#include<string.h>
using namespace std;

#include "LinkedList.h"
#include "Inode.h"
#include "MCB.h"
class MCB;

//Modes for opening a file.
enum OPEN_MODE
{
    READ,
    WRITE
};

const int MAX_FILE_SIZE = 4 * 128; //4 blocks of 128 bytes each.

//Data structure to store
//information over openned
//files.
struct openedFile
{
    char filename[8];
    char permission[4];
    OPEN_MODE mode;
    int fileID;
    int ownerID;
    int opened_by_id;
    int write_cursor;
    int read_cursor;
};


class Ufs
{
    private:
        string fs_name;
        string fs_metafile_name;
        int fs_block_size; //For ultima it should be 128
        int fs_no_blocks;  //For ultima it should be 16
        int next_handle;
        char init_char; //For ultima it should be '^'
        int no_blocks_available; //At begin should be 16, but as a file is created it should decrement;
        int fs_id;
        LinkedList<openedFile> opened_files;
        inode *inodes;
        int next_file_id;
        MCB *mcb;
        char b[256];
    public:
        Ufs(string theFilename, int theNoBlocks, int theBlockSize, char theInitChar);
        void format();
        int Open(int task_id, int file_handle, string filename, OPEN_MODE mode);
        int Close(int task_id, int file_id);

        int Read_char(int task_id, int file_id, char *ch);
        int Write_char(int task_id, int file_id, char ch);

        int Create_file(int task_id, string filename, int file_size, string permission);
        int Del_file(int task_id, string filename);
        int Change_permission(int task_id,string filename,string new_permission);
        void Dir();
        void Dir(int task_id);
        void dump();
        void setMCB(MCB *mcb) { this->mcb = mcb; }
};

//----------------------------------------
//---- M E M B E R  F U N C T I O N S ----
//----------------------------------------

//Constructor - The Ufs constructor should locate a datafile and a metadata
//              file, if not it will call the format() function so that 
//              both files are created.
Ufs::Ufs(string theFilename, int theNoBlocks, int theBlockSize, char theInitChar)
{
    this->fs_no_blocks = theNoBlocks;
    this->fs_block_size = theBlockSize;
    this->init_char = theInitChar;
    this->next_file_id = 0;

    this->next_handle = 0;
    this->no_blocks_available = theNoBlocks;
    this->fs_id = 0;

    inodes = new inode[theNoBlocks];

    //I have 2 files: one to keep metadata(inode) information
    //, and one to keep actual data.
    //Filenames: [fs_name].txt - actual data.
    //           meta[fs_name.txt] - metadata information.
    this->fs_name = theFilename + ".txt";
    
    this->fs_metafile_name = "meta"+fs_name;

    fstream metadata(this->fs_metafile_name);
    fstream datafile(this->fs_name);
    //If either of the files does not exist,
    //call format to create them.
    if(metadata.fail() || datafile.fail())
    {        
        metadata.close();
        datafile.close();
        format(); 
    }
    else
    {
        //Files exist, read the metadata file into the 
        //inode[theNoBlocks] array.
        datafile.close();
        metadata.read((char*)inodes,(sizeof(inode) * theNoBlocks));
        metadata.close();

        //Updating to current amount of inodes available.
        for(int i = 0 ; i < theNoBlocks; i++)
            if(inodes[i].owner_task_id != -1)
                no_blocks_available--;
    }
}
//----------------------------------------

//Format() - The format function is an auxiliary function
//           in case of the file system mechanism does not
//           find a metadata file and data file.
void Ufs::format()
{
    //Create inodes*fs_no_blocks(16 inodes for ULTIMA).
    for(int i  = 0; i < fs_no_blocks; i++)
    {
        strcpy(inodes[i].filename,"0000000");
        inodes[i].owner_task_id = -1;
        inodes[i].starting_block = i;
        inodes[i].size = 0;
        strcpy(inodes[i].permission,"0000");
        inodes[i].next_block = -1;
        inodes[i].blocks[0] = 
        inodes[i].blocks[1] = 
        inodes[i].blocks[2] = 
        inodes[i].blocks[3] = -1;
        inodes[i].handle = next_handle;

    }

    //After creating 16 inodes, store them into a metadata file.
    ofstream metadata(fs_metafile_name);

    metadata.write((char*)inodes,(sizeof(inode) * fs_no_blocks));
    metadata.close();

    //Now creating datafile containing init character.
    ofstream datafile(fs_name);
    for(int i = 0; i < (fs_no_blocks * fs_block_size); i++)
        datafile << init_char;

    datafile.close();
}
//----------------------------------------

//Open() - Open a file by checking some constraints such
//         ownership, permission, and mode.
//         The opening of a file is done by inserting a
//         node in a linked list of opened files.
int Ufs::Open(int task_id, int file_handle, string filename, OPEN_MODE mode)
{   
    mcb->mysema3->down(task_id);
    for(int i = 0; i < fs_no_blocks ; i++)
    {
        //If there is a filename.
        if(strcmp(inodes[i].filename,filename.c_str()) == 0)
        {
            
            //Check ownership, and check if file handle is valid.
            if(inodes[i].owner_task_id == task_id && inodes[i].handle == file_handle)
            {
                //If owner, check if user read permission is set.
                if((mode == READ) && (inodes[i].permission[0] == '0'))
                {
                    sprintf(b, " Invalid, owner cannot read!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return -1;
                }
                else if((mode == WRITE) && (inodes[i].permission[1] == '0')) //Check user write.
                {

                    sprintf(b, " Invalid, owner cannot write!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return -1;
                }
            }
            else
            {
                //If owner, check if user read permission is set.
                if((mode == READ) && (inodes[i].permission[2] == '0'))
                {
                    sprintf(b, " Invalid, others cannot read!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return -1;
                }
                else if((mode == WRITE) && (inodes[i].permission[3] == '0')) //Check user write.
                {

                    sprintf(b, " Invalid, others cannot write!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return -1;
                }
            }

            //If file is ok, then open it by creating a new node
            //a linked list of opened files.
            openedFile newOpenedFile;
            strcpy(newOpenedFile.filename,filename.c_str());
            strcpy(newOpenedFile.permission, inodes[i].permission);
            newOpenedFile.mode = mode;
            newOpenedFile.fileID = next_file_id;
            newOpenedFile.ownerID = inodes[i].owner_task_id;
            newOpenedFile.opened_by_id = task_id;
            newOpenedFile.read_cursor = 0;
            newOpenedFile.write_cursor = 0;

            opened_files.insertBack(newOpenedFile,next_file_id);

            next_file_id++;
            mcb->mysema3->up();
            return newOpenedFile.fileID;
        }
    }

    //If no filename found at all, return -1 for error.
    sprintf(b, " Invalid filename!\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
    mcb->mysema3->up();
    return -1;
}
//----------------------------------------

//Close() - Goes through the linked list of opened files
//          check if there are such files or if the task
//          in which is trying to close a file is the one
//          that opened. If those constraints apply then
//          remove from the opened files list and return
//          1, or if not return -1 for error.

int Ufs::Close(int task_id, int file_id)
{
    mcb->mysema3->down(task_id);
    if(!opened_files.isNode(file_id)) //Check if there is such opened file.
    {
        sprintf(b, " Invalid filename or file not open!\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
        mcb->mysema3->up();
        return -1;
    }
    else
    {
        Node<openedFile> *tmp = opened_files.searchNode(file_id);
        if(tmp->data.opened_by_id != task_id) //Check if the task trying to close if who opened.
        {
            sprintf(b, " Task does not own file!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            mcb->mysema3->up();
            return -1;
        }
        else
        {
            sprintf(b," File closed!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            opened_files.removeNode(file_id);
            mcb->mysema3->up();
            return 1;
        } 
    }
}
//----------------------------------------

int Ufs::Read_char(int task_id, int file_id, char *ch)
{
    mcb->mysema3->down(task_id);
    char c;
    //Check if the file is open
    if(!opened_files.isNode(file_id))
    {
        //If close.
        sprintf(b, " File not open!\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
        mcb->mysema3->up();
        return -1;
    }
    else
    {
        //If open.
        Node<openedFile> *tmp = opened_files.searchNode(file_id);
        
        //Check if the mode in which the file is were
        //opened, is correct.
        if(tmp->data.mode != READ)
        {
            sprintf(b, " File not open for READ!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            mcb->mysema3->up();
            return -1;
        }
        //Check if who opened is who wants to read.
        if(tmp->data.opened_by_id != task_id)
        {
            sprintf(b, " File not opened by task id!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            mcb->mysema3->up();
            return -1;
        }

        //Now lets try to read.

        for(int i = 0; i < fs_no_blocks; i++)
        {
            //Look through the inodes for my file.
            if(strcmp(tmp->data.filename, inodes[i].filename) == 0)
            {
                if(tmp->data.read_cursor < fs_block_size && tmp->data.read_cursor <= inodes[i].size) //If where I am reading is in the first block.
                {
                    fstream datafile(fs_name);
                    datafile.seekp(tmp->data.read_cursor + (inodes[i].starting_block * fs_block_size));
                    datafile.get(c);
                    datafile.close();
                    sprintf(b, " Char: %c \n",c);
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    tmp->data.read_cursor = tmp->data.read_cursor + 1 ;
                    mcb->mysema3->up();
                    return 1;
                }
                else if(tmp->data.read_cursor > inodes[i].size) //If where I am reading is out of my filesize.
                {
                    //If I am trying to read outside of my filesize boundaries
                    //I just set the cursor
                    tmp->data.read_cursor = 0;
                    sprintf(b, " Read char outside of boundaries!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return -1;
                }else   // If where I am reading is in another block.
                {
                    //Find corresponding block.
                    int no_blocks_needed = inodes[i].size % fs_block_size;
                    int block_needed = inodes[i].blocks[no_blocks_needed-1];
                    fstream datafile(fs_name);
                    datafile.seekp(block_needed * fs_block_size);
                    datafile.get(c);
                    datafile.close();
                    sprintf(b, " Char: %c \n",c);
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    tmp->data.read_cursor = tmp->data.read_cursor + 1 ;
                    mcb->mysema3->up();
                    return 1;
                }
            }   
        }
    } 
    mcb->mysema3->up();
    return -1;
}
//----------------------------------------

int Ufs::Write_char(int task_id, int file_id, char ch)
{
    mcb->mysema3->down(task_id);
    //Check if the file is open
    if(!opened_files.isNode(file_id))
    {
        //If close.
        sprintf(b, " File not open!\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
        mcb->mysema3->up();
        return -1;
    }
    else
    {
        //If open.
        Node<openedFile> *tmp = opened_files.searchNode(file_id);
        
        //Check if the mode in which the file is were
        //opened, is correct.
        if(tmp->data.mode != WRITE)
        {
            sprintf(b, " File not open for WRITE!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            mcb->mysema3->up();
            return -1;
        }
        //Check if who opened is who wants to read.
        if(tmp->data.opened_by_id != task_id)
        {
            sprintf(b, " File not opened by task id!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            mcb->mysema3->up();
            return -1;
        }

        //Now lets try to read.

        for(int i = 0; i < fs_no_blocks; i++)
        {
            //Look through the inodes for my file.
            if(strcmp(tmp->data.filename, inodes[i].filename) == 0)
            {
                if(tmp->data.write_cursor < fs_block_size && tmp->data.write_cursor <= inodes[i].size) //If where I am reading is in the first block.
                {
                    fstream datafile(fs_name);
                    datafile.seekp(tmp->data.write_cursor + (inodes[i].starting_block * fs_block_size));
                    datafile.put(ch);
                    datafile.close();
                    tmp->data.write_cursor = tmp->data.write_cursor + 1 ;
                    mcb->mysema3->up();
                    return 1;
                }
                else if(tmp->data.write_cursor > inodes[i].size) //If where I am reading is out of my filesize.
                {
                    //If I am trying to read outside of my filesize boundaries
                    //I just set the cursor
                    tmp->data.write_cursor = 0;
                    sprintf(b, " Read char outside of boundaries!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return -1;
                }else   // If where I am reading is in another block.
                {
                    //Find corresponding block.
                    int no_blocks_needed = inodes[i].size % fs_block_size;
                    int block_needed = inodes[i].blocks[no_blocks_needed-1];
                    fstream datafile(fs_name);
                    datafile.seekp(block_needed * fs_block_size);
                    datafile.put(ch);
                    datafile.close();
                    tmp->data.write_cursor = tmp->data.write_cursor + 1 ;
                    mcb->mysema3->up();
                    return 1;
                }
            }   
        }
    }
    mcb->mysema3->up();
    return -1;
}

//----------------------------------------

//Create_file() - The function create file, creates a file according
//                to the constraints passed as parameter.
//                Moreover, it checks if file already exist,
//                if file is bigger than space we got, and
//                if filename is acceptable size wise.
int Ufs::Create_file(int task_id, string filename, int file_size, string permission)
{
    mcb->mysema3->down(task_id);
    //Check if there is such file already.
    for(int i = 0 ; i < fs_no_blocks; i++)
    {
        if(task_id == inodes[i].owner_task_id && strcmp(inodes[i].filename,filename.c_str()) == 0)
        {
            sprintf(b, " File already exists!\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            mcb->mysema3->up();
            return -1;
        }
    }
    
    if(file_size < 0 || file_size > MAX_FILE_SIZE ) //Check if file size is valid.
    {
        sprintf(b, " Invalid file size!\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
        mcb->mysema3->up();
        return -1;
    }
    else if(strlen(filename.c_str()) > 8)
    {
        sprintf(b, " Invalid file name!\n");
        mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
        mcb->mysema3->up();
        return -1;
    }
    else
    {   //If file size only requires one data block.
        if(file_size < fs_block_size && no_blocks_available > 0)
        {
            for(int i = 0; i < fs_no_blocks; i++)
            {
                if(inodes[i].owner_task_id == -1)
                {
                    strcpy(inodes[i].filename,filename.c_str());
                    inodes[i].owner_task_id = task_id;
                    inodes[i].starting_block = i;
                    inodes[i].size = file_size;
                    strcpy(inodes[i].permission, permission.c_str());
                    inodes[i].next_block = -1;
                    inodes[i].blocks[0] = i;
                    inodes[i].handle = next_handle;
                    
                    no_blocks_available--;
                    next_handle++;

                    //Store to metadafile in right location.
                    fstream metadata(fs_metafile_name);
                    //Go to correct inode location.
                    metadata.seekp(sizeof(inode) * i);
                    //Write on that inode location the new information.
                    metadata.write((char *)&(inodes[i]),sizeof(inode));
                    metadata.close();

                    sprintf(b, " File created with success!\n");
                    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                    mcb->mysema3->up();
                    return inodes[i].handle;
                }
            }
        }
        else
        {
            //If file size requires more than one data block due to file size > 128.
            int no_blocks_needed = file_size / fs_block_size;
            
            //If not enough space.
            if(no_blocks_available < no_blocks_needed)
            {
                sprintf(b, " Not enough enough space!\n");
                mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                mcb->mysema3->up();
                return -1;
            }
            else
            {
                int count = 0;
                inode *previous_inode_block;
                for(int i = 0; i < fs_no_blocks; i++)
                {
                    if(inodes[i].owner_task_id == -1)
                    {
                        //Store to metadafile in right location.
                        fstream metadata(fs_metafile_name);

                        strcpy(inodes[i].filename,filename.c_str());
                        inodes[i].owner_task_id = task_id;
                        inodes[i].starting_block = i;
                        inodes[i].size = file_size;
                        inodes[i].handle = next_handle;
                        strcpy(inodes[i].permission, permission.c_str());
                        if(count == 0)
                        {
                            //Stores the first inode of the file
                            previous_inode_block=&inodes[i];
                        }
                        else if(count > 0)
                        {
                            //Goes back to previous inode,
                            //and assign the next_block value
                            //and the used blocks array.
                            previous_inode_block->next_block = i;
                            previous_inode_block->blocks[count] = i;
                            inodes[i].blocks[count-1] = previous_inode_block->blocks[count-1];

                            //Go to previous inode location to assign new next_block value;
                            metadata.seekp(sizeof(inode) * previous_inode_block->starting_block);
                            //Write on that inode location the new information.
                            metadata.write((char *)&(inodes[previous_inode_block->starting_block]),sizeof(inode));

                            //Now current inode will be saved
                            //so that previous inode next_block
                            //is set pointing to current inode created.
                            previous_inode_block = &inodes[i];
                        }
                        
                        inodes[i].blocks[count] = i; //Sets block being used.
                        no_blocks_available--;
                    
                        //Go to correct inode location.
                        metadata.seekp(sizeof(inode) * i);
                        //Write on that inode location the new information.
                        metadata.write((char *)&(inodes[i]),sizeof(inode));
                        metadata.close();

                        //If is the last block needed for
                        //the file, just set the next_block 
                        //to -1 meaning there is not a next
                        //one.
                        if(count == no_blocks_needed)
                        {
                            next_handle++;
                            inodes[i].next_block = -1;
                            sprintf(b, " File created with success!\n");
                            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
                            mcb->mysema3->up();
                            return inodes[i].handle;
                        }
                        count++;
                    }
                }
            }
        }   
    }
}
//----------------------------------------


int Ufs::Del_file(int task_id, string filename)
{
    mcb->mysema3->down(task_id);
    int result = -1;
    for(int i = 0 ; i < fs_no_blocks; i ++)
    {
        if(inodes[i].owner_task_id == task_id && strcmp(inodes[i].filename,filename.c_str()) == 0)
        {
            
            fstream datafile(fs_name);
            datafile.seekp(inodes[i].starting_block * fs_block_size);   
            for(int j = 0; j < fs_block_size; j++)
                datafile << '$';
            
            datafile.close();

            strcpy(inodes[i].filename,"0000000");
            inodes[i].owner_task_id = -1;
            inodes[i].starting_block = i;
            inodes[i].size = 0;
            strcpy(inodes[i].permission,"0000");
            inodes[i].next_block = -1;
            inodes[i].blocks[0] = 
            inodes[i].blocks[1] = 
            inodes[i].blocks[2] = 
            inodes[i].blocks[3] = -1;
            inodes[i].handle = next_handle;

            //Store to metadafile in right location.
            fstream metadata(fs_metafile_name);
            //Go to correct inode location.
            metadata.seekp(sizeof(inode) * i);
            //Write on that inode location the new information.
            metadata.write((char *)&(inodes[i]),sizeof(inode));
            metadata.close();

            result = 1;
        }
    }
    mcb->mysema3->up();
    return result;
}
//----------------------------------------

int Ufs::Change_permission(int task_id,string filename,string new_permission)
{
    mcb->mysema3->down(task_id);
    int result = -1;
    //Update all corresponding inodes
    for(int i = 0; i < fs_no_blocks; i++)
    {
        if(inodes[i].owner_task_id == task_id && strcmp(inodes[i].filename,filename.c_str()) == 0)
        {    
            strcpy(inodes[i].permission, new_permission.c_str());

            fstream metadata(fs_metafile_name);
            //Go to correct inode location.
            metadata.seekp(sizeof(inode) * i);
            //Write on that inode location the new information.
            metadata.write((char *)&(inodes[i]),sizeof(inode));
            metadata.close();
            result = 1;
        }
    }
    Node <openedFile> *tmp = opened_files.front();
    while(tmp != NULL)
    {
        if(tmp->data.ownerID == task_id && strcmp(tmp->data.filename, filename.c_str()) == 0)
        {
            strcpy(tmp->data.permission, new_permission.c_str());
        }
        tmp = tmp->next;
    }
    mcb->mysema3->up();
    return result;
}
//----------------------------------------

//Dir() - Show all everyone's file name and permissions.
void Ufs::Dir()
{
    sprintf(b, " -----------\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
    for(int i = 0; i < fs_no_blocks; i++)
    {
        if(inodes[i].owner_task_id != -1)
        {    
            sprintf(b, " Filename: %s\n", inodes[i].filename);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Owner ID: %d\n", inodes[i].owner_task_id);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Starting Block: %d\n", inodes[i].starting_block);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " File size: %d\n" , inodes[i].size);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Permission: ");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[0] == '1')
                sprintf(b, "r");
            else if(inodes[i].permission[0] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[1] == '1')
                sprintf(b, "w");
            else if(inodes[i].permission[1] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, "x");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[2] == '1')
                sprintf(b, "r");
            else if(inodes[i].permission[2] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[3] == '1')
                sprintf(b, "w");
            else if(inodes[i].permission[3] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, "\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Next block: %d\n", inodes[i].next_block);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Used blocks: %d | %d | %d | %d \n", inodes[i].blocks[0], inodes[i].blocks[1], inodes[i].blocks[2], inodes[i].blocks[3]);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Handle: %d\n",inodes[i].handle);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " -----------\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
        }
    }
}


//Dir() - Show file belonging to an specific task id.
//        The function should break out of the loop
//        when the first is print by calling "break;".
//        OVERLOADED.
void Ufs::Dir(int task_id)
{
    mcb->mysema3->down(task_id);
    sprintf(b, " -----------\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
    for(int i = 0; i < fs_no_blocks; i++)
    {
        if(inodes[i].owner_task_id == task_id)
        {    
            sprintf(b, " Filename: %s\n", inodes[i].filename);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Owner ID: %d\n", inodes[i].owner_task_id);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Starting Block: %d\n", inodes[i].starting_block);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " File size: %d\n" , inodes[i].size);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Permission: ");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[0] == '1')
                sprintf(b, "r");
            else if(inodes[i].permission[0] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[1] == '1')
                sprintf(b, "w");
            else if(inodes[i].permission[1] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, "x");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[2] == '1')
                sprintf(b, "r");
            else if(inodes[i].permission[2] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            if(inodes[i].permission[3] == '1')
                sprintf(b, "w");
            else if(inodes[i].permission[3] == '0')
                sprintf(b, "-");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, "\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Next block: %d\n", inodes[i].next_block);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Used blocks: %d | %d | %d | %d \n", inodes[i].blocks[0], inodes[i].blocks[1], inodes[i].blocks[2], inodes[i].blocks[3]);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " Handle: %d\n",inodes[i].handle);
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);
            sprintf(b, " -----------\n");
            mcb->mywin->write_window(mcb->mywin->Misc_Table,b);

            break;
        }
    }
    mcb->mysema3->up();
}

//Dump() - Dump datafile content.
void Ufs::dump()
{
    ifstream datafile(fs_name);
    char mydata[(fs_no_blocks*fs_block_size)+1];
    datafile.read((char*)mydata,(fs_no_blocks * fs_block_size));
    strcat(mydata,"\n");
    mcb->mywin->write_window(mcb->mywin->Misc_Table, mydata);
    datafile.close();
}


#endif