#ifndef NETWORK_NETNODE_H
#define NETWORK_NETNODE_H

#include "netnode.h"

class ANetwork;

class NetworkNetNode : public ANetNode{

    Q_OBJECT

public:

    NetworkNetNode();
    virtual ~NetworkNetNode();

    virtual const QString pixmapName() 
      { return "tcpip"; }

    virtual const QString nodeName() 
      { return tr("IP Configuration"); }

    virtual const QString nodeDescription() ;

    virtual ANetNodeInstance * createInstance( void );

    virtual const char ** needs( void );
    virtual const char * provides( void );

    virtual bool generateProperFilesFor( ANetNodeInstance * NNI );
    virtual bool hasDataFor( const QString & S );
    virtual bool generateDataForCommonFile( 
        SystemFile & SF, long DevNr, ANetNodeInstance * NNI );

private:

};

extern "C"
{
  void create_plugin( QList<ANetNode> & PNN );
};

#endif