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


using namespace std;

int main (int argc , char * argv[]){
    const string SUM  = "sum";
    const string MIN = "min";
    const string MAX = "max";
    const string SOS = "sos";

    long signed int sum = 0;
    ifstream nums;
    nums.open("nums.csv");
    if (!nums.is_open()){
        cout<< "File not found"<< endl;
        exit(-1);
    }

    if (argv[1] == SUM){
        while (!nums.eof()){
            int temp;
            nums >> temp;
            sum += temp;

        }

    }

    cout<< sum << endl;


    return 42;
}