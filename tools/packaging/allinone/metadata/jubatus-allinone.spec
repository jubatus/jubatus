Name:		%{user_dist_name}
Version:	%{user_dist_version}
Release:	%{user_dist_release}%{?dist}
Summary:	Distributed Online Machine Learning Framework
Vendor:		PFN & NTT
Group:		System Environment/Daemons
License:	LGPL 2.1
URL:		http://jubat.us/

BuildRequires:  log4cxx-devel = 0.10.0
BuildRequires:  opencv-devel >= 2.3

Requires:       log4cxx = 0.10.0
Requires:       opencv >= 2.3

%description
Jubatus is a distributed processing framework and streaming machine learning library.

%files
%defattr(-,root,root,-)
/*
