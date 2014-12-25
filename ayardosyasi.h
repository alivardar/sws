#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syslog.h>
#include <time.h>
#include <sys/wait.h>

int i, sock;
int rc = 0;
int serverSocket;
struct sockaddr_in serverAddr;
struct sockaddr_in clientAddr;
struct hostent *entity;
int clientAddrSize;
char *cbuf;

char buffer[4096];
char *cp, *cp2;
FILE *f;
char home[255];
int toplam;

int port;			// Default server port number
char homedizini[50];		// ana dizin olan yer
char defaultsayfa[50];		// default sayfa
char hatasayfasi[100];		// default sayfa
int maxbaglanti;
int maxbag=0;
int userid, groupid;
FILE *dosya;
char satir[50];
char dizi[50];
char *okunan;
FILE *fdsws, *fdsws_err;	//log files
char zaman[20];
int git;
char *hatamesaj = "<html><body><br>File not found.<br><br>Sws Web Server<br></body></html>\0";


char *gettime ()
{
  time_t time_of_day;
  time_of_day = time (NULL);
  strftime (zaman, 80, "%d.%m.%Y - %H.%M.%S", localtime (&time_of_day));
  return zaman;
}


void open_log_file (void)
{
  fdsws = fopen ("/var/log/sws_web_server/sws_web_server.log", "a");
  fdsws_err = fopen ("/var/log/sws_web_server/sws_web_server_err.log", "a");
  if (!(fdsws && fdsws_err))
    {
      //syslog (LOG_INFO, "Log Dosyalarý açýlamadý. /var/log/sws_web_server dizinini kontrol ediniz");
      syslog (LOG_INFO, "/var/log/sws_web_server/sws_web_server l og files cannot opened. ");
      exit (1);
    }
  return;
}

void write_log (char *chr)
{
  //fprintf (fdsws, "%s ip:%s iþlem:\"%s\"\n", gettime (), inet_ntoa ((struct in_addr) clientAddr.sin_addr), chr);
  fprintf(fdsws,"%s ip:%s job:\"%s\"\n",gettime(),inet_ntoa((struct in_addr) clientAddr.sin_addr), chr);
  fflush (fdsws);
}

void write_err_log (char *pch)
{
  fprintf (fdsws_err, "%s %s\n", gettime (), pch);
  fflush (fdsws_err);
}

int sendall(int s, char *buf, int *len)
{
	int total = 0;
	int bytesleft = *len;
	int n;	
	while(total < *len)
	{
		n = send(s, buf+total, bytesleft, 0);
		if ((n == -1) | (n == 0)) break;
		total += n;
		bytesleft -= n;
	}	
	*len = total;
	return ((n==-1) | (n == 0))?-1:0;
}
