TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= mediumwidget.h mediumglobal.h mainwindow.h
SOURCES		= main.cpp mediumwidget.cc mediumglobal.cc mainwindow.cc
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
