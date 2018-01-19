//
// Created by Matheos Asfaw on 10/22/16.
//



#include <cmath>
#include <stdio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <errno.h>

using namespace std;

#define ERROR -1
#define MAX_DATA 8000
#define C_PORT_NUM 23624


int main (int argc , char * argv[]){


    // variables for performing operations
    const string SUM  = "sum";
    const string MIN  = "min";
    const string MAX  = "max";
    const string SOS  = "sos";

    vector<long signed int> nums;
    vector<long signed int>:: iterator itter = nums.begin();
    long signed int result = 0;
    long signed int arrayNums [MAX_DATA];

    char command [MAX_DATA];
    string comndString;


    // variables to set up connection
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sock, dataLength;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int length = sizeof(struct sockaddr_in);


    // Geting a socket for the server
    if ((sock = socket(AF_INET,SOCK_DGRAM,0)) == ERROR){
        perror("serverB socket : ");
        exit(-1);
    }

    //These following code was influenced by tutorials on youtube.

    // filling in the information of the server.
    memset(&server,0,length);
    server.sin_family = AF_INET;
    server.sin_port = htons(C_PORT_NUM);
    server.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");




    //binding the socket
    if ((::bind(sock, (struct sockaddr * )&server,length)) < 0){
        cout<< "bind failed"<< endl;
        perror("bind ");
        exit(-1);
    }


    cout<<"server C is up and running using UDP on port " << ntohs(server.sin_port)<<endl;



    while (1){



        dataLength =(recvfrom(sock,command,MAX_DATA,0,(struct sockaddr* ) &client,&addrlen));
        if (dataLength < 0){
            cout<< "failed to get command"<< endl;
            perror("recvFrom ");
            exit(-1);
        }

        comndString = command;


        dataLength =(recvfrom(sock,arrayNums,MAX_DATA,0,(struct sockaddr* ) &client,&addrlen));

        if (dataLength < 0){
            cout << "failed to get the numbers"<< endl;
            perror("recvFrom ");
            exit(-1);
        }


        int numberOfElmts = dataLength/ sizeof(long signed int);

        cout << "The Server C has received "<< numberOfElmts << " numbers"<<endl;


        // adding the numbers onto a vector for processing.
        for (int i = 0 ; i < numberOfElmts; i++){

            nums.push_back(*(arrayNums +i));
        }

        // calculating the result based on the command given.
        if (comndString.compare (SUM) == 0 ){
            for (itter = nums.begin(); itter != nums.end(); itter++){
                result += *itter;
            }
        }
        else if (comndString.compare(SOS) == 0){
            for (itter = nums.begin(); itter != nums.end(); itter++){
                result += (*itter) * (*itter);
            }
        }
        else if (comndString.compare(MIN) == 0){
            result = *(min_element(nums.begin(),nums.end()));
        }
        else if (comndString.compare(MAX) == 0 ){
            result = *(max_element(nums.begin(),nums.end()));
        }

        cout << "The Server C has successfully finished the reduction " << comndString << ": "<<result << endl;

        dataLength = sendto(sock,&result, sizeof(result),0,(struct sockaddr *) &client, sizeof(client));

        if (dataLength < 0){
            cout << "failed to get the numbers"<< endl;
            perror("recvFrom ");
            exit(-1);
        }

        cout << "The Server C has successfully finished sending the reduction value to AWS server "<< endl;

        // resetting the variables for the next call.
        result= 0;
        nums.erase(nums.begin(),nums.end());
        memset(arrayNums,0, sizeof(arrayNums));

    }


    return 42;
}
