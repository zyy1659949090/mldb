// This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

var dataset = mldb.createDataset({type:'sparse.mutable',id:'test'});

var ts = new Date("2015-01-01");

function recordExample(row, x, y, label)
{
    dataset.recordRow(row, [ [ "x", x, ts ], ["y", y, ts], ["label", label, ts] ]);
}

// Very simple linear regression, with x = y
recordExample("ex1", 0, 0, "cat");
recordExample("ex2", 1, 1, "dog");
recordExample("ex3", 1, 2, "cat");
recordExample("ex4", 6, 6, "poil");

dataset.commit()


function assertEqual(expr, val, msg)
{
    if (expr == val)
        return;
    if (JSON.stringify(expr) == JSON.stringify(val))
        return;

    throw "Assertion failure: " + msg + ": " + JSON.stringify(expr)
        + " not equal to " + JSON.stringify(val);
}

function succeeded(response)
{
    return response.responseCode >= 200 && response.responseCode < 400;
}

function assertSucceeded(process, response)
{
    plugin.log(process, response);

    if (!succeeded(response)) {
        throw process + " failed: " + JSON.stringify(response);
    }
}

function createAndRunProcedure(config, name)
{
    var createOutput = mldb.put("/v1/procedures/" + name, config);
    assertSucceeded(name + " creation", createOutput);

    // Run the training
    var trainingOutput = mldb.post("/v1/procedures/" + name + "/runs", {});
    assertSucceeded("procedure running", trainingOutput);
}

// transform our 4 elements with limit=3 (MLDB-799)
var transform_config = {
    type: 'transform',
    params: {
        inputDataset: { id: 'test' },
        outputDataset: { id: 'transformed', type: 'sparse.mutable' },
        select: 'x, y, x * 10 AS z, y + 6 AS q',
        rowName: "rowName() + '_transformed'",
        limit: 3
    }
};

createAndRunProcedure(transform_config, "transform");

var resp = mldb.get("/v1/datasets/transformed/query", {select: 'x,y,z,q', format: 'table'});

plugin.log("transform limit 3 query result", resp);

var expected = [
    [ "_rowName", "x", "y", "z", "q" ],
    [ "ex3_transformed", 1, 2, 10, 8 ],
    [ "ex1_transformed", 0, 0, 0, 6 ],
    [ "ex4_transformed", 6, 6, 60, 12 ]
];

assertEqual(mldb.diff(expected, resp.json, false /* strict */), {},
            "Output was not the same as expected output");

// transform our 4 elements with orderby rowName()
var transform_config2 = {
    type: 'transform',
    params: {
        inputDataset: { id: 'test' },
        outputDataset: { id: 'transformed2', type: 'sparse.mutable' },
        select: 'x, y, x * 10 AS z, y + 6 AS q',
        rowName: "rowName() + '_transformed'",
        orderBy: "rowName()"
    }
};

createAndRunProcedure(transform_config2, "transform2");

var resp = mldb.get("/v1/datasets/transformed2/query", {select: 'x,y,z,q', format: 'table', orderBy: 'rowName()'});

plugin.log(resp);

var expected = [
    [ "_rowName", "x", "y", "z", "q" ],
    [ "ex1_transformed", 0, 0, 0, 6  ],
    [ "ex2_transformed", 1, 1, 10, 7 ],
    [ "ex3_transformed", 1, 2, 10, 8 ],
    [ "ex4_transformed", 6, 6, 60, 12]
];

assertEqual(mldb.diff(expected, resp.json, false /* strict */), {},
            "Output was not the same as expected output");

"success"
