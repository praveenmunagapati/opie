TEMPLATE        = lib
CONFIG          += qte warn_on 

HEADERS         = qcolordialog.h       \
                  qsplitter.h          \
                  qinputdialog.h       \
                                       \
                  ocolorpopupmenu.h    \
                  ocolorbutton.h       


SOURCES         = qcolordialog.cpp     \
                  qsplitter.cpp        \
                  qinputdialog.cpp     \
                                       \
                  ocolorpopupmenu.cpp  \
                  ocolorbutton.cpp    

TARGET          = qtaux2
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib


include ( $(OPIEDIR)/include.pro )
