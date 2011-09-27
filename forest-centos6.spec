Name: forest
Version: 0.8.10
Release: 0%{?dist}centos6
Summary: A whole network update manager
Group: Applications/System
License: GPLv2
URL: http://www.njcrawford.com/forest/
Source0: http://www.njcrawford.com/download2/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildArch: noarch

%description
A whole network update manager

%prep
%setup -q

%install

rm -rf %{buildroot}

install -m 0755 -d %{buildroot}%{_datadir}/forest
install -m 0755 -d %{buildroot}%{_datadir}/forest/www
install -m 0755 -d %{buildroot}%{_datadir}/forest/www/inc
install -m 0755 -d %{buildroot}%{_sbindir}

install -m 0755 server/summary-email.php %{buildroot}%{_datadir}/forest
install -m 0755 server/upgrade_db.php %{buildroot}%{_datadir}/forest
install -m 0644 server/db_schema_v1.sql %{buildroot}%{_datadir}/forest
install -m 0644 server/db_schema_v2.sql %{buildroot}%{_datadir}/forest
install -m 0644 server/db_schema_v3.sql %{buildroot}%{_datadir}/forest

install -m 0644 server/www/*.php %{buildroot}%{_datadir}/forest/www
install -m 0644 server/www/*.css %{buildroot}%{_datadir}/forest/www
install -m 0644 server/www/inc/*.php %{buildroot}%{_datadir}/forest/www/inc

install -m 0755 -d %{buildroot}%{_sysconfdir}/cron.d
install -m 0644 server/cron-example-centos5 %{buildroot}%{_sysconfdir}/cron.d/forest-server

install -m 0644 server/forest-server.conf %{buildroot}%{_sysconfdir}/forest-server.conf

install -m 0755 client/forest-client %{buildroot}%{_sbindir}/forest-client

install -m 0755 -d %{buildroot}%{_sysconfdir}/cron.daily
install -m 0755 client/cron-example-centos5 %{buildroot}%{_sysconfdir}/cron.daily/forest-client

install -m 0644 client/forest-client.conf %{buildroot}%{_sysconfdir}/forest-client.conf

%clean
rm -rf %{buildroot}

%changelog
* Tue Aug 23 2011 Nathan Crawford <njcrawford@gmail.com>
- CentOS 6 specific release of 0.8.10

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
%dir %{_datadir}/forest/www/inc
%{_datadir}/forest/*
%{_datadir}/forest/www/*
%{_datadir}/forest/www/inc/*
%config(noreplace) %{_sysconfdir}/forest-server.conf
%{_sysconfdir}/cron.d/forest-server

%package client
Summary: A whole network update manager (client)
Group: Applications/System
%description client
A whole network update manager (client)

%files client
%defattr(-,root,root)
%{_sbindir}/forest-client
%{_sysconfdir}/cron.daily/forest-client
%config(noreplace) %{_sysconfdir}/forest-client.conf


