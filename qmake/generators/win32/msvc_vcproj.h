/****************************************************************************
** $Id: msvc_vcproj.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of VcprojGenerator class.
**
** Created : 970521
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the network module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef __VCPROJMAKE_H__
#define __VCPROJMAKE_H__

#include "winmakefile.h"
#include "msvc_objectmodel.h"

enum target {
    Application,
    SharedLib,
    StaticLib
};

class VcprojGenerator : public Win32MakefileGenerator
{
    bool init_flag;
    bool writeVcprojParts(QTextStream &);

    bool writeMakefile(QTextStream &);
    virtual void writeSubDirs(QTextStream &t);
    QString findTemplate(QString file);
    void init();

public:
    VcprojGenerator(QMakeProject *p);
    ~VcprojGenerator();

    QString defaultMakefile() const;
    virtual bool doDepends() const { return FALSE; } //never necesary

protected:
    virtual bool openOutput(QFile &file) const;
    virtual void processPrlVariable(const QString &, const QStringList &);
    virtual bool findLibraries();
    virtual void outputVariables();
    
    void initOld();
    void initProject();
    void initConfiguration();
    void initCompilerTool();
    void initLinkerTool();
    void initLibrarianTool();
    void initIDLTool();
    void initCustomBuildTool();
    void initPreBuildEventTools();
    void initPostBuildEventTools();
    void initPreLinkEventTools();
    void initSourceFiles();
    void initHeaderFiles();
    void initMOCFiles();
    void initUICFiles();
    void initFormsFiles();
    void initTranslationFiles();
    void initLexYaccFiles();
    void initResourceFiles();

    /*
    void writeGuid( QTextStream &t );
    void writeAdditionalOptions( QTextStream &t );
    void writeHeaders( QTextStream &t );
    void writeSources( QTextStream &t );
    void writeMocs( QTextStream &t );
    void writeLexs( QTextStream &t );
    void writeYaccs( QTextStream &t );
    void writePictures( QTextStream &t );
    void writeImages( QTextStream &t );
    void writeIDLs( QTextStream &t );

    void writeForms( QTextStream &t );
    void writeFormsSourceHeaders( QString &variable, QTextStream &t );
    void writeTranslations( QTextStream &t );
    void writeStrippedTranslations( QTextStream &t );
    */

    VCProject vcProject;
    target projectTarget;

    friend class VCFilter;
};

inline VcprojGenerator::~VcprojGenerator()
{ }

inline QString VcprojGenerator::defaultMakefile() const
{
    return project->first("TARGET") + project->first("VCPROJ_EXTENSION");
}

inline bool VcprojGenerator::findLibraries()
{ 
    return Win32MakefileGenerator::findLibraries("MSVCVCPROJ_LIBS");
}

#endif /* __VCPROJMAKE_H__ */
