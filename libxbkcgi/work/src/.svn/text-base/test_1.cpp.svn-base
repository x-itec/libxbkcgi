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
#include "xcgiform.cpp"
//#include "xstring.cpp"
#include "xcgi.cpp"

int main()
{
puts("Content-Type: text/html\n");
puts("<HTML><head></head>hallo");
puts("</HTML>");

xcgi_form formular;
EDEBUG
formular.processformdata(stdin,200000);

xcgi_formdata* eingabefeld;

eingabefeld=&formular["textarea"];

if(eingabefeld)
{
cout << "Das Ergebnis ist: " <<eingabefeld->value;
}

cout << "Fuer das Listenfeld select2 wurden ausgewaehlt: ";

int zaehler=formular.count("select2");
cout << zaehler << "\n";

formular.setindexpos("select2");

cout << "Auflistung der Elemente von select2: ";
int x;

for(x=0;x<zaehler;x++)
{
cout << "\nvalue: ";
eingabefeld=formular.getvalue();
cout << eingabefeld->value; 
}

cout << "\n Wenn es eine Datei gibt, abspeichern";
eingabefeld=formular.getvalue("file");
if(eingabefeld)
	{
	cout << "SAVING!!!!";
	formular.savefile("file","testdatei");  
	}

cout << "XXXXXXXXXXXXXXXX";
}
