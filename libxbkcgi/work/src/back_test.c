#include<string>
#include<stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/resource.h>
#define DEBUGME

#ifdef DEBUGME 

static int debugme;

#define EDEBUG debugme=1;
#define DDEBUG debugme=0;

#define DEBUG(format, args...) if(debugme==1){printf("\nDEBUG:%s:%s[%i]:",__FILE__,__FUNCTION__,__LINE__);printf(format, ## args);}
#else  
#define DEBUG(format, args...) {}
#define EDEBUG
#define DDBUG
#endif 


/*
QS: Zeitmessungs-Klasse

*/
#define qs_start() zeit.start();
#define qs_stop(x) zeit.stop(x,__FILE__,__FUNCTION__,__LINE__); 
#define qs_stop() zeit.stop("",__FILE__,__FUNCTION__,__LINE__);

struct qstime_block
{
string current_file,current_function;//GCC Makrofelder
string myvalue;//benutzerdefinierter Inhalt
int current_line;

//Sekunden waeren clock_user/CLK_TCK nicht vergessen
long clock_user;
long clock_system;

//CPU Auslastung
double cpuload[3];//1,5,15 Minuten

rusage ru_start,ru_stop;//nochmal alle Werte von rusage


};

class qstime
{
public:
int m;//Anzahl lokaler Messwerte fuer diese Funktion
qstime();
~qstime();
qstime(string fi,string function,int line);

void start();//Zeitmessung START

void stop();//Zeitmessung STOP und Result auf Vektor
void stop(string x,char* fi,char * fu,int li);
void print();//listnet Ergebnisse auf

private:
rusage ru,t_start,t_stop;

vector <qstime_block> zeitblock;
qstime_block qstime_tovector;

};
qstime::~qstime()
{
print();
puts("DESTRUKTOR!!!");
}
qstime::qstime()
{
m=0;//Messwerte auf 0 setzen
}
void qstime::start()
{
getrusage(0,&ru);
t_start=ru;
qstime_tovector.ru_start=ru;
}

void qstime::stop(string x,char * fi,char * fu,int li)
{
qstime_tovector.current_file=fi;
qstime_tovector.current_function=fu;
qstime_tovector.current_line=li;
stop();
}

void qstime::stop() 
{
getrusage(0,&ru);
t_stop=ru;
qstime_tovector.clock_user=t_stop.ru_utime.tv_usec-t_start.ru_utime.tv_usec;
qstime_tovector.clock_system=t_stop.ru_stime.tv_usec-t_start.ru_stime.tv_usec;
qstime_tovector.ru_stop=ru;

getloadavg(qstime_tovector.cpuload,3);

zeitblock.push_back(qstime_tovector);
m++;
}

void qstime::print()
{
int x;
for(x=0;x<zeitblock.size();x++)
 {
 cout << zeitblock[x].current_file << ";";
 cout << zeitblock[x].current_function << ";";
 cout << zeitblock[x].current_line << ";";
 cout << zeitblock[x].clock_user << ";";
 cout << zeitblock[x].clock_system << ";";
 cout << zeitblock[x].cpuload[0] << ";";
 cout << zeitblock[x].cpuload[1] << ";";
 cout << zeitblock[x].cpuload[2] << ";\n";
 }
}



qstime zeit;


/*
----------------------------------------------
*/

/*
--------------------
stripslashes
Entfernt \ bei " \ oder '
--------------------
*/

string stripslashes(string text)
{

if(text==""){return "";};
qs_start();

register const char *cstring;
register char *ziel;
register int z=0;

cstring=text.c_str();
ziel = new char[strlen(cstring)+1];

while(*cstring)
{
if( (ziel[z]=*cstring++)==92)
   {
    if( *cstring==34 || *cstring==39 || *cstring==92)
    z--;
   };
z++;
}

ziel[z]=0;
qs_stop();
return ziel;

}


/*
--------------------
addslashes
Fuegt \ bei " \ oder ' an
--------------------
*/

string addslashes(string text)
{
qs_start();
register int x;
register int t=0;
register char *tmp = new char[text.length()*2];
register const char *txt=text.c_str();
//printf("\nlashcall mit %s\n ",txt);
register int l = text.length();

while(*txt)
{
//printf("bearbeite Zeichen %c\n",*txt);
 if(*txt == 34 || *txt == 39 || *txt == 92) {tmp[t++]=92;}

 tmp[t++]=*txt;
 *txt++;
}
tmp[t]=0;
//printf("lavsjkdf --- %s\n",tmp);
qs_stop();
return tmp;
}

/*
entfernt Leerzeichen und Returns am Anfang und am Ende
*/ 

string trim(string text)
{
qs_start();
register const char *txt = text.c_str();
register char *tmp;
register int t=0;
int l = strlen(txt);

//Zeichen am Anfang uebergehen
tmp=new char[l];
while(*txt) { if ( (*txt!=' ' && *txt!=13 && *txt!=10) || *txt==0){break;} *txt++; }

//wir gehen zum Ende
register const char *ende=txt+l-1;

//rueckwaerts suchen bis ein passendes Zeichen gefunden ist

while(*ende)  
 { 
 if( *ende != ' ' && *ende != 13 && *ende != 10){*ende++;break;}
 *ende--;
 }

while(txt!=ende) { tmp[t++]=*txt++; } 
tmp[t+1]=0;
qs_stop();
return tmp;
}




string str_replace(string searchfor,string replacetxt,string origtxt )
{
qs_start();
DEBUG("STR_REPLACE START mit String:");
register const char * sf=searchfor.c_str();
register const char * rt=replacetxt.c_str();
register const char * ot=origtxt.c_str();

int l = strlen(ot);
int memcounter = l+(3*strlen(rt));//Speicher = Originaltext+3fachen Replacetxt
//int otcounter  = l;
register int restspeicher = memcounter;
register int restspeicher_buffer;

register int t=0;
register int t_buffer;

register const char *ot_buffer;
register const char *rt_buffer;
register const char *sf_buffer;

register int rt_len=strlen(rt);
register int rt_counter;


register int sf_len=strlen(sf);
register int sf_counter;

//register char * tmp = new char(memcounter);
register char * tmp = (char*)malloc(memcounter);

register int pagemul=10;//wird +1 erhoeht nach jedem realloc!

while(*ot)
{
DEBUG("WHILE *OT Schleife Start an TMP-POS %i\n",t);
DEBUG("WHILE Start TMP: %s\n",tmp);
DEBUG("WHILE Restspeicherbytes: %i\n",restspeicher);
/*
Stimmt Anfang und Ende mit Suchbegriff ueberein? Performance rulez!
*/
if(*ot==*sf && *ot+sf_len==*sf+sf_len)
{
DEBUG("Erster und letzter Punkt im Suchbegriff passen schonmal");
DEBUG("vergleiche Zeichen %c mit Zeichen %c\n",*ot,*sf );

   //jetzt muss der gesamte Bereich verglichen werden
   //wenn er passt, dann muessen wir substituieren

    ot_buffer=ot;		//Quelltext "hallo #irgendwas#"
    rt_buffer=rt;		//"ERSETZUNGSWERT"
    rt_counter=0;
    sf_counter=0;
    sf_buffer=sf;		//"#irgendwas#"  
    t_buffer=t;
    restspeicher_buffer=restspeicher;

    

    while(*sf_buffer)
     {


DEBUG("MEMORY CHECK, aktueller String: __%s__\n",tmp);
DEBUG("Restspeicher: %i Minimum: %i \n",restspeicher,rt_len);
 //Restspeicher pruefen
   if (restspeicher<(rt_len))
     {
       DEBUG("+++ WARNUNG, REALLOCATION\n");
       DEBUG("Aktueller MEMCOUNTER Bytes: %i\n",memcounter);
       memcounter=memcounter+(rt_len*pagemul);
       DEBUG("Reallokation auf %i Bytes\n",memcounter);
       DEBUG("vor realloc: __%s__\n",tmp);
       //char *stmp = strdup(tmp);
       //xtmp= new char(memcounter); 
       tmp=(char*)realloc(tmp,memcounter);
       //tmp=strcpy(tmp,stmp);
       DEBUG("nach realloc: __%s__\nMemcounter fuer Realloc: %i\n",tmp,memcounter);
       restspeicher=memcounter;
        pagemul++;
     }

        DEBUG(" Loop: Vergleiche sf %c mit aktuellem Zeichen %c an Position %i[%c\%d]\n",*sf_buffer,*ot_buffer,t_buffer,tmp[t_buffer],tmp[t_buffer]);

      if(*sf_buffer!=*ot_buffer){break;} 

      DEBUG(" Zeichen %c passt mit %c\n",*ot_buffer,*sf_buffer);
      sf_counter++; 
      if(rt_counter++<=rt_len){tmp[t_buffer++]=*rt_buffer++;restspeicher_buffer--;}  
      *ot_buffer++;
      *sf_buffer++;
     }

    DEBUG("sf_counter  %i / sf_len %i\n",sf_counter,sf_len);

    if(sf_counter == sf_len){ 
                            DEBUG("  STRING PASST, bin noch an TMP-Pos %i\n",t_buffer);
			    //evtl Rest anfuegen
                            if(*rt_buffer){
				//t_buffer--;//test
			      while(*rt_buffer){

DEBUG("               aktueller String: %s\n",tmp);


				 DEBUG("    setze Restzeichen %c in TMP ein an Pos %i\n",*rt_buffer,t_buffer);

                                 tmp[t_buffer++]=*rt_buffer++;restspeicher_buffer--;}
				//Ende While
                              t_buffer++;   
                                          }
                            t=t_buffer;ot=ot_buffer;t--;
                            restspeicher=restspeicher_buffer;
                            }
                             else{tmp[t]=0;}

}//while buffer

DEBUG("TMP-Memory Zeichencounter an Position %i, aktuelles Zeichen in tmp-1: %c [%d]\n",t,tmp[t-1],tmp[t-1]);
tmp[t++]=*ot++;restspeicher--; 
tmp[t]=0;//Folgezeichen IMMER NULL
}//while SF

tmp[t]=0;
DEBUG("RUECK-STRING: %s\n",tmp);
qs_stop();
return tmp;
}

/*
packt einen String in eine Liste getrennt durch sep, das aus
einem Zeichen bestehen soll (deswegen explodec und nicht explode)
Rueckgabe = Vektor mit den Strings 
*/

vector<string> explodec(string sep,string text)
{
qs_start();

register char **charvector;
register char *textzeiger=(char*)text.c_str();
register const char *sepzeiger=sep.c_str();
register int rueck=0;
register int z=0;
charvector = (char **)malloc(text.length());

vector<string> stringvector;

//Startpunkt = textzeiger
charvector[0]=&textzeiger[0];
//,,
DEBUG("Setze cv0 auf tz0 %i/%i",charvector[0],&textzeiger[0]);

while(*textzeiger) 
 {
 DEBUG("  textzeiger[%c]=sepzeiger[%c]?",*textzeiger,*sepzeiger);
 if(*textzeiger==*sepzeiger){
    rueck++;
    charvector[rueck]=textzeiger+1;//Startadresse
	textzeiger[0]=0;//das Zeichen an der Position 0 killen
	stringvector.push_back(charvector[rueck-1]);
	DEBUG("    Inhalt von charvector r-1 %i = %s",rueck-1,charvector[rueck-1]);

    }//if 
 *textzeiger++;
 }//while
if(rueck>0){stringvector.push_back(charvector[rueck]);}
qs_stop();
if(zeit.m==3){zeit.print();} 

return stringvector;
}


int main()
{

string sep,suchtext;
vector <string> ergebnis;
EDEBUG
sep=" ";
suchtext="das ist ein test";
ergebnis=explodec(sep,suchtext);
ergebnis=explodec(sep,suchtext);
ergebnis=explodec(sep,suchtext);
ergebnis=explodec(sep,suchtext);

int z=0;
for(z=0;z<ergebnis.size();z++)
{
cout <<  ergebnis[z];
cout <<"\n";
}

DDEBUG
string str_addslashes;
str_addslashes="hallo \"Oetzi 'Muschikatz";
for(z=0;z<22;z++)
{
cout <<z;
str_addslashes=addslashes(str_addslashes);
}

/*

string irgendwas;
irgendwas="  ";
irgendwas=trim(irgendwas);
cout << ":::" << irgendwas << ":::";
exit(0);
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
