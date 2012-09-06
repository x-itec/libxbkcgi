// $Id: xcgi.cpp,v 1.15 2001/12/14 14:07:57 bk Exp $
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

extern int errno;

/**
Daten, die accept() zurueckliefert.
Diese Daten vereinfachen es, wenn nach einem accept() ein
Thread benutzt werden soll, weil hier alle wichtigen Infos stehen,
die man fuer die weitere Nutzung brauchen koennte.
*/
class xcgi_accept_data
{
public:
/// Filedescriptor
int fd_conn;
/// Remote IP
string remote_ip;
/// Remote Port
string remote_port;
};

/**
Basisklasse fuer Errormeldungen des Transmitters
*/

class xcgi_transmitter_error
{
public:
/// Fehlernummer, wird durch die abgeleiteten Klassen erzeugt
int errorcode;
};


/**
Exception fuer einen Timeout.
Wird erzeugt, wenn ein Timeout auftritt. Ein Timeout tritt auf, wenn ein festgelegter Zeitpunkt (settimeout) erreicht wurde. 
*/

class xcgi_transmitter_error_timeout : public xcgi_transmitter_error
{
public:
xcgi_transmitter_error_timeout();
};

xcgi_transmitter_error_timeout::xcgi_transmitter_error_timeout()
{
DEBUG("Exception xcgi_transmitter_error_timeout: Festgelegtes Timeout erreicht");
}

/**
End of File. Wird bei EOF Signal ausgeloest.
*/
class xcgi_transmitter_error_eof : public xcgi_transmitter_error
{
public:
xcgi_transmitter_error_eof();
};

xcgi_transmitter_error_eof::xcgi_transmitter_error_eof()
{
DEBUG("Exception xcgi_transmitter_error_eof: EOF Signal.");
}




/**
Execption fuer Datentransfer.
Wird erzeugt, wenn beim Lesen/Schreiben der Stream abbricht (Verbindung einfach getrennt, oder was auch immer) in einer TCP Transmitterklasse)
*/
class xcgi_transmitter_error_transfer : public xcgi_transmitter_error
{
public:
xcgi_transmitter_error_transfer();
};


xcgi_transmitter_error_transfer::xcgi_transmitter_error_transfer()
{
DEBUG("Exception: xcgi_transmitter_error_transfer: Stream abgebrochen, EOF oder Timeout");
}

// ---------------------------------------------------------------------

/**
Transmitter Basisklasse
Die Transmitter-Basisklasse hat die Aufgabe, Daten auf ein Ausgabemedium oder von einem Eingabemedium zu lesen.
raw_send() soll den String an das jeweilige Geraet senden,
je nach Ableitung dieser Klasse (siehe xcgi_transmitter_www)

*/

class xcgi_transmitter_base
{
private:
///true wenn Header gesendet wurde
bool headersend;//false=noch nicht,true=Header gesendet
string ctype;//content-type

protected:
int t_timeout;//Timeout in Sekunden -> settimeout

public:
///Ausgaberoutine
virtual void raw_send(string text)=0;//sendet eine Zeile
///Zeichen lesen 
virtual string raw_readchar()=0;//liest ein Zeichen
/// Zeile lesen
virtual string raw_readline()=0;//liest eine Zeile

///Content-Type setzen (z. B. text/html)
void setcontenttype(string text);//ctype setzen
///sendet den content-type wenn headersend == false
void sendcontenttype();
///Content-Type nicht senden
void disablecontenttype();//verhindert die Ausgabe des Content-Types sofort
///ruft Transmitter->raw_send auf
//void send(string text);//ruft raw_send auf
///Sendet ein Return, je nach Implementation (z. B. ein BR oder ein Slash-N...
virtual void sendLF()=0;
};

inline void xcgi_transmitter_base::disablecontenttype()
{
headersend=true;
}

inline void xcgi_transmitter_base::setcontenttype(string text)
{
ctype=text;
}

void xcgi_transmitter_base::sendcontenttype()
{
if(headersend==false){raw_send("Content-Type: "+ctype+"\n\n");
headersend=true;}
}

/*
void xcgi_transmitter_base::send(string text)
{
raw_send(text);
}
*/






/**
TCP Transmitterbasisklasse.
Liefert Basisfunktionalitaeten die noch abgeleitet werden muessen

*/

class xcgi_transmitter_base_tcp : public xcgi_transmitter_base
{

protected:
/// Socket-Filedescriptor und Connection-Filedescriptor
int 	fd_sock,fd_conn;
/// Anzahl der max. wartenden Verbindungen fuer listen()
int backlog;//fuer listen(), maximale Anzahl von wartenden Verbindungen im Queue, war frueher auf 5 aber das reicht nicht aus, deswegen variabel

public:
/// maximale Buffergroesse fuer readline
int bufsize;//buffer-size fuer readline

/// wohin connecten (Port)? muss gesetzt werden
int 	conn_port;//Portnummer an die man connecten soll
/// IP Adresse angeben, oder einen Hostnamen bei conn_hostname
string 	conn_ip;//IP Adresse oder:

/// Wird nach accept() gefüllt um zu sehen, wer connected hat.
string remote_ip;
int remote_port;

/// Semaphore-ID fuer Locking
int 	sem_lockid;


void   raw_send(string text);

void sendLF();

/// bind
virtual void bind()=0;
/// listen
virtual void listen()=0;
/// accept, gibt Filedescriptor zurueck sonst Exception
virtual xcgi_accept_data* accept()=0;

/// Zeichen lesen
string raw_readchar();//liest ein Zeichen ein, bei Fehler Exception xcgi_transmitter_error_outofdata
/// Zeile lesen bis Return
string raw_readline();//liest eine Zeile Abschluss \n und gibt sie zurueck ohne \n
/// Zeile senden
void sendline(string line);

///Timeout setzen fuer read/write nach accept() (Server) oder connect() (Client) in Sekunden. Wenn Timeout ueberschritten, wird Exception ausgeloest.
void settimeout(int secs);

void thread();/// erstellt einen neuen Thread
void lock();/// Semaphore eintreten
void unlock();/// Semaphore verlassen
void close();


/// Setzt einen Filedescriptor 
void set_fd(int fd);
//Konstruktor, um fuer Newbies den backlog zu setzen
xcgi_transmitter_base_tcp();
};

void xcgi_transmitter_base_tcp::set_fd(int fd)
{
fd_conn=fd;
}

void xcgi_transmitter_base_tcp::settimeout(int secs)
{
t_timeout=secs;
}

string xcgi_transmitter_base_tcp::raw_readchar()
{
char buffer[1];
string zeichen;
struct timeval tv;
int n=0;

tv.tv_sec=t_timeout;
tv.tv_usec=0;
fd_set rset;
DEBUG("FD-ZERO");
FD_ZERO(&rset);
DEBUG("FD-SET");
FD_SET(fd_conn,&rset);
DEBUG("FD-SET done.");
DEBUG("select-START, timeout: %i secs",t_timeout);
int s=select (fd_conn+1,&rset,NULL,NULL,&tv);
DEBUG("select-STOP, result: %i",s);
//0=timeout
//>0 = mindestens ein fd zum lesen bereit
//<0 = Fehler
if(s==1)
 {
n=read(fd_conn,&buffer[0],1);
zeichen=buffer[0];
 }
else if (s<0)
 {
 xcgi_transmitter_error_transfer t;
 throw(t);// keine Zeichen mehr im Stream / schweres Problem 
 }
else if(s==0)
 {
 //timeout
 xcgi_transmitter_error_timeout t;
 throw(t);//Timeout wurde erreicht
 }

if (n==0) //eof
 {
 xcgi_transmitter_error_eof t;
 throw(t);//EOF
 }

return zeichen;
}

string xcgi_transmitter_base_tcp::raw_readline()
{
string z,e;
register int c=0;
DEBUG("bufsize: %i",bufsize);

while ((z=raw_readchar()) !="\n")
 {
 e+=z;if(++c>=bufsize){break;};
 DEBUG("read %i\n",c);

 }
return e;
}

void xcgi_transmitter_base_tcp::sendline(string line)
{
register int l = line.length();
DEBUG("SENDE ZEILE");
DEBUG("Laenge: %i",line.length());
register int x = ::write(fd_conn,line.c_str(),l);
DEBUG("write-result: %i",x);
if (x != l )
 {
 xcgi_transmitter_error_transfer t; throw(t);
 }
}



xcgi_transmitter_base_tcp::xcgi_transmitter_base_tcp()
{
backlog=5;
bufsize=4096;
t_timeout=60;//60 Sekunden 
}


void xcgi_transmitter_base_tcp::close()
{
::close(fd_conn);
::close(fd_sock);
}

void xcgi_transmitter_base_tcp::lock()
{
}
void xcgi_transmitter_base_tcp::unlock()
{
}

void xcgi_transmitter_base_tcp::thread()
{
}

void xcgi_transmitter_base_tcp::raw_send(string text)
{
int x = ::write(fd_conn,text.c_str(),text.length());
if (x != text.length() )
 {
 xcgi_transmitter_error_transfer t; throw(t);
}

}

void xcgi_transmitter_base_tcp::sendLF()
{
raw_send("\r\n");//muss ich noch pruefen ob konform mit java
}



/**
TCP V4 Ableitung zur Uebertragung von Daten mittels TCP V4
*/

class xcgi_transmitter_base_tcp4 : public xcgi_transmitter_base_tcp
{
private:
struct sockaddr_in adresse;
size_t adrlaenge;


public:
xcgi_transmitter_base_tcp4(); 
void bind();
void listen();
xcgi_accept_data * accept();
};

xcgi_transmitter_base_tcp4::xcgi_transmitter_base_tcp4()
{
adrlaenge=sizeof(struct sockaddr_in);
}

void xcgi_transmitter_base_tcp4::bind()
{

bzero(&adresse,sizeof(adresse));

fd_sock = socket(PF_INET,SOCK_STREAM,0);

if( fd_sock  <0)
 {
 DEBUG("BIND/SOCKET PF_INET SOCK_STREAM ERROR");
 //Exception auswerfen
 }
printf("Socket: %i",fd_sock);

if(conn_ip!="") //wenn IP Adresse vorliegt
 {
  DEBUG("Wandle IP in Hex um");
 if( inet_pton(AF_INET,conn_ip.c_str(),&adresse.sin_addr)  <1) 
  {  
   DEBUG("INET_PTON ERROR"); 
   //Fehler
  }
 }

int dummy=1;
setsockopt(fd_sock,SOL_SOCKET,SO_REUSEADDR,&dummy,sizeof(dummy));

adresse.sin_family=AF_INET;
adresse.sin_port = htons(conn_port);
if(conn_ip=="")
 { 
 DEBUG("conn_ip ist leer, memset, listen to INADDR_ANY");
 adresse.sin_addr.s_addr=htonl(INADDR_ANY); 
 }

DEBUG("BIND TO PORT: %i",conn_port);


if( int b=::bind(fd_sock,(struct sockaddr *) &adresse,sizeof(adresse)) )
 {
 DEBUG("BIND ERROR");
 DEBUG("Errorcode: %i",errno);
 //Fehler BIND
 exit(1);
 }

}

void xcgi_transmitter_base_tcp4::listen()
{
 if(::listen(fd_sock,backlog))
  {
   DEBUG("LISTEN ERROR");
  //Fehler listen()  
  }
}

xcgi_accept_data*  xcgi_transmitter_base_tcp4::accept()
{
DEBUG("TCP4: accept()");
fd_conn = ::accept(fd_sock,(struct sockaddr *)&adresse,&adrlaenge);  
DEBUG("TCP4: accept fd_conn: %i",fd_conn);

 if (fd_conn <0)
  {
   DEBUG("Vermutlich ACCEPT ERROR");
  //accept Fehler oder es gibt keine Verbindungen mehr
  //den fd_conn kann der User selbst kontrollieren
  } 
  DEBUG("fd_conn: %i",fd_conn);

//REMOTE-PORT und IP 
char str[INET_ADDRSTRLEN];//ipv4 
remote_ip=inet_ntop(AF_INET,&adresse.sin_addr,str,INET_ADDRSTRLEN);
remote_port=ntohs(adresse.sin_port);

DEBUG("remote-ip/port: %s/%i",remote_ip.c_str(),remote_port);

//Datenstruktur fuellen
xcgi_accept_data *daten;
daten = new xcgi_accept_data;
daten->fd_conn=fd_conn;
daten->remote_ip=remote_ip;
daten->remote_port=remote_port;
return daten;


}















/**
WWW Transmitter, zur Ausgabe auf stdout. 
*/
class xcgi_transmitter_www : public xcgi_transmitter_base
{
void raw_send(string text);
/// im Augenblick unsupported
string raw_readchar();
/// im Augenblick unsupported
string raw_readline();
void sendLF();
string header();
};

string xcgi_transmitter_www::raw_readchar()
{
return "";
}

string xcgi_transmitter_www::raw_readline()
{
return "";
}


inline void xcgi_transmitter_www::raw_send(string text)
{
cout << text;
}
inline void xcgi_transmitter_www::sendLF()
{
raw_send("<br>");
}








/**
In dieser Klasse stehen aufgestellt das Kommando
sowie dessen Parameter. Einmal als Liste und
per Anweisung auch durch , aufgeteilt.
/yyy
*/
 
class xcgi_commandparser_base
{
protected:
vector<string> plist;
public:
string command;
string parameters;
/// den Parameterstring aufteilen 
void processparameters(); 

};


void xcgi_commandparser_base::processparameters()
{

register int x=0;//Startposition Quellstring
register int y=0;//Startposition Zielstring
register int l=parameters.length(); 
char *kpointer = new char(l+1);
//innen: Suche nach ,

DEBUG("Loop außen");
//zzz 
do
 {
 for(;;)  
  { 
   kpointer[y]=parameters[x]; 
   DEBUG("Pos: %i/%i max: %i ",x,y,l);
  if (x>=l || parameters[x]==','){DEBUG("BREAK");break;}
 ++x;++y; 
  } 
++x;//, ueberspringen
 kpointer[y]=0;
 DEBUG("PUSH BACK: %s",kpointer);
 plist.push_back(kpointer);y=0;
 }
while(x<=l);
DEBUG("Ende Loop aussen");
}


/**
Ausgabeklasse, verwendet einen Transmitter zur Datenausgabe
*/

class xcgi_out_base
{
private:
string send_buffer;
public:
/// Transmitter ableiten, Public zur weiteren Nutzung
xcgi_transmitter_base *transmitter;
/// Konstruktor mit Transmitter
xcgi_out_base(xcgi_transmitter_base *trans); 
/// Text in Buffer hinzufuegen
void add(string text);//Text an Buffer anfuegen
/// Zeichenkette senden
void sendline(string text);//Zeile senden
/// Buffer der mit add(..) gefuellt wurde, senden
void sendbuffer();//Buffer senden
/// Zeichen lesen vom Transmitter
string readchar();
/// Zeile lesen vom Transmitter
string readline();
/// readcommand - eine Zeile einlesen und als Kommando auswerten. Beispiel: Jemand sendet das Kommando test(hallo) dann wird das Kommando test ermittelt und der Parameter hallo.
xcgi_commandparser_base readcommand();
};

xcgi_commandparser_base xcgi_out_base::readcommand()
{
string c       = transmitter->raw_readline();

char * co      = new char(c.length());
char * pa      = new char(c.length());
register int l = c.length();
register int p = 0; 
xcgi_commandparser_base rueck;


DEBUG("suche Kommando");
register int x;

for(x=0;x<l;x++)
 {
 if(c[x]=='('){break;}
 co[x]=c[x];
 }
 co[x]=0; 
DEBUG("Kommando: %s",co);
rueck.command=co;

DEBUG("suche Parameter ");
// ) suchen
register int y;
for(y=l;y>0;y--)
 {
 if(c[y]==')'){break;}
 }
DEBUG(" ) an Position %i",y);
x++;y--;
DEBUG("Parameter start/end: %i/%i",x,y);

register int z;

for(z=x;z<=y;z++) 
{
pa[p++]=c[z];
}
pa[p]=0;
DEBUG("Parameter: %s",pa);
rueck.parameters=pa;
return rueck;
}

inline void xcgi_out_base::add(string text)
{
send_buffer+=text;
}

inline xcgi_out_base::xcgi_out_base(xcgi_transmitter_base *trans)
{
transmitter=trans;
}


inline void xcgi_out_base::sendbuffer() 
{
transmitter->raw_send(send_buffer);send_buffer="";
}

inline void xcgi_out_base::sendline(string text)
{
transmitter->raw_send(text);
}

inline string xcgi_out_base::readchar()
{
return transmitter->raw_readchar();
}

inline string xcgi_out_base::readline()
{
return transmitter->raw_readline();
}

/*
Beispiel fuer den Kopf:

Transmitter erstellen:

xcgi_transmitter_base *transmitter;
transmitter = new xcgi_transmitter_www;

Content-Typ einstellen:

transmitter->setcontenttype("text/html");

Transferkomponente initialisieren
xcgi_out_base *transfer(transmitter);

Beispieltext auf stdout senden:
transfer->send("Hallo Leute");

*/
