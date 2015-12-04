// This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

/** external_python_procedure.h                                                          -*- C++ -*-
    Francois Maillet, 31 aout 2015
    Copyright (c) 2015 Datacratic Inc.  All rights reserved.

    External python procedure
*/

#pragma once

#include "mldb/server/procedure.h"
#include "mldb/types/value_description.h"
#include "mldb/server/plugin_resource.h"

namespace Datacratic {
namespace MLDB {

struct ExternalPythonProcedureConfig {
    ExternalPythonProcedureConfig()
    {
    }

    std::string stdInData;
    ScriptResource scriptConfig;
};

DECLARE_STRUCTURE_DESCRIPTION(ExternalPythonProcedureConfig);



/*****************************************************************************/
/* EXTERNAL PYTHON PROCEDURE                                                 */
/*****************************************************************************/

struct ExternalPythonProcedure: public Procedure {
    
    ExternalPythonProcedure(MldbServer * owner,
                   PolyConfig config,
                   const std::function<bool (const Json::Value &)> & onProgress);

    virtual RunOutput run(const ProcedureRunConfig & run,
                          const std::function<bool (const Json::Value &)> & onProgress) const;

    virtual Any getStatus() const;

    ExternalPythonProcedureConfig procedureConfig;

    std::shared_ptr<LoadedPluginResource> pluginRes;
};


} // namespace MLDB
} // namespace Datacratic
