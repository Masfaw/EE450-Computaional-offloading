//
// Created by Matheos Asfaw on 10/21/16.
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
#define MAX_CLIENTS 2
#define MAX_DATA 8000
#define A_PORT_NUM 21624
#define B_PORT_NUM 22624
#define C_PORT_NUM 23624
#define AWS_UDP 24624
#define AWS_TCP 25624


using namespace std;


int main (int argc , char * argv[]){

    const string SUM  = "sum";
    const string MIN  = "min";
    const string MAX  = "max";
    const string SOS  = "sos";

    int UDP_sock,TCP_sock, clientSock,dataLength, length= sizeof(struct sockaddr_in) ;
    struct sockaddr_in awsUDP, awsTCP, TCP_client,serverA,serverB,serverC, receiver;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    struct hostent * hp;
    char command [MAX_DATA];
    string comndString;


    long signed int numsArray [MAX_DATA];
    long signed int resultFromA, resultFromB, resultFromC,incoming,  resultFromAWS = 0;
    vector<long signed int> numsForA,numsForB,numsForC,results;
    vector<long signed int>::iterator itter;


    // Set up the UDP port

    UDP_sock = socket(AF_INET,SOCK_DGRAM,0);

    if (UDP_sock < 0){
        cout<< "UDP Socket failed" << endl;
        perror("Socket ");
        exit(-1);
    }

    //These following code was influenced by tutorials on youtube.
    memset(&awsUDP,0,length);
    awsUDP.sin_family= AF_INET;
    awsUDP.sin_port = htons(AWS_UDP);
    awsUDP.sin_addr.s_addr = INADDR_ANY;
    //memset(awsUDP.sin_zero,'\0', sizeof(awsUDP.sin_zero));

    // bind  the socket
    if ((::bind (UDP_sock,(struct sockaddr * ) &awsUDP, sizeof(awsUDP))) < 0 ){

        cout <<"cant bind UPD socket"<< endl;
        perror("TCP socket ");
        exit (-1);
    }

    // Set up the TCP port

    TCP_sock = socket(AF_INET,SOCK_STREAM,0);

    if (TCP_sock < 0){
        cout<< "TCP Socket failed" << endl;
        perror("Socket ");
        exit(-1);
    }

    memset(&awsTCP,0,length);
    awsTCP.sin_family= AF_INET;
    awsTCP.sin_port = htons(AWS_TCP);
    awsTCP.sin_addr.s_addr = INADDR_ANY;
    memset(awsTCP.sin_zero,'\0', sizeof(awsUDP.sin_zero));

    // bind  the socket
    if ((::bind (TCP_sock,(struct sockaddr * ) &awsTCP, sizeof(awsTCP))) < 0 ){

        cout <<"cant bind TCP socket"<< endl;
        perror("Socket Bindng ");
        exit (-1);
    }



    // set up ServerA connection
    memset(&serverA,0,length);
    serverA.sin_family= AF_INET;
    serverA.sin_port = htons(A_PORT_NUM);
    serverA.sin_addr.s_addr = INADDR_ANY;

    // set up ServerB connection
    memset(&serverB,0,length);
    serverB.sin_family= AF_INET;
    serverB.sin_port = htons(B_PORT_NUM);
    serverB.sin_addr.s_addr = INADDR_ANY;

    // set up ServerC connection
    memset(&serverC,0,length);
    serverC.sin_family= AF_INET;
    serverC.sin_port = htons(C_PORT_NUM);
    serverC.sin_addr.s_addr = INADDR_ANY;


    // set up receiver connection
    memset(&receiver,0,length);



    cout<< "The AWS is up and running."<< endl;


    if (listen(TCP_sock,MAX_CLIENTS) == ERROR){
        perror("listen");
        exit(-1);
    }

    while (1) {
        ///connecting to a client

        clientSock = accept(TCP_sock,(struct sockaddr * ) &TCP_client, &addrlen);


        if (clientSock== ERROR){
            perror("accept");
            exit(-1);
        }

        dataLength = recv(clientSock,command,MAX_DATA,0);
        if (dataLength < 0 ){
            cout << "failed to get commnad " << endl;
            perror("recv ");
            exit(-1);
        }

        cout << "The command received is " << command << endl;


        dataLength = recv(clientSock,numsArray,MAX_DATA,0);
        if (dataLength < 0 ){
            cout << "failed to get commnad " << endl;
            perror("recv ");
            exit(-1);
        }

        cout<< "Has recived numbers  "<< endl;

        /// dividing up the work into 3 parts

        int numberOfElmts = dataLength / sizeof(long signed int);

        cout<< "The AWS has received "<< numberOfElmts << " numbers from the client using TCP over port "<< ntohs(awsTCP.sin_port)<< endl;


        long signed int numsToA[numberOfElmts / 3];
        long signed int numsToB[numberOfElmts / 3];
        long signed int numsToC[numberOfElmts / 3];



        for (int i = 0; i < numberOfElmts; i++) {
            if (i < numberOfElmts / 3) {
                numsForA.push_back(numsArray[i]);
            } else if (i >= numberOfElmts / 3 && i < 2 * numberOfElmts / 3) {
                numsForB.push_back(numsArray[i]);
            } else if (i >= 2 * numberOfElmts / 3 && i < numberOfElmts) {
                numsForC.push_back(numsArray[i]);
            }
        }

        copy(numsForA.begin(), numsForA.end(), numsToA);
        copy(numsForB.begin(), numsForB.end(), numsToB);
        copy(numsForC.begin(), numsForC.end(), numsToC);





        ///sending Comnnand and Data to servers

        /// Server A

        dataLength = sendto(UDP_sock, command, sizeof(command), 0, (struct sockaddr *) &serverA, sizeof(serverA));

        if (dataLength < 0) {
            cout << "failed to send command to A " << endl;
            perror("Send command to A ");
            exit(-1);
        }

        dataLength = sendto(UDP_sock, numsToA, sizeof(numsToA), 0, (struct sockaddr *) &serverA, sizeof(serverA));

        if (dataLength < 0) {
            cout << "failed to send numbers" << endl;
            perror("Send nums to A ");
            exit(-1);
        }

        cout<< "The AWS has sent "<< numsForA.size()<< " numbers to Backend-server A"<<endl;








        /// Server B
        dataLength = sendto(UDP_sock, command, sizeof(command), 0, (struct sockaddr *) &serverB, sizeof(serverB));

        if (dataLength < 0) {
            cout << "failed to send command to A " << endl;
            perror("Send command to B  ");
            exit(-1);
        }

        dataLength = sendto(UDP_sock, numsToB, sizeof(numsToB), 0, (struct sockaddr *) &serverB, sizeof(serverB));

        if (dataLength < 0) {
            cout << "failed to send numbers" << endl;
            perror("Send nums to B  ");
            exit(-1);
        }

        cout<< "The AWS has sent "<< numsForB.size()<< " numbers to Backend-server B"<<endl;






        ///Server C
        dataLength = sendto(UDP_sock, command, sizeof(command), 0, (struct sockaddr *) &serverC, sizeof(serverC));

        if (dataLength < 0) {
            cout << "failed to send command to A " << endl;
            perror("Send command to C ");
            exit(-1);
        }

        dataLength = sendto(UDP_sock, numsToC, sizeof(numsToC), 0, (struct sockaddr *) &serverC, sizeof(serverC));

        if (dataLength < 0) {
            cout << "failed to send numbers" << endl;
            perror("Send nums to C ");
            exit(-1);
        }

        cout<< "The AWS has sent "<< numsForB.size()<< " numbers to Backend-server C"<<endl;





        /// Receiving results from servers



        dataLength = (recvfrom(UDP_sock, &incoming, MAX_DATA, 0, (struct sockaddr *) &receiver, &addrlen));
        if (dataLength < 0) {
            cout << "failed to get the result from A " << endl;
            perror("recvFrom ");
            exit(-1);
        }



        if (receiver.sin_port == serverA.sin_port){
           resultFromA = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server A using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromA<<endl;

        }else if (receiver.sin_port == serverB.sin_port){
            resultFromB = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server B using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromB<<endl;

        }else if (receiver.sin_port == serverC.sin_port){
            resultFromC = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server C using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromC<<endl;

        }




        dataLength = (recvfrom(UDP_sock, &incoming, MAX_DATA, 0, (struct sockaddr *) &receiver, &addrlen));
        if (dataLength < 0) {
            cout << "failed to get the result from B " << endl;
            perror("recvFrom ");
            exit(-1);
        }



        if (receiver.sin_port == serverA.sin_port){
            resultFromA = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server A using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromA<<endl;

        }else if (receiver.sin_port == serverB.sin_port){
            resultFromB = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server B using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromB<<endl;

        }else if (receiver.sin_port == serverC.sin_port){
            resultFromC = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server C using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromC<<endl;

        }




        dataLength = (recvfrom(UDP_sock, &incoming, MAX_DATA, 0, (struct sockaddr *) &receiver, &addrlen));
        if (dataLength < 0) {
            cout << "failed to get the result from C " << endl;
            perror("recvFrom ");
            exit(-1);
        }




        if (receiver.sin_port == serverA.sin_port){
            resultFromA = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server A using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromA<<endl;

        }else if (receiver.sin_port == serverB.sin_port){
            resultFromB = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server B using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromB<<endl;

        }else if (receiver.sin_port == serverC.sin_port){
            resultFromC = incoming;
            cout<< "The AWS has received reduction result of "<< command<< " form Backend-Server C using UDP port "
                << ntohs(awsUDP.sin_port)<<" and it is "<< resultFromC<<endl;

        }




        results.push_back(resultFromA);
        results.push_back(resultFromB);
        results.push_back(resultFromC);

        comndString = command;
        // calculating the result based on the command given.
        if (comndString.compare (SUM) == 0 ){
            for (itter = results.begin(); itter != results.end(); itter++){
                resultFromAWS += *itter;
            }
        }
        else if (comndString.compare(SOS) == 0){
            for (itter = results.begin(); itter != results.end(); itter++){
                resultFromAWS += (*itter);
            }
        }
        else if (comndString.compare(MIN) == 0){
            resultFromAWS = *(min_element(results.begin(),results.end()));
        }
        else if (comndString.compare(MAX) == 0 ){
            resultFromAWS = *(max_element(results.begin(),results.end()));
        }

        cout << "The AWS has successfully finished the reduction " << comndString << ": "<<resultFromAWS << endl;



        dataLength =send(clientSock,&resultFromAWS, sizeof(resultFromAWS),0);
        if (dataLength <=0){
            cout << "result not sent to client"<< endl;
            perror("send");
            exit(-1);
        }

        cout << "The AWS has successfully finished sending the reduction value to client " <<endl;

        close(clientSock);

        numsForA.erase(numsForA.begin(), numsForA.end());
        numsForB.erase(numsForB.begin(), numsForB.end());
        numsForC.erase(numsForC.begin(), numsForC.end());
        memset(numsToA,0, sizeof(numsToA));
        memset(numsToB,0, sizeof(numsToB));
        memset(numsToC,0, sizeof(numsToC));
        memset(&numberOfElmts , 0 , sizeof(numberOfElmts));
        memset(command,0, sizeof(command));
        memset(numsArray,0, sizeof(numsArray));
        memset(&resultFromAWS,0, sizeof(resultFromAWS));
        memset(&resultFromA,0, sizeof(resultFromA));
        memset(&resultFromB,0, sizeof(resultFromB));
        memset(&resultFromC,0, sizeof(resultFromC));
        resultFromA = 0;
        resultFromAWS = 0;
        resultFromB = 0;
        resultFromC = 0;
        results.erase(results.begin(),results.end());



    }




    return 42;
}
