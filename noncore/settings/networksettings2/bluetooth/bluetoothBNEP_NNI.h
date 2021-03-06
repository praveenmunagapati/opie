#ifndef BLUETOOTHBNEP_H
#define BLUETOOTHBNEP_H

#include <netnode.h>
#include "bluetoothBNEPdata.h"
#include "bluetoothBNEPrun.h"

class BluetoothBNEPNetNode;
class BluetoothBNEPEdit;

class ABluetoothBNEP : public ANetNodeInstance {

public :

      ABluetoothBNEP( BluetoothBNEPNetNode * PNN );

      RuntimeInfo * runtime( void )
        { return
           ( RT ) ? RT : ( RT = new BluetoothBNEPRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void )
        { return (void *)&Data; }

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      BluetoothBNEPEdit * GUI;
      BluetoothBNEPData Data;
      BluetoothBNEPRun * RT;

};

#endif
