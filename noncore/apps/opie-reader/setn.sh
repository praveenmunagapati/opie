red='\e[0;31m'
RED='\e[1;31m'
blue='\e[0;34m'
BLUE='\e[1;34m'
cyan='\e[0;36m'
CYAN='\e[1;36m'
NC='\e[0m'              # No Color

if test -z $OLDPATH; then
    export OLDPATH=$PATH
fi

if test -z $OLD_LD_LIBRARY_PATH; then
    export OLD_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi

#export QPEDIR=/opt/Qtopia
export QTDIR=/home/tim/nevyos/qt-embedded-free-3.1.1
export PATH=$QTDIR/bin:$OLDPATH
export TMAKEPATH=/opt/Qtopia/tmake/lib/qws/linux-generic-g++
export LD_LIBRARY_PATH=$QTDIR/lib:$OLD_LD_LIBRARY_PATH

#PS1="[${cyan}\T$NC][${RED}Laptop${NC}]\W> "
#PS1="[${RED}NevyOS${NC}]\W> "
PS1="[NevyOS]\W> "

echo -e \\n${RED}NevyOS${NC} environment configured\\n