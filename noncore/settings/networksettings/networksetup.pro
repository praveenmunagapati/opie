#DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= mainwindowimp.h addconnectionimp.h interface.h interfaceinformationimp.h interfacesetupimp.h interfaces.h defaultmodule.h  kprocctrl.h module.h  kprocess.h
SOURCES		= main.cpp mainwindowimp.cpp addconnectionimp.cpp interface.cpp interfaceinformationimp.cpp  interfacesetupimp.cpp kprocctrl.cpp kprocess.cpp interfaces.cpp
#INCLUDEPATH	+= $(OPIEDIR)/include
#DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= mainwindow.ui addconnection.ui interfaceinformation.ui interfaceadvanced.ui interfacesetup.ui
TARGET		= networksetup
