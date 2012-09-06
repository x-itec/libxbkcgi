#include<string>
#include<stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/resource.h>

#define DEBUGME
#define QS
#include <xbk/xbkdebug.c>
#include <xbk/xbkqs.c>
#include <xbk/xbkio.h>

int main()
{
EDEBUG

xbkio io;

io.load("test_2_democonfig.txt");

string parameter1=io.getparam("parameter1");
cout << "Parameter1 = " << parameter1;
cout <<io.file_content;

}
