// This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

/* http_named_endpoint.h                                           -*- C++ -*-
   Jeremy Barnes, 9 November 2012
   Copyright (c) 2012 Datacratic.  All rights reserved.
*/

#pragma once

#include "mldb/http/http_endpoint.h"
#include "mldb/jml/utils/vector_utils.h"
#include "named_endpoint.h"
#include "http_rest_proxy.h"
#include "http_rest_endpoint.h"


namespace Datacratic {



/*****************************************************************************/
/* HTTP NAMED ENDPOINT                                                       */
/*****************************************************************************/

/** A message loop-compatible endpoint for http connections. */

struct HttpNamedEndpoint : public NamedEndpoint, public HttpRestEndpoint {

    HttpNamedEndpoint();

    void init(std::shared_ptr<ConfigurationService> config,
              const std::string & endpointName);

    /** Bid into a given address.  Address is host:port.

        If no port is given (and no colon), than use any port number.
        If port is a number and then "+", then scan for any port higher than
        the given number.
        If host is empty or "*", then use all interfaces.
    */
    std::string
    bindTcpAddress(const std::string & address);

    /** Bind into a specific tcp port.  If the port is not available, it will
        throw an exception.

        Returns the uri to connect to.
    */
    std::string
    bindTcpFixed(std::string host, int port);

    /** Bind into a tcp port.  If the preferred port is not available, it will
        scan until it finds one that is.

        Returns the uri to connect to.
    */
    std::string
    bindTcp(PortRange const & portRange, std::string host = "");

};

} // namespace Datacratic

