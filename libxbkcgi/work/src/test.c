#include<string>
#include<stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <pthread.h>

#define DEBUGME
#define QS
#include <xbk/xbkdebug.c>
#include "xqs.c"
#include "xstring.cpp"
#include "xdbq.cpp"
#include "xcgi.cpp"

/* 
global:
pthread_mutex_t mutex;

dann init:
pthread_mutex_init(&mutex,NULL);

ein Thread muss bei kritischen Bereichen dann nur
pthread_mutex_lock(&mutex);
und unlock:
pthread_mutex_unlock(&mutex);
benutzen
*/



void* commloop(void *arg)
{
EDEBUG

xcgi_commandparser_base k;


xcgi_accept_data *x;
x=(xcgi_accept_data*) arg;
DEBUG("Transmitter-FD ist %i",x->fd_conn);
xcgi_transmitter_base_tcp *transmitter;
transmitter = new xcgi_transmitter_base_tcp4;
transmitter->set_fd(x->fd_conn);
xcgi_out_base cgi(transmitter);
transmitter->settimeout(60);
try
{ 
 for(;;)
 {
 DEBUG("LOOP");
 k = cgi.readcommand();
 k.processparameters();
 cgi.sendline("command received.");
 }
}
catch (...)
 {
 DEBUG("Exception");
 close(x->fd_conn);//Verbindung mit Client schliessen
 }
}

int main()
{

string sep,suchtext;
vector <string> ergebnis;
EDEBUG

xcgi_transmitter_base_tcp *transmitter;
transmitter = new xcgi_transmitter_base_tcp4;
transmitter->conn_ip="192.168.0.99";
transmitter->conn_port=91;

xcgi_out_base cgi(transmitter);
DEBUG("BIND");
transmitter->bind();
DEBUG("LISTEN");
transmitter->listen();

for(;;)
{
DEBUG("ACCEPT");
xcgi_accept_data *daten;
xcgi_accept_data da2;
da2.fd_conn=5;
daten=transmitter->accept();
DEBUG("ACCEPT wurde aufgerufen");
transmitter->settimeout(5);
DEBUG("FD der Daten: %i",daten->fd_conn);
pthread_t tid;
pthread_create(&tid,NULL,&commloop,daten);
DEBUG("Thread erzeugt.");
};

cout << "\nVerbindung von: ";
cout <<transmitter->remote_ip;

transmitter->settimeout(5);

try
{
for(;;)
{
string zeile = cgi.readline();
cout << zeile; 
cgi.sendline(zeile);

}
}
catch(...)
{
cout << "Timeout";
exit(0);
}

/*

xcgi_transmitter_base *transmitter;
transmitter = new xcgi_transmitter_www;
transmitter->setcontenttype("text/html");
xcgi_out_base cgi(transmitter);
cgi.send("Hallo Leute");

*/

/*
PGSQL-Tests
xbkdbq_base* dbq;
dbq = new xbkdbq_pgsql;

string d_user,d_pass,d_db;
d_user="cmsuser";
d_pass="cmspass";
d_db="CMSDB";
dbq->username(d_user);
dbq->password(d_pass);
dbq->port("5432");
dbq->dbname(d_db);
dbq->connect();

if(dbq->connstate==xbkdbq_pgsql::cBAD){puts("BAD");}

dbq->query("select * from messung");
int ad=dbq->num();
DEBUG("Anzahl Datensaetze: %i",ad);
while(dbq->eof())
{
string ua=dbq->value("datum");
//DEBUG("Line: %i Useragent: %s",dbq->currentline,ua.c_str());
dbq->next();
}

exit(0);

suchtext="Var1=Two&name=Odysseus&address=Calypso%27s+island"; 
string erg=URLDecode(suchtext);
cout <<erg << "\n";
erg=URLEncode(erg);
cout <<erg << "\n"; 
DDEBUG
*/

/*

string text;
int x=0;
timeval zeit;
gettimeofday(&zeit, NULL);
int startzeit = zeit.tv_sec;
text ="\"Hallo Leute\" \'";
puts("start 1");
for(x=0;x<23;x++)
{
puts("startadd");
text=addslashes(text);
printf("x %i strlen %i\n",x,text.length());
}
gettimeofday(&zeit,NULL);
int endzeit = zeit.tv_sec;

//den erstellen Text jetzt entstrippen
string t2=text;

puts("destrip");
text=stripslashes(text);
puts("done");
puts("destrip 2");
char *irgendwas;
printf("Ergebnis C++ %s\n",text.c_str());
//printf("Ergebnis Orig: %s\n",irgendwas);
*/
}
