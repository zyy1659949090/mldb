# This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

$(eval $(call test,bucketing_probabilizer_test,ml,boost))
$(eval $(call test,kmeans_test,ml test_utils,boost))
