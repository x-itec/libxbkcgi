#ifndef xbkcgiform
#define xbkcgiform
#include<string>
#include<stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <map>
#include <iostream>


#include <xbk/xbkstringutil.h>


extern int errno;


/*
todo:
die Basisklasse fuer die CGI Behandlung kann die Multimap
bekommen und die dafür vorgesehenen Routinen. Sie sind 
für sich allein gestellt und können von daher übernommen werden in
die base Klasse.


/*

#define DEBUGME
#define QS

#include "xdebug.c"
#include "xqs.c"
*/

/*
----------------------------------------------
*/


//YYY
struct xcgi_formdata
{
string index;
string value;
//nur belegt bei Binaerdaten
char  *binvalue;
string content_type;
long content_length;
};


/*
Diese Klasse regelt den Zugriff auf GET/POST Variablen
die via Formular uebertragen wurden.

Referenzimplementation (Basisklasse:)
*/

class xcgi_form_base
{
public:

xbkstringutil xbks;//wird öfters benutzt, deswegen steckt die hier

/**
Egal ob die Daten via POST/GET uebertragen wurden,
hier soll ein Resultat folgen!
*/
virtual xcgi_formdata* getvalue(string value)=0;
/**
Gibt die Anzahl moeglicher Elemente von index zurueck,
zum Beispiel sinnvoll fuer RadioGroups die gleich heissen..
*/
virtual int count(string index)=0;

/**
Der Indexoperator soll getvalue aufrufen und ebenfalls
den Content zurueckgeben (xcgi_formdata)
*/
virtual xcgi_formdata& operator[](string index)=0;

/**
Daten aus dem Inputstream einlesen
*/
virtual void processformdata(FILE* inputstream,int maxlen)=0;  

/**
Eine Binaerdatei (File-Upload) speichern
*/  
virtual int savefile(string index, string filename)=0;

};

class xcgi_form : public xcgi_form_base
{
public:

multimap <string,xcgi_formdata> multimap_xcgi_formdata;
multimap <string,xcgi_formdata>::iterator multimap_iterator;

long   mem_size;
char* mem_content;
char* mem_content_backup;//Startposition backuped

int   getblock_size;     //-1 = nichts gefunden
char *getblock_content; //fuer getblock Routine
char *getblock_end;	//die Endposition des Blocks


public:

int savefile(string index, string filename);

/**
Anzahl der gefundenen Elemente fuer eine CGI Variable. Wird benutzt, wenn man mehrere Radio-Groups hat oder Mehrfachauswahl-Listenfelder die mehrere Ergebnisse zurueckliefern können
*/
int count(string index);
xcgi_formdata* getvalue(string value);
/**
getvalue holt das Element, was gerade mittels setindexpos gesetzt wurde
*/
xcgi_formdata* getvalue();
/** diese Funktion wird benutzt, wenn man mehrere Ergebnisse von einer CGI Variablen erwartet (Listenfelder, Radiogroups mit doppeltem Namen, ect). mit setindex setze ich dann die Suchposition und mit getvalue() hole ich mir das aktuelle Element, gleichzeitig spring getvalue ein Element weiter. Ist die Ergebnismenge leer, wird NULL zurueckgegeben.
*/
void setindexpos (string index);

void   processformdata(FILE* inputstream,int maxlen);
xcgi_formdata& operator[](string index);

string readmemline();
string getpostboundary();

inline bool eof();//fuer readmemline, ist false solange mem_size >0

int setnpos(string searchme);//suche in mem_content nach searchme und stellt mem_content auf das erste Zeichen hinter searchme - recht praktisch 

int getblock(string searchme);//sucht in mem_content nach dem Inhalt zwischen searchme und searchme.. also sowas wie searchme1234searchme = 1234 in getblock_content. rueckgabewert = -1 wenn nichts gefunden und >=0 == etwas gefunden

string nposition(char *source, string searchme, string endchar);//gibt den String zwischen searchme und endchar zurueck

char* nextline(char *source);//springt in die naechste Zeile, oder NULL wenn keine Zeile mehr vorhanden

string getline(char *source);//gibt den Inhalt der aktuellen Zeile zurueck oder "" wenn leer

};

int xcgi_form::count(string index)
{
return multimap_xcgi_formdata.count(index);
}


int xcgi_form::savefile(string index, string filename)
{
int saveresult=0;//Fehler, 1=ok
xcgi_formdata *content=NULL;
content=getvalue(index);
if(content)
	{

	/* Diese Abfrage hier ist deswgen weil sonst unnötig Mem 
		alloziiert wird wenn kein debug-mode
	*/	
	#ifdef DEBUGME
	if(debugme=1)
		{
		char verz[500];
		getcwd(verz,500);
		DEBUG("getcwd %s",verz);
		}
	#endif

	FILE *fd;
	DEBUG("filename: %s",filename.c_str());
	fd=fopen(filename.c_str(),"w");
	if(fd)    
		{
		DEBUG("SAVING %l bytes",content->content_length);
	         saveresult=fwrite(content->binvalue,content->content_length,1,fd);
		fclose(fd);	
		}
	#ifdef DEBUGME
	else {
		DEBUG("fopen() error");
		DEBUG("error: %s",strerror(errno));
             }
	#endif
	}
return saveresult;
}


void xcgi_form::setindexpos(string index)
{
multimap_iterator = multimap_xcgi_formdata.find(index);
}

xcgi_formdata* xcgi_form::getvalue()
{
xcgi_formdata *ro=NULL;

if(multimap_iterator != multimap_xcgi_formdata.end())
{
ro=&multimap_iterator->second;multimap_iterator++;
}
return ro;
}

xcgi_formdata* xcgi_form::getvalue(string index)
{
DEBUG("getvalue fuer %s",index.c_str());
multimap_iterator = multimap_xcgi_formdata.find(index);
if(multimap_iterator != multimap_xcgi_formdata.end())
{
DEBUG("Objekt gefunden");
xcgi_formdata *f;
DEBUG("Objekt-index: %s",multimap_iterator->second.index.c_str());
DEBUG("Objekt-Value: %s",multimap_iterator->second.value.c_str());
return &multimap_iterator->second;
}
else {return NULL;}

}

xcgi_formdata& xcgi_form::operator[](string index)
{
xcgi_formdata *f;
f=getvalue(index);
return *f;
}


string xcgi_form::getline(char *source)
{
char *buff=new char[strlen(source)];
buff[0]=0;
register int x=0;

	for(;;)
	{
	if(*source==NULL){DEBUG("NULL");break;}//Stringende
	if(*source==13){DEBUG("RETURN-chr13");break;}//returs
	if(*source==10){DEBUG("RETURN-chr10");break;}
	DEBUG("kopiere Zeichen %c ASCII-CODE %d",source[0],source[0]);
	buff[x++]=*source++;
	}
	buff[x]=0;
	DEBUG("Result: %s",buff);
return buff;
} 

int xcgi_form::getblock(string searchme)
{
getblock_size=-1; //Standard
register int l=searchme.length();
register long x,y;
	x=setnpos(searchme);//erste Startposition ermitteln
	//wegen dem Boundary \r eins weiter
	//mem_content++; 
	char *start=mem_content;//=das erste neue Zeichen

	y=setnpos(searchme);//naechste Startposition ermitteln
	//mem_content++;

	if(y<l)
	{
	DEBUG("getblock memsize leer stop");
	//kein Suchbegriff mehr gefunden
 	return -1;	
	}
	char *ende=mem_content-l-2;
	
	if( (x<l) || (y<l) ){return -1;} 
	//jetzt muss ich nur noch den Inhalt zwischen start und ende
	//nach getblock_content kopieren 
	
	*ende--;//Endposition eins zurueck

	getblock_size=ende-start;
	DEBUG("GB-SIZE: %i",getblock_size);
	getblock_content = new char[getblock_size];
	x=0;
	for(x=0;x<=getblock_size;x++)
	{
		getblock_content[x]=*start++; 
	} 
	getblock_end = getblock_content+getblock_size;
	//Speicherposition zurueckschrauben
	if(y==l)
	{
	DEBUG("Schraube");
	mem_content=mem_content-l-1;//vorher -2
	}
return getblock_size;
}

/*
Anwendungsbeispiel fuer nposition:
source beinhaltet: bla bla file="super" bla asdf
um an super heranzukommen mache ich folgendes
ergebnis=...nposition(source,"file=\"","\"");
*/


string xcgi_form::nposition(char *source, string searchme, string endchar)
{
const char *sme=searchme.c_str();
const char *smb=sme;
const char *smx=endchar.c_str();
char *start=NULL,*ende=NULL;
char *mem=NULL;
register int l=searchme.length();
register int x=0,y=0;
DEBUG("searchme= %s, endchar=%s",sme,smx);
char *result=NULL;
	for(;;) 
	{
	if(*source++==*sme++){++y;}else{y=0;sme=smb;}
	if(y==l){result=source;DEBUG("gefunden.");break;}
	if(*source==0){DEBUG("Stringende");break;}//leer
	}
	if(result)
	{
	DEBUG("Startposition fuer CUT: %s",result);
	start=result;
	for(;;)
		{
		if(result[0]==smx[0]){ende=result-1;DEBUG("CUT-Ende gefunden");break;}
		if(*result==0){DEBUG("Stringende");break;}
		++result;
		}
	if(ende)
	{
	DEBUG("Ende liegt vor.");
	int le=ende-start;
	DEBUG("Laenge: %i",le);
	mem=new char[le];
	for(y=0;y<=le;y++)
		{
		mem[y]=*start++;
		}
		mem[y]=0;

	}//if ende

	}//if result
DEBUG("mem:::%s:::",mem);
return (mem==NULL ? "" :mem);
}


/*
gibt die Laenge des Suchstrings zurueck wenn positiv und
setzt mem_content auf das naechste Zeichen, ansonsten
wenn nicht gefunden wurde, wird eine Zahl < Laenge zurueckgegeben
*/

int xcgi_form::setnpos(string searchme)
{
register long x=mem_size;
register int l=searchme.length();
register long y;
const char *sme = searchme.c_str();
const char *smb = sme;

DEBUG("suche nach %s",searchme.c_str());
DEBUG("Suche beginnt bei Content-Adresse: %i",mem_content);
DEBUG("Inhalt ab dieser Content-Adesse: %s",mem_content);
for(;;)
	{
	y=0;
	sme=smb;//startpos Suchstring
		for(;;)
		{
		//DEBUG("LOOP, Position: %i Zeichen: %c ASCII: %d",mem_content,mem_content[0],mem_content[0]);
		if(mem_size<0){DEBUG("kein Zeichen mehr, break");break;}//kein Zeichen mehr
		if(*mem_content++==*sme++){++y;}else{break;}
		}
	--mem_size;
	if(mem_size<0){DEBUG("mem_size leer");y=-1;break;}//kein Platz mehr
	if(y==l){DEBUG("gefunden!");break;}//gefunden, Zeiger zeigt schon auf naechstes Element
	}
DEBUG("y=l: %i=%i",y,l);
DEBUG("neue Content-Startposition ist nun: %i",mem_content);
DEBUG("Inhalt ab dieser Position: %s",mem_content);
return y;//Anzahl Zeichen
}

inline bool xcgi_form::eof()
{
return (mem_size > 0 ? false : true);
}

/*
readmemline
Es wird aus einem Speicherbereich, der mit
der Position beginnt die der Zeiger uebergibt
eine Zeile gelesen, die mit \n endet. Das
Zeilenende-Zeichen wird nicht mitgenommen.
Anwendungsbereich: Man hat Daten im RAM 
und muss davon eine einzelne Zeile ermitteln.

*/

string xcgi_form::readmemline()
{
char *result=new char[mem_size]; 
DEBUG("memory von %i Bytes angelegt.",mem_size);
register int x=0,y=0;
for(;;)
	{
	if(*mem_content ==13){break;} 
	if(x>=mem_size){break;}
	result[x++]=*mem_content++;   
	}
*mem_content++;//Return ueberspringen
mem_size=mem_size-x;//fuer den naechsten Aufruf
return result;       
}

/*
springt in die naechste Zeile, wobei ein Zeilensprung
durch ein chr13 im Augenblick gekennzeichnet wird
*/
char* xcgi_form::nextline(char *source)
{
char *res=source;
DEBUG("nexline fuer: %s",source);
for(;;) 
	{
	//DEBUG("pruefe Zeichen: %c",res[0]);  
	if(*res==13){res++;DEBUG("RETURN-chr13"); break;}
	if(*res==10){res++;DEBUG("RETURN-chr10"); break;}
	if(*res==0){DEBUG("NULL");return NULL;}
	res++;
	}
return res;
}





/*
Diese Routine ist nur der Vollstaendigkeit halber da,
um die Funktionen besser auseinanderzuhalten.
POST Data wird durch ein Boundary getrennt, welches
durch die erste Zeile der Daten dargestellt wird.
Dieses Boundary holen wir uns, indem wir einfach nur
die erste Zeile lesen.
*/

string xcgi_form::getpostboundary()
{
mem_content=mem_content_backup;//resetten
string x= readmemline();
mem_content=mem_content_backup;//und wieder resetten
return x;
}


/*
--------------------
Diese Funktion gibt die Formulardaten zurueck,
je nach GET oder POST erstmal als RAW Data
*/
void xcgi_form::processformdata(FILE* inputstream, int maxlen)
{
string request_method;
request_method=xbks.xbkgetenv("REQUEST_METHOD");
char *post_result;

xcgi_formdata *formdata;

string cgi_value,cgi_name,cgi_contenttype;
char *cgi_binvalue;//binaerdaten
int cgi_contentlen;//fuer Binaerdaten
long x;//ein Zaehler
long sta;
long b;

int n,l;//standard-schneifencounter

string result;

if(request_method=="GET") 
	{
	string dummystring="";

	DEBUG("REQUEST-METHOD: GET");
	result=xbks.xbkgetenv("QUERY_STRING");

	 formdata = new xcgi_formdata;

	 l=result.length()+1;
	
	for(n=0;n<=l;n++)
	{
	if(result[n]=='=')
		{
			//aktueller Wert = index
			DEBUG("FORMDATA-INDEX FOUND: %s",dummystring.c_str());
			formdata->index=dummystring;
			dummystring="";
		}else
	if(result[n]=='&' || n==l)
		{
			//aktueller Wert = value
			DEBUG("FORMDATA-VALUE FOUND: %s",dummystring.c_str());
			formdata->value=dummystring;
			DEBUG("ADDING INDEX/VALUE: %s/%s",formdata->index.c_str(),formdata->value.c_str());
			   multimap_xcgi_formdata.insert(
                		make_pair<string, xcgi_formdata>
                		(formdata->index,*formdata));
		 	formdata = new xcgi_formdata;	
			dummystring="";
			//xyy
			//formdata Element posten
		}else
		{	dummystring+=result[n];}
	}


	DEBUG("Query-String: %s",result.c_str());
	}
else if(request_method=="POST" )
	{
	DEBUG("REQUEST-METHOD: POST");
	string content_length=xbks.xbkgetenv("CONTENT_LENGTH");
	mem_size=atoi(content_length.c_str());
	//FIX: die maxlen muss noch geprueft werden!  
	mem_content = (char*)malloc(mem_size); 

	sta=fread(mem_content,1,mem_size,inputstream);
	mem_content_backup=mem_content;
	DEBUG("Hole Boundary");
	string boundary=getpostboundary();
	DEBUG("Boundary: %s",boundary.c_str());

	DEBUG("Hole testweise ersten Block");

     while((b=getblock(boundary)  )!=-1) {
	if(b==-1){break;}
	
	formdata = new xcgi_formdata;
	DEBUG("MEMORY: %s",mem_content);
	DEBUG("Hole mal name= Konstruktion");

	/*
	ich weiss auch dass das doppelt ist, wenn man das in die
	multimap packt. Das ist mir im Augenblick aber EGAL
	*/
	formdata->index=nposition(getblock_content,"name=\"","\"");

	DEBUG("getblock_content Position jetzt: %s",getblock_content);
	DEBUG("ich springe mal eine Zeile weiter");
	getblock_content=nextline(getblock_content);
	DEBUG("getblock_content nach Zeilensprung: %s",getblock_content);

	DEBUG("Content-Type ermitteln falls vorliegend");

	getblock_content=nextline(getblock_content);//chr13
	getblock_content=nextline(getblock_content);//chr10
 	//ZZZ	
	formdata->content_type=getline(getblock_content);
	DEBUG("CCT: %s",formdata->content_type.c_str());
	
	/*
	wenn der Content-Type leer ist, dann ist die naechste Zeile
	der Inhalt des Feldes cgi_name
	
	Ansonsten
	liegt ein Content-Type vor, dann ist die Folgezeile leer
	und man muss nochmal ein nextline abschmeissen. Ausserdem
	kommen dann Binaerdaten und keine Stringdaten mehr.	

	*/
	cgi_contentlen=0;
	cgi_binvalue=NULL;
	
	/*
	wenn spaeter die contentlen >0 ist, dann liegen
	Binaerdaten in binvalue vor, ansonsten nur ASCII Daten
	in der anderen Variable
	*/
	if(formdata->content_type=="")
	{
	DEBUG("Kein ContentType, also ist das naechste Feld das Ergebnis");
	getblock_content=nextline(getblock_content);//chr13
	getblock_content=nextline(getblock_content);//chr10
	formdata->value=getline(getblock_content);
	//cout << "CGI-VALUE: " << cgi_value << "\r";
	DEBUG("Value: %s",cgi_value.c_str());
	}
	else
	{
	DEBUG("Es liegt ein Content-Type vor: %s",cgi_contenttype.c_str());
	DEBUG("CTF");
	//naechste Zeile
	getblock_content=nextline(getblock_content);//chr13
	getblock_content=nextline(getblock_content);//chr10
	getblock_content=nextline(getblock_content);//chr13
	getblock_content=nextline(getblock_content);//chr10
	DEBUG("Wir sind hier: %s",getblock_content);

	/*
	Wir haben jetzt die exakte Startposition fuer den Content.
	Jetzt muss der Inhalt nur noch ermittelt werden
	*/

	DEBUG("ADDR getblock_end: %i ADDR getblock_content %i",getblock_end,getblock_content);

	formdata->content_length=getblock_end-getblock_content;
	DEBUG("CL: %i",formdata->content_length);
	formdata->binvalue=NULL;
	if(formdata->content_length>0)
	{	
	formdata->binvalue = new char[formdata->content_length];
	//AAA
	for(x=0;x<formdata->content_length;x++)
		{
		//DEBUG("Kopiere Zeichen: %c ASCII: %d",*getblock_content,*getblock_content);
		formdata->binvalue[x]=*getblock_content++;
		}
		DEBUG("Anzahl Zeichen kopiert: %i",x);

		getblock_content=getblock_end;
		mem_size=mem_size-formdata->content_length;
		DEBUG("CTA");
		mem_content=mem_content+formdata->content_length;
		DEBUG("CTA-PS: %i",mem_content);	
	}//if contentlen >0
	}//content-type = vorliegend
	//DEBUG("getblock_content Inhalt nun: %s",getblock_content);
	DEBUG("Content-Header-Length: %i",b);
	//Formdata in Multimap eintragen
	//OOO
	multimap_xcgi_formdata.insert(
		make_pair<string, xcgi_formdata>
		(formdata->index,*formdata));
	}//while
	}
DEBUG("Ende Processformdata.");
}

#endif
