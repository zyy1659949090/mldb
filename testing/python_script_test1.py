# This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

# Test python script

print "Hello, world!"

print mldb.script

mldb.log(str(mldb.script))
mldb.log("Hello, world")
