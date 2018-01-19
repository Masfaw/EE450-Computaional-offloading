//
// Created by Matheos Asfaw on 10/20/16.
//


#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <stdio.h>
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



#define ERROR -1
#define MAX_DATA 8000
#define AWS_TCP 25624

using namespace std;

int main (int argc , char * argv[]){


    const string SUM  = "sum";
    const string MIN  = "min";
    const string MAX  = "max";
    const string SOS  = "sos";

    long signed int result = 0;
    vector<long signed int> nums;
    vector<long signed int>:: iterator itter;
    char command [MAX_DATA];
    string comnd;

    if (argv[1] != NULL){
        comnd = argv[1];
    }
    else {
        cout << "Please enter a valid command."<<endl;
        cout<< "Valid comands are "<<endl;
        cout << "sum , max, min , sos" <<endl;
        exit (-1);
    }

    if ( comnd != "sum" && comnd != "sos" && comnd != "max" && comnd != "min"){
        cout << "Please enter a valid command."<<endl;
        cout<< "Valid comands are "<<endl;
        cout << "sum , max, min , sos" <<endl;
        exit (-1);
    }






    int sock, datalength, finalResult,length= sizeof(struct sockaddr_in) ;
    struct sockaddr_in AWS_server;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int count = 0;


    // open file and check if it works
    ifstream inFile;
    inFile.open("nums.csv");

    if (!inFile.is_open()){
        cout<< "File not found"<< endl;
        exit(-1);
    }




    // import all the nubmers and add them to the vector

    while (!inFile.eof()){
        long signed int temp;
        inFile >> temp;
        nums.push_back(temp);
        count++;
    }

    //close file

    inFile.close();

    // create array with the size of the vector
    long signed int numsArray [nums.size()];
    copy(nums.begin(),nums.end(),numsArray);


    // Get the socket
    sock = socket (AF_INET,SOCK_STREAM,0);
    if (sock <0){
        cout<< "failed to get a socket"<< endl;
        perror("socket");
        exit(-1);
    }

    memset(&AWS_server,0,length);
    AWS_server.sin_family = AF_INET;
    AWS_server.sin_port = htons(AWS_TCP);
    AWS_server.sin_addr.s_addr = INADDR_ANY;//  inet_addr( gethostbyname("nunki.usc.edu"));
    memset(&AWS_server.sin_zero,0,sizeof(AWS_server.sin_zero));



    // connect to the AWS server.
    if (connect(sock,(struct sockaddr *) &AWS_server, sizeof(struct sockaddr_in))== ERROR){
        perror("connect ");
        exit(-1);
    }

    cout<< "The client is up and running"<<endl;

    datalength = send(sock,argv[1],strlen(argv[1]),0);

    if (datalength <=0){
        cout<< "comand not sent"<< endl;
        perror("send reduction");
        exit(-1);
    }

    cout << "The client has sent the reduction type "<< argv[1] << " to AWS"<<endl;


    datalength = send(sock,numsArray, sizeof(numsArray),0);

    if (datalength <=0){
        cout<< "numbers not sent"<< endl;
        perror("send numbes");
        exit(-1);
    }

    cout << "The client has sent "<< nums.size() << " numbers to AWS"<<endl;



    datalength = recv(sock,&finalResult,MAX_DATA,0);
    if (datalength <=0){
        cout<< "faild to receive result"<< endl;
        perror("revc ");
        exit(-1);
    }

    cout<< "The client has received reduction " << argv[1] << ": " << finalResult<< endl;


    close(sock);



    return 42;
}
