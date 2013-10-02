Forest
======

Forest is a system to manage updates on mixed OS networks.

The client gathers information about available updates and reports them to the
server over http(s). Updates can then be accepted via a web interface on the
server. The system can also report that a client needs to be rebooted. For more
information, visit [http://www.njcrawford.com/programs/forest/](http://www.njcrawford.com/programs/forest/).

Both client and server depend on cron to run daily reports. The server depends
on Codeigniter 3.0. Codeigniter can be installed anywhere on the server, just
configure the path in index.php.

Minimal build instructions
--------------------------

Under Linux:

1.  Install dependencies (On Ubuntu: libcurl4-gnutls-dev)
2.  Run ./configure
3.  Run make

Under Windows:

1.  Browse to client folder and run configure.bat
2.  Build curl as a release DLL according to instructions at curl-source/winbuild/BUILD.WINDOWS.txt (solution and post-build.bat expect options to be mode=dll vc=10)
3.  Open forest-client solution and build


Requirements for running the server
-----------------------------------

1.  Mysql server
2.  Apache server
3.  PHP5 and Apache mod_php
4.  For nicer URLs, Apache mod_rewrite
5.  For daily status emails, cron
