%define __libtoolize true
%define name ksubtitleripper
%define version 0.3
%define release 1plf

Name: %{name}
Version: %{version}
Release: %{release}
Summary: Application to extract DVD subtitles to text
Source: http://download.berlios.de/ksubtitleripper/%{name}-%{version}.tar.bz2
URL: http://ksubtitleripper.berlios.de/
License: GPL
Group: Video
BuildRoot: %{_tmppath}/%{name}-buildroot
Requires: subtitleripper gocr transcode

%description
KSubtitleRipper is a GUI for KDE to rip DVD subtitles. Extracts
subtitles from .vob files and creates subtitles in SRT format.

This package is in PLF because it depends on a dvd-ripping tool.

%prep
rm -rf $RPM_BUILD_ROOT
%setup

%build
%configure
%make

%install
%makeinstall
install -d $RPM_BUILD_ROOT/%{_menudir}
cat << EOF > $RPM_BUILD_ROOT/%{_menudir}/%{name}
?package(%{name}):command="%{_bindir}/%{name}" icon="%{name}.png" \
                needs="X11" section="Multimedia/Video" title="KSubtitleRipper" \
                longtitle="Subtitle Ripper"
EOF

install -d $RPM_BUILD_ROOT/%{_miconsdir}
install -d $RPM_BUILD_ROOT/%{_iconsdir}
install -d $RPM_BUILD_ROOT/%{_liconsdir}
cp $RPM_BUILD_ROOT/%{_datadir}/icons/hicolor/16x16/apps/ksubtitleripper.png \
	$RPM_BUILD_ROOT/%{_miconsdir}/ksubtitleripper.png
cp $RPM_BUILD_ROOT/%{_datadir}/icons/hicolor/32x32/apps/ksubtitleripper.png \
	$RPM_BUILD_ROOT/%{_iconsdir}/ksubtitleripper.png
cp $RPM_BUILD_ROOT/%{_datadir}/icons/hicolor/48x48/apps/ksubtitleripper.png \
	$RPM_BUILD_ROOT/%{_liconsdir}/ksubtitleripper.png

%find_lang %{name}

%clean
rm -rf %{buildroot}

%post
%{update_menus}

%postun
%{clean_menus}

%files -f %{name}.lang
%defattr(-, root, root, 0755)
%doc AUTHORS COPYING ChangeLog NEWS README TODO
%{_bindir}/%{name}
%{_menudir}/%{name}
%{_datadir}/apps/%{name}/ksubtitleripperui.rc
%{_miconsdir}/%{name}.png
%{_iconsdir}/%{name}.png
%{_liconsdir}/%{name}.png
%{_datadir}/icons/hicolor/16x16/apps/%{name}.png
%{_datadir}/icons/hicolor/32x32/apps/%{name}.png
%{_datadir}/icons/hicolor/48x48/apps/%{name}.png
%{_datadir}/icons/hicolor/32x32/actions/*
%{_datadir}/applnk/Utilities/%{name}.desktop
%{_datadir}/doc/HTML/en/%{name}/*

%changelog
* Fri Apr 08 2005 Sergio Cambra <runico@users.berlios.de> 0.3-1plf
- Release 0.3

* Thu Aug 19 2004 Sergio Cambra <runico@users.berlios.de> 0.2-1plf
- Release 0.2
- Change Copyright to License, add transcode to Requires and fix changelog

* Fri Apr 02 2004 Sergio Cambra <runico@users.berlios.de> 0.1-2mdk
- Menu file added

* Fri Apr 02 2004 Sergio Cambra <runico@users.berlios.de> 0.1-1mdk
- Initial release
