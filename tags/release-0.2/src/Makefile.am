## Makefile.am for ksubtitleripper

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ksubtitleripper

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# the library search path. 
ksubtitleripper_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
ksubtitleripper_LDADD   = $(LIB_KFILE) $(LIB_KDEPRINT)

# which sources should be compiled for ksubtitleripper
ksubtitleripper_SOURCES = main.cpp ksubtitleripper.cpp ksubtitleripperviewdlg.ui ksubtitleripperview.cpp project.cpp newproject.cpp convertdialog.cpp extractdialog.cpp previewdialog.cpp extractprocess.cpp configuration.cpp prefgenerallayout.ui prefdialog.cpp prefgeneral.cpp createsrt.cpp

# these are the headers for your project
noinst_HEADERS = ksubtitleripper.h ksubtitleripperview.h project.h newproject.h convertdialog.h extractdialog.h previewdialog.h extractprocess.h configuration.h prefdialog.h prefgeneral.h createsrt.h

# client stuff

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ksubtitleripper.pot

KDE_ICON = ksubtitleripper

# this is where the kdelnk file will go 
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = ksubtitleripper.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/ksubtitleripper
rc_DATA = ksubtitleripperui.rc
