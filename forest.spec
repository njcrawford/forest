Name: forest
Version: 1.0.2
Release: 0%{?dist}
Summary: A whole network update manager
Group: Applications/System
License: GPLv2
URL: http://www.njcrawford.com/forest/
Source0: http://www.njcrawford.com/download2/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
%define debug_package %{nil}

%description
A whole network update manager

%prep
%setup -q
cd client
./configure
make

%install

rm -rf %{buildroot}

# make dirs for server
install -m 0755 -d %{buildroot}%{_datadir}/forest
install -m 0755 -d %{buildroot}%{_datadir}/forest/www
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/jquery
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/helpers
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/language
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/language/english
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/mssql
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/sqlite
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/oci8
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/postgre
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/mysql
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/odbc
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/database/drivers/mysqli
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/core
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/fonts
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/libraries
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/libraries/Cache
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/libraries/Cache/drivers
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/system/libraries/javascript
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/css
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/helpers
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/cache
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/hooks
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/language
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/language/english
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/models
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/logs
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/config
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/controllers
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/core
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/third_party
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/errors
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/libraries
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/application/views
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/inc

# install server files
install -m 0755 server/summary-email.php %{buildroot}%{_datadir}/forest
install -m 0755 server/upgrade_db.php %{buildroot}%{_datadir}/forest
install -m 0644 server/db_schema_v1.sql %{buildroot}%{_datadir}/forest
install -m 0644 server/db_schema_v2.sql %{buildroot}%{_datadir}/forest
install -m 0644 server/db_schema_v3.sql %{buildroot}%{_datadir}/forest
install -m 0644 server/www/*.php %{buildroot}%{_datadir}/forest/www
install -m 0644 server/www/inc/*.php %{buildroot}%{_datadir}/forest/www/inc
install -m 0644 server/www/system/helpers/*.php %{buildroot}%{_datadir}/forest/www/system/helpers
install -m 0644 server/www/system/language/english/*.php %{buildroot}%{_datadir}/forest/www/system/language/english
install -m 0644 server/www/system/database/drivers/mssql/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/mssql
install -m 0644 server/www/system/database/drivers/sqlite/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/sqlite
install -m 0644 server/www/system/database/drivers/oci8/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/oci8
install -m 0644 server/www/system/database/drivers/postgre/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/postgre
install -m 0644 server/www/system/database/drivers/mysql/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/mysql
install -m 0644 server/www/system/database/drivers/odbc/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/odbc
install -m 0644 server/www/system/database/drivers/mysqli/*.php %{buildroot}%{_datadir}/forest/www/system/database/drivers/mysqli
install -m 0644 server/www/system/database/*.php %{buildroot}%{_datadir}/forest/www/system/database
install -m 0644 server/www/system/core/*.php %{buildroot}%{_datadir}/forest/www/system/core
install -m 0644 server/www/system/libraries/Cache/drivers/*.php %{buildroot}%{_datadir}/forest/www/system/libraries/Cache/drivers
install -m 0644 server/www/system/libraries/Cache/*.php %{buildroot}%{_datadir}/forest/www/system/libraries/Cache
install -m 0644 server/www/system/libraries/javascript/*.php %{buildroot}%{_datadir}/forest/www/system/libraries/javascript
install -m 0644 server/www/system/libraries/*.php %{buildroot}%{_datadir}/forest/www/system/libraries
install -m 0644 server/www/application/models/*.php %{buildroot}%{_datadir}/forest/www/application/models
install -m 0644 server/www/application/config/*.php %{buildroot}%{_datadir}/forest/www/application/config
install -m 0644 server/www/application/controllers/*.php %{buildroot}%{_datadir}/forest/www/application/controllers
install -m 0644 server/www/application/errors/*.php %{buildroot}%{_datadir}/forest/www/application/errors
install -m 0644 server/www/application/views/*.php %{buildroot}%{_datadir}/forest/www/application/views
install -m 0644 server/www/css/forest.css %{buildroot}%{_datadir}/forest/www/css

# install server cron job
install -m 0755 -d %{buildroot}%{_sysconfdir}/cron.d
install -m 0644 server/cron-example-centos5 %{buildroot}%{_sysconfdir}/cron.d/forest-server

# install server config file
install -m 0644 server/forest-server.conf %{buildroot}%{_sysconfdir}/forest-server.conf

# make dir for client
install -m 0755 -d %{buildroot}%{_sbindir}

#install client executable
install -m 0755 client/forest-client %{buildroot}%{_sbindir}/forest-client

#install client cron job
install -m 0755 -d %{buildroot}%{_sysconfdir}/cron.daily
install -m 0755 client/cron-example-centos5 %{buildroot}%{_sysconfdir}/cron.daily/forest-client

#install client config file
install -m 0644 client/forest-client.conf %{buildroot}%{_sysconfdir}/forest-client.conf

%clean
rm -rf %{buildroot}

%changelog
* Wed Sep 21 2011 Nathan Crawford <njcrawford@gmail.com>
- 1.0.2 release

* Tue Aug 23 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.11 release

* Fri Jul 22 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.10 release

* Tue Jun 22 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.9 release

* Tue Jun 07 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.7 release

* Wed May 13 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.5 release

* Wed May 11 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.4 release

* Mon Apr 11 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.3 release

* Thu Mar 17 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.2 release

* Wed Mar 16 2011 Nathan Crawford <njcrawford@gmail.com>
- 0.8.1 release

%package server
Summary: A whole network update manager (server)
Group: Applications/System
%description server
A whole network update manager (server)

%files server
%defattr(-,root,root)
%dir %{_datadir}/forest
%dir %{_datadir}/forest/www
%dir %{_datadir}/forest/www/jquery
%dir %{_datadir}/forest/www/system
%dir %{_datadir}/forest/www/system/helpers
%dir %{_datadir}/forest/www/system/language
%dir %{_datadir}/forest/www/system/language/english
%dir %{_datadir}/forest/www/system/database
%dir %{_datadir}/forest/www/system/database/drivers
%dir %{_datadir}/forest/www/system/database/drivers/mssql
%dir %{_datadir}/forest/www/system/database/drivers/sqlite
%dir %{_datadir}/forest/www/system/database/drivers/oci8
%dir %{_datadir}/forest/www/system/database/drivers/postgre
%dir %{_datadir}/forest/www/system/database/drivers/mysql
%dir %{_datadir}/forest/www/system/database/drivers/odbc
%dir %{_datadir}/forest/www/system/database/drivers/mysqli
%dir %{_datadir}/forest/www/system/core
%dir %{_datadir}/forest/www/system/fonts
%dir %{_datadir}/forest/www/system/libraries
%dir %{_datadir}/forest/www/system/libraries/Cache
%dir %{_datadir}/forest/www/system/libraries/Cache/drivers
%dir %{_datadir}/forest/www/system/libraries/javascript
%dir %{_datadir}/forest/www/css
%dir %{_datadir}/forest/www/application
%dir %{_datadir}/forest/www/application/helpers
%dir %{_datadir}/forest/www/application/cache
%dir %{_datadir}/forest/www/application/hooks
%dir %{_datadir}/forest/www/application/language
%dir %{_datadir}/forest/www/application/language/english
%dir %{_datadir}/forest/www/application/models
%dir %{_datadir}/forest/www/application/logs
%dir %{_datadir}/forest/www/application/config
%dir %{_datadir}/forest/www/application/controllers
%dir %{_datadir}/forest/www/application/core
%dir %{_datadir}/forest/www/application/third_party
%dir %{_datadir}/forest/www/application/errors
%dir %{_datadir}/forest/www/application/libraries
%dir %{_datadir}/forest/www/application/views
%dir %{_datadir}/forest/www/inc
%{_datadir}/forest/*
%{_datadir}/forest/www/*
%{_datadir}/forest/www/inc/*
%{_datadir}/forest/www/system/helpers/*
%{_datadir}/forest/www/system/language/english/*
%{_datadir}/forest/www/system/database/drivers/mssql/*
%{_datadir}/forest/www/system/database/drivers/sqlite/*
%{_datadir}/forest/www/system/database/drivers/oci8/*
%{_datadir}/forest/www/system/database/drivers/postgre/*
%{_datadir}/forest/www/system/database/drivers/mysql/*
%{_datadir}/forest/www/system/database/drivers/odbc/*
%{_datadir}/forest/www/system/database/drivers/mysqli/*
%{_datadir}/forest/www/system/database/*
%{_datadir}/forest/www/system/core/*
%{_datadir}/forest/www/system/libraries/Cache/drivers/*
%{_datadir}/forest/www/system/libraries/Cache/*
%{_datadir}/forest/www/system/libraries/javascript/*
%{_datadir}/forest/www/system/libraries/*
%{_datadir}/forest/www/application/models/*
%{_datadir}/forest/www/application/config/*
%{_datadir}/forest/www/application/controllers/*
%{_datadir}/forest/www/application/errors/*
%{_datadir}/forest/www/application/views/*
%{_datadir}/forest/www/css/*
%config(noreplace) %{_sysconfdir}/forest-server.conf
%{_sysconfdir}/cron.d/forest-server

%package client
Summary: A whole network update manager (client)
Group: Applications/System
BuildRequires: curl-devel gcc-c++
Requires: curl
%description client
A whole network update manager (client)

%files client
%defattr(-,root,root)
%{_sbindir}/forest-client
%{_sysconfdir}/cron.daily/forest-client
%config(noreplace) %{_sysconfdir}/forest-client.conf


