/* This file is part of the KDE libraries
    Copyright (C) 1997 Christian Czezakte (e9025461@student.tuwien.ac.at)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//
//  KPROCESSCONTROLLER -- A helper class for KProcess
//
//  version 0.3.1, Jan, 8th 1997
//
//  (C) Christian Czezatke
//  e9025461@student.tuwien.ac.at
//  Ported by Holger Freyther
//

//#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <qsocketnotifier.h>
#include "oprocctrl.h"

using namespace Opie::Core::Internal;

OProcessController *OProcessController::theOProcessController = 0;

struct sigaction OProcessController::oldChildHandlerData;
bool OProcessController::handlerSet = false;

OProcessController::OProcessController()
{
    assert( theOProcessController == 0 );

    if (0 > pipe(fd))
        printf(strerror(errno));

    notifier = new QSocketNotifier(fd[0], QSocketNotifier::Read);
    notifier->setEnabled(true);
    QObject::connect(notifier, SIGNAL(activated(int)),
                     this, SLOT(slotDoHousekeeping(int)));
    connect( &delayedChildrenCleanupTimer, SIGNAL( timeout()),
             SLOT( delayedChildrenCleanup()));

    theOProcessController = this;

    setupHandlers();
}


void OProcessController::setupHandlers()
{
    if( handlerSet )
        return;
    struct sigaction act;
    act.sa_handler=theSigCHLDHandler;
    sigemptyset(&(act.sa_mask));
    sigaddset(&(act.sa_mask), SIGCHLD);
    // Make sure we don't block this signal. gdb tends to do that :-(
    sigprocmask(SIG_UNBLOCK, &(act.sa_mask), 0);

    act.sa_flags = SA_NOCLDSTOP;

    // CC: take care of SunOS which automatically restarts interrupted system
    // calls (and thus does not have SA_RESTART)

#ifdef SA_RESTART
    act.sa_flags |= SA_RESTART;
#endif

    sigaction( SIGCHLD, &act, &oldChildHandlerData );

    act.sa_handler=SIG_IGN;
    sigemptyset(&(act.sa_mask));
    sigaddset(&(act.sa_mask), SIGPIPE);
    act.sa_flags = 0;
    sigaction( SIGPIPE, &act, 0L);
    handlerSet = true;
}

void OProcessController::resetHandlers()
{
    if( !handlerSet )
        return;
    sigaction( SIGCHLD, &oldChildHandlerData, 0 );
    // there should be no problem with SIGPIPE staying SIG_IGN
    handlerSet = false;
}

// block SIGCHLD handler, because it accesses processList
void OProcessController::addOProcess( OProcess* p )
{
    sigset_t newset, oldset;
    sigemptyset( &newset );
    sigaddset( &newset, SIGCHLD );
    sigprocmask( SIG_BLOCK, &newset, &oldset );
    processList.append( p );
    sigprocmask( SIG_SETMASK, &oldset, 0 );
}

void OProcessController::removeOProcess( OProcess* p )
{
    sigset_t newset, oldset;
    sigemptyset( &newset );
    sigaddset( &newset, SIGCHLD );
    sigprocmask( SIG_BLOCK, &newset, &oldset );
    processList.remove( p );
    sigprocmask( SIG_SETMASK, &oldset, 0 );
}

//using a struct which contains both the pid and the status makes it easier to write
//and read the data into the pipe
//especially this solves a problem which appeared on my box where slotDoHouseKeeping() received
//only 4 bytes (with some debug output around the write()'s it received all 8 bytes)
//don't know why this happened, but when writing all 8 bytes at once it works here, aleXXX
struct waitdata
{
    pid_t pid;
    int status;
};

void OProcessController::theSigCHLDHandler(int arg)
{
    struct waitdata wd;
    //  int status;
    //  pid_t this_pid;
    int saved_errno;

    saved_errno = errno;
    // since waitpid and write change errno, we have to save it and restore it
    // (Richard Stevens, Advanced programming in the Unix Environment)

    bool found = false;
    if( theOProcessController != 0 )
    {
        // iterating the list doesn't perform any system call
        for( QValueList<OProcess*>::ConstIterator it = theOProcessController->processList.begin();
                it != theOProcessController->processList.end();
                ++it )
        {
            if( !(*it)->isRunning())
                continue;
            wd.pid = waitpid( (*it)->pid(), &wd.status, WNOHANG );
            if ( wd.pid > 0 )
            {
                ::write(theOProcessController->fd[1], &wd, sizeof(wd));
                found = true;
            }
        }
    }
    if( !found && oldChildHandlerData.sa_handler != SIG_IGN
            && oldChildHandlerData.sa_handler != SIG_DFL )
        oldChildHandlerData.sa_handler( arg ); // call the old handler
    // handle the rest
    if( theOProcessController != 0 )
    {
        static const struct waitdata dwd = { 0, 0 }
                                           ; // delayed waitpid()
        ::write(theOProcessController->fd[1], &dwd, sizeof(dwd));
    }
    else
    {
        int dummy;
        while( waitpid( -1, &dummy, WNOHANG ) > 0 )
            ;
    }

    errno = saved_errno;
}



void OProcessController::slotDoHousekeeping(int )
{
    unsigned int bytes_read = 0;
    unsigned int errcnt=0;
    // read pid and status from the pipe.
    struct waitdata wd;
    while ((bytes_read < sizeof(wd)) && (errcnt < 50))
    {
        int r = ::read(fd[0], ((char *)&wd) + bytes_read, sizeof(wd) - bytes_read);
        if (r > 0) bytes_read += r;
        else if (r < 0) errcnt++;
    }
    if (errcnt >= 50)
    {
        fprintf(stderr,
                "Error: Max. error count for pipe read "
                "exceeded in OProcessController::slotDoHousekeeping\n");
        return;           // it makes no sense to continue here!
    }
    if (bytes_read != sizeof(wd))
    {
        fprintf(stderr,
                "Error: Could not read info from signal handler %d <> %d!\n",
                bytes_read, sizeof(wd));
        return;           // it makes no sense to continue here!
    }
    if (wd.pid==0)
    { // special case, see delayedChildrenCleanup()
        delayedChildrenCleanupTimer.start( 1000, true );
        return;
    }

    for( QValueList<OProcess*>::ConstIterator it = processList.begin();
            it != processList.end();
            ++it )
    {
        OProcess* proc = *it;
        if (proc->pid() == wd.pid)
        {
            // process has exited, so do emit the respective events
            if (proc->run_mode == OProcess::Block)
            {
                // If the reads are done blocking then set the status in proc
                // but do nothing else because OProcess will perform the other
                // actions of processHasExited.
                proc->status = wd.status;
                proc->runs = false;
            }
            else
            {
                proc->processHasExited(wd.status);
            }
            return;
        }
    }
}

// this is needed e.g. for popen(), which calls waitpid() checking
// for its forked child, if we did waitpid() directly in the SIGCHLD
// handler, popen()'s waitpid() call would fail
void OProcessController::delayedChildrenCleanup()
{
    struct waitdata wd;
    while(( wd.pid = waitpid( -1, &wd.status, WNOHANG ) ) > 0 )
    {
        for( QValueList<OProcess*>::ConstIterator it = processList.begin();
                it != processList.end();
                ++it )
        {
            if( !(*it)->isRunning() || (*it)->pid() != wd.pid )
                continue;
            // it's OProcess, handle it
            ::write(fd[1], &wd, sizeof(wd));
            break;
        }
    }
}

OProcessController::~OProcessController()
{
    assert( theOProcessController == this );
    resetHandlers();

    notifier->setEnabled(false);

    close(fd[0]);
    close(fd[1]);

    delete notifier;
    theOProcessController = 0;
}

//#include "kprocctrl.moc"
