// This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

/**
 * csv_export_procedure.h
 * Mich, 2015-11-11
 * Copyright (c) 2015 Datacratic Inc. All rights reserved.
 **/

#pragma once
#include "mldb/server/procedure.h"
#include "mldb/server/function.h"
#include "mldb/server/dataset.h"
#include "mldb/sql/sql_expression.h"

namespace Datacratic {
namespace MLDB {

struct CsvExportProcedureConfig : ProcedureConfig {
    CsvExportProcedureConfig();
    SelectExpression select;
    std::shared_ptr<TableExpression> inputDataset;
    std::shared_ptr<TableExpression> outputDataset;
    OrderByExpression orderBy;
    WhenExpression when;
    std::shared_ptr<SqlExpression> where;
    TupleExpression groupBy;
    std::shared_ptr<SqlExpression> having;
    ssize_t offset;
    ssize_t limit;
    Url dataFileUrl;
    bool headers;
    std::string delimiter;
    std::string quoteChar;
};
DECLARE_STRUCTURE_DESCRIPTION(CsvExportProcedureConfig);

struct CsvExportProcedure: public Procedure {

    CsvExportProcedure(
        MldbServer * owner,
        PolyConfig config,
        const std::function<bool (const Json::Value &)> & onProgress);

    virtual RunOutput run(
        const ProcedureRunConfig & run,
        const std::function<bool (const Json::Value &)> & onProgress) const;

    virtual Any getStatus() const;

    CsvExportProcedureConfig procedureConfig;
};

} // namespace MLDB
} // namespace Datacratic

