/*****************************************************
 M.Ali VARDAR	2005 - 12 - July

 GPL lisansý altýnda daðýtýlabilir.
 ali@linuxprogramlama.com - www.linuxprogramlama.com
 sws_web_server  0.1.7
 
 Uygulamaya emeði geçenler   -  Developers
 M.Ali VARDAR - ali@linuxprogramlama.com
 Mehmet GÖÇER - mgocer@sakarya.edu.tr
 Murat KOC - ?
 Ihsan TOPALOGLU - nietzche@turmail.com
******************************************************/

#include "ayardosyasi.h"

char *recvBuffer;

int dosya_oku_gonder (char *buf)
{
  cp = buf + 5;
  cp2 = strstr (cp, " HTTP");
  if (cp2 != NULL) *cp2 = '\0';

  if (buf[strlen (buf) - 1] == '/')
    {
      strcpy (cp, defaultsayfa);
      strcpy (home, homedizini);
      strcat (home, cp);
    }
  else
    {
      strcpy (home, homedizini);
      strcat (home, cp);
    }
  write_log (home);
  if (strstr (home, "../")) sprintf (home, "%s%s", homedizini, hatasayfasi);
  if ((f = fopen (home, "r")) == NULL)
    {
      if ((f = fopen (hatasayfasi, "r")) == NULL)
	{
	  //write_err_log ("Uyarý hatasayfasý bulunamadý veya acilamadi");
	  write_err_log ("Warning errorpage not found or cannot be opened");          
	  send (sock, hatamesaj, 160, 0);
	  return 0;
	}
    }

  while ((i = fread (buffer, 1, 4096, f)) > 0) 
	  {    	if (sendall (sock, buffer, &i) == -1) break;    }
  fclose (f);
  return 0;
}
/***********************************************************************************/
void sonlandir (int sig)
{
  close (sock);
  fclose (fdsws);
  fclose (fdsws_err);

  //syslog (LOG_INFO, "Uygulama kapatýldý.");
  syslog(LOG_INFO, "Application finished.");

  free(recvBuffer);
  exit (1);
}
/*************************************************************************/
void ayardosyasi_oku_parametre_ayarla ()
{
  dosya = fopen ("/etc/sws_web_server/sws_web_server.conf", "r");

  if (dosya == NULL)
    {
      //write_err_log ("/etc/sws_web_server/sws_web_server.conf dosyasý bulunamadý.!!!");
      write_err_log("/etc/sws_web_server/sws_web_server.conf file not found.!!!"); 
      exit (1);
    }

  while (!feof (dosya))
    {
      fgets (satir, 50, dosya);
      if (strcmp (satir, "#") != -1)
	{

//Port numarasý okunuyor
	  if (strstr (satir, "port"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      port = atoi (dizi);
	    }

//homedizini okunuyor
	  if (strstr (satir, "homedizini"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      strcpy (homedizini, dizi);
	    }

//defaultsayfa okunuyor
	  if (strstr (satir, "defaultsayfa"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      strcpy (defaultsayfa, dizi);
	    }

//hatasayfasi okunuyor
	  if (strstr (satir, "hatasayfasi"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      strcpy (hatasayfasi, dizi);
	    }

//maxbaglanti okunuyor
	  if (strstr (satir, "maxbaglanti"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      maxbaglanti = atoi (dizi);
	    }


//user ID okunuyor
	  if (strstr (satir, "userid"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      userid = atoi (dizi);
	    }

//group ID okunuyor
	  if (strstr (satir, "groupid"))
	    {
	      okunan = strtok (satir, " ");
	      okunan = strtok (NULL, ";");
	      strcpy (dizi, okunan);

	      groupid = atoi (dizi);
	    }

	}
    }
  fclose (dosya);
/*
printf("port:%d\n",port);
printf("defaultsayfa:%s\n",defaultsayfa);
printf("homedizini:%s\n",homedizini);
printf("hatalisayfa:%s\n",hatasayfasi);
printf("max:%d\n",maxbaglanti);
*/
}
/***********************************************************************************/
int main (int argc, char *argv[])
{
recvBuffer = (char *) malloc (4096);

  (void) signal (SIGINT, sonlandir);
  (void) signal (SIGILL, sonlandir);
  (void) signal (SIGTERM, sonlandir);

//log dosyalarini ac...
//open log files
  open_log_file ();

// /etc/sws_web_server/sws.conf reading and initialize variables
  ayardosyasi_oku_parametre_ayarla ();

//socket acýlýyor
//open socket
  serverSocket = socket (AF_INET, SOCK_STREAM, 0);

  if (serverSocket == -1)
    {
	//write_err_log ("Geçersiz soket");
	write_err_log ("Wrong socket");
	free(recvBuffer);
	exit (1);
    }

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons (port);
  serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  memset (&(serverAddr.sin_zero), 0, 8);

  rc = bind (serverSocket, (struct sockaddr *) &serverAddr, sizeof (struct sockaddr));
  if (rc == -1)
    {
      //write_err_log ("Port kullanýmda ...");
      write_err_log("Port using ...");
      free(recvBuffer);
      exit (1);
    }

  rc = listen (serverSocket, maxbaglanti);

  //Sadece uygulama baþlama ve bitiþ bilgiler system loglarýna yazýldý
  //syslog (LOG_INFO, "Uygulama baþladý.");
  syslog(LOG_INFO, "Application started.");

//kullanýcý ID ayarlanýyor
//please control /etc/sws_web_server/sws_web_server.conf file
  setuid (userid);
  seteuid (userid);
  setgid (groupid);
  setegid (groupid);

  while (1)
    {
      clientAddrSize = sizeof (struct sockaddr_in);
      do
	sock =  accept (serverSocket, (struct sockaddr *) &clientAddr,  &clientAddrSize);
	//while ((sock == -1) && (errno == EINTR));
      while (sock == -1);
      
      i = recv (sock, recvBuffer, 4096, 0);
      if (i == -1)
	{
	 //write_err_log ("Okuma hatalý...");
	  write_err_log("Reading wrong...");                    
	  close (sock);	  
	  //continue;
	}
      recvBuffer[i] = '\0';

        if (maxbag<maxbaglanti)
	{
          if (!fork ())      
	    {
	    maxbag++;
	    dosya_oku_gonder (recvBuffer);
	    close (sock);
	    maxbag--;	    
	    exit (0);
	    }
	}

      while (waitpid (-1, NULL, WNOHANG) > 0);
      close (sock);
      // write_log ("Baðlantý kesildi.");
      write_log("Connection closed.");
    }

  free(recvBuffer);
  return 0;

}
