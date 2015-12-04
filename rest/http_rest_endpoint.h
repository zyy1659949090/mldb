// This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

/* http_rest_endpoint.h                                            -*- C++ -*-
   Jeremy Barnes, 9 November 2012
   Copyright (c) 2012 Datacratic.  All rights reserved.
*/

#pragma once

#include <atomic>
#include <memory>
#include <string>
#include "mldb/http/http_socket_handler.h"
#include "mldb/http/port_range_service.h"


namespace Datacratic {

/* Forward declarations */
struct EventLoop;
struct HttpHeader;
struct TcpAcceptor;


/*****************************************************************************/
/* HTTP REST ENDPOINT                                                        */
/*****************************************************************************/

/** An HTTP endpoint for REST calls over HTTP connections. */

struct HttpRestEndpoint {
    HttpRestEndpoint(EventLoop & ioService);
    virtual ~HttpRestEndpoint();

    /** Set the Access-Control-Allow-Origin: * HTTP header */
    void allowAllOrigins();

    void init();
    void shutdown();
    void closePeer();

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
    virtual std::string
    bindTcp(PortRange const & portRange, std::string host = "");
    
    /** Connection handler structure for the endpoint. */
    struct RestConnectionHandler: public HttpLegacySocketHandler {
        RestConnectionHandler(HttpRestEndpoint * endpoint, TcpSocket && socket);

        HttpRestEndpoint * endpoint;

        /** Disconnect handler. */
        std::function<void ()> onDisconnect;

        virtual void
        handleHttpPayload(const HttpHeader & header,
                          const std::string & payload);

        void sendErrorResponse(int code, const std::string & error);

        void sendErrorResponse(int code, const Json::Value & error);

        void sendResponse(int code,
                          const Json::Value & response,
                          const std::string & contentType = "application/json",
                          RestParams headers = RestParams());

        void sendResponse(int code,
                          const std::string & body,
                          const std::string & contentType,
                          RestParams headers = RestParams());

        void sendResponseHeader(int code,
                                const std::string & contentType,
                                RestParams headers = RestParams());

        /** Send an HTTP chunk with the appropriate headers back down the
            wire. */
        void sendHttpChunk(const std::string & chunk,
                           NextAction next = NEXT_CONTINUE,
                           OnWriteFinished onWriteFinished = OnWriteFinished());

        /** Send the entire HTTP payload.  Its length must match that of
            the response header.
        */
        void sendHttpPayload(const std::string & str);
    };

    typedef std::function<void (std::shared_ptr<RestConnectionHandler> connection,
                                const HttpHeader & header,
                                const std::string & payload)> OnRequest;

    OnRequest onRequest;

    std::vector<std::pair<std::string, std::string> > extraHeaders;

    std::unique_ptr<TcpAcceptor> acceptor_;
};

} // namespace Datacratic
