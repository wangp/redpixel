Summary: A violent game.
Name: redpixel
Version: 1.0
Release: 1
License: zlib
Packager: Peter Wang <tjaden@users.sf.net>
Group: Games/Arcade
Source: %{name}-%{version}.tar.gz
Buildroot: %{_tmppath}/%{name}-buildroot

%description
Red Pixel is a platform, deathmatch game for two players. It can be played
over a serial cable link or over a local area network.

%prep
%setup -q

%build
make

%install
rm -rf %{buildroot}
make prefix=%{buildroot}%{_prefix} \
	sharedir=%{buildroot}%{_prefix}/share/games/redpixel \
	install

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%doc LICENCE.txt
%{_prefix}/games/redpixel
%{_datadir}/games/redpixel

%changelog
* Wed Dec 12 2001 Peter Wang <tjaden@users.sf.net>  1.0-1
- created spec file
