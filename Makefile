#M.Ali VARDAR ali@linuxprogramlama.com

all:	 sws_web_server

sws_web_server:	sws_web_server.c
	gcc -O3 -w -Wall -I/usr/include -I/usr/local/include -o sws_web_server sws_web_server.c
	strip sws_web_server

clean:
	rm -rf sws_web_server
	rm -rf *.o

install:
	mkdir /etc/sws_web_server
	mkdir /var/run/sws_web_server
	mkdir /var/log/sws_web_server
	mkdir /var/sws_web_server
	mkdir /var/sws_web_server/html
	chmod 755 /var/sws_web_server
	chmod 755 /var/sws_web_server/html
	cp ./sws_web_server.conf /etc/sws_web_server/sws_web_server.conf
	cp ./error.html /var/sws_web_server/error.html
	cp ./index.html /var/sws_web_server/html/index.html
	cp ./sws_web_server /usr/local/bin
	cp ./sws_web_server.init /etc/init.d
	
uninstall:
	rm -rf /etc/sws_web_server
	rm -rf /usr/local/bin/sws_web_server
	rm -rf /var/run/sws_web_server
	rm -rf /var/log/sws_web_server
	rm -rf /var/sws_web_server	
	rm -rf /etc/sws_web_server.init
