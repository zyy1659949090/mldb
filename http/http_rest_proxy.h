/* http_rest_proxy.h                                               -*- C++ -*-
   Jeremy Barnes, 10 April 2013
   Copyright (c) 2013 Datacratic Inc.  All rights reserved.

   This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.
*/

#pragma once

#include <mutex>

#include "mldb/jml/utils/vector_utils.h"
#include "mldb/base/exc_assert.h"
#include "mldb/jml/utils/string_functions.h"
#include "mldb/types/value_description.h"
#include "mldb/http/http_header.h"


namespace Datacratic {

/*****************************************************************************/
/* HTTP REST PROXY                                                           */
/*****************************************************************************/

/** A class that can be used to perform queries against an HTTP service.
    Normally used to consume REST-like APIs, hence the name.
*/

struct HttpRestProxy {
    HttpRestProxy(const std::string & serviceUri = "")
        : serviceUri(serviceUri), noSSLChecks(false), debug(false)
    {
    }

    void init(const std::string & serviceUri)
    {
        this->serviceUri = serviceUri;
    }

    ~HttpRestProxy();

    /** The response of a request.  Has a return code and a body. */
    struct Response {
        Response()
            : code_(0), errorCode_(0)
        {
        }

        /** Return code of the REST call. */
        int code() const {
            return code_;
        }

        /** Body of the REST call. */
        std::string body() const
        {
            return body_;
        }

        Json::Value jsonBody() const
        {
            return Json::parse(body_);
        }

        /** Get the given response header of the REST call. */
        const std::pair<const std::string, std::string> *
        hasHeader(const std::string & name) const
        {
            auto it = header_.headers.find(name);
            if (it == header_.headers.end())
                it = header_.headers.find(ML::lowercase(name));
            if (it == header_.headers.end())
                return nullptr;
            return &(*it);
        }

        /** Get the given response header of the REST call. */
        std::string getHeader(const std::string & name) const
        {
            auto p = hasHeader(name);
            if (!p)
                throw ML::Exception("required header " + name + " not found");
            return p->second;
        }
        
        long code_;
        std::string body_;
        HttpHeader header_;

        /// Error code for request, normally a CURL code, 0 is OK
        int errorCode_;

        /// Error string for an error request, empty is OK
        std::string errorMessage_;
    };

    /** Add a cookie to the connection that comes in from the response. */
    void setCookieFromResponse(const Response& r)
    {
        cookies.push_back("Set-Cookie: " + r.getHeader("set-cookie"));
    }

    /** Add a cookie to the connection. */
    void setCookie(const std::string & value)
    {
        cookies.push_back("Set-Cookie: " + value);
    }

    /** Structure used to hold content for a POST request. */
    struct Content {
        Content()
            : data(0), size(0), hasContent(false)
        {
        }

        Content(const std::string & str,
                const std::string & contentType = "")
            : str(str), data(str.c_str()), size(str.size()),
              hasContent(true), contentType(contentType)
        {
        }

        Content(const char * data, uint64_t size,
                const std::string & contentType = "",
                const std::string & contentMd5 = "")
            : data(data), size(size), hasContent(true),
              contentType(contentType), contentMd5(contentMd5)
        {
        }

        Content(const Json::Value & content,
                const std::string & contentType = "application/json")
            : str(content.toString()), data(str.c_str()),
              size(str.size()), hasContent(true),
              contentType(contentType)
        {
        }

        static std::string urlEncode(const std::string & str)
        {
            std::string result;
            for (auto c: str) {
                
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                    result += c;
                else result += ML::format("%%%02X", c);
            }
            return result;
        }
        
        static std::string urlEncode(const Utf8String & str)
        {
            // Encode each character separately
            std::string result;
            for (auto c: str.rawString()) {
                
                if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                    result += c;
                else result += ML::format("%%%02X", c);
            }
            return result;
        }
        
        Content(const RestParams & form)
        {
            for (auto p: form) {
                if (!str.empty())
                    str += "&";
                str += urlEncode(p.first) + "=" + urlEncode(p.second);
            }

            data = str.c_str();
            size = str.size();
            hasContent = true;
            contentType = "application/x-www-form-urlencoded";
        }

        std::string str;

        const char * data;
        uint64_t size;
        bool hasContent;

        std::string contentType;
        std::string contentMd5;
    };

    /// Callback function for when data is received
    typedef std::function<bool (const std::string &)> OnData;

    /// Callback function for when a response header is received
    typedef std::function<bool (const HttpHeader &)> OnHeader;

    /** Perform a POST request from end to end. */
    Response post(const std::string & resource,
                  const Content & content = Content(),
                  const RestParams & queryParams = RestParams(),
                  const RestParams & headers = RestParams(),
                  double timeout = -1,
                  bool exceptions = true,
                  OnData onData = nullptr,
                  OnHeader onHeader = nullptr) const
    {
        return perform("POST", resource, content, queryParams, headers,
                       timeout, exceptions, onData, onHeader);
    }

    /** Perform a PUT request from end to end. */
    Response put(const std::string & resource,
                 const Content & content = Content(),
                 const RestParams & queryParams = RestParams(),
                 const RestParams & headers = RestParams(),
                 double timeout = -1,
                 bool exceptions = true,
                 OnData onData = nullptr,
                 OnHeader onHeader = nullptr) const
    {
        return perform("PUT", resource, content, queryParams, headers,
                       timeout, exceptions, onData, onHeader);
    }

    /** Perform a synchronous GET request from end to end. */
    Response get(const std::string & resource,
                 const RestParams & queryParams = RestParams(),
                 const RestParams & headers = RestParams(),
                 double timeout = -1,
                 bool exceptions = true,
                 OnData onData = nullptr,
                 OnHeader onHeader = nullptr,
                 bool followRedirect = false) const
    {
        return perform("GET", resource, Content(), queryParams, headers,
                       timeout, exceptions, onData, onHeader, followRedirect);
    }

    /** Perform a synchronous request from end to end. */
    Response perform(const std::string & verb,
                     const std::string & resource,
                     const Content & content = Content(),
                     const RestParams & queryParams = RestParams(),
                     const RestParams & headers = RestParams(),
                     double timeout = -1,
                     bool exceptions = true,
                     OnData onData = nullptr,
                     OnHeader onHeader = nullptr,
                     bool followRedirect = false) const;
    
    /** URI that will be automatically prepended to resources passed in to
        the perform() methods
    */
    std::string serviceUri;

    /** SSL checks */
    bool noSSLChecks;

    /** Are we debugging? */
    bool debug;

private:
    /** Private type that implements a connection handler. */
    struct ConnectionHandler;

public:
    /** Get a connection. */
    struct Connection {
        Connection(ConnectionHandler * conn,
                   HttpRestProxy * proxy)
            : conn(conn), proxy(proxy)
        {
        }

        ~Connection() noexcept;

        Connection(Connection && other) noexcept
            : conn(other.conn), proxy(other.proxy)
        {
            other.conn = nullptr;
        }

        Connection & operator = (Connection && other) noexcept
        {
            this->conn = other.conn;
            this->proxy = other.proxy;
            other.conn = 0;
            return *this;
        }

        ConnectionHandler & operator * ()
        {
            ExcAssert(conn);
            return *conn;
        }

        ConnectionHandler * operator -> ()
        {
            ExcAssert(conn);
            return conn;
        }

    private:
        friend class HttpRestProxy;
        ConnectionHandler * conn;
        HttpRestProxy * proxy;
    };

    Connection getConnection() const;
    void doneConnection(ConnectionHandler * conn);

private:    
    /** Lock for connection pool. */
    mutable std::mutex lock;

    /** List of inactive handles.  These can be selected from when a new
        connection needs to be made.
    */
    mutable std::vector<Connection> inactive;

    std::vector<std::string> cookies;
};

inline std::ostream &
operator << (std::ostream & stream, const HttpRestProxy::Response & response)
{
    return stream << response.header_ << "\n" << response.body_ << "\n";
}


/****************************************************************************/
/* JSON REST PROXY                                                          */
/****************************************************************************/

/** A class that performs json queries and expects json responses, by
 * serializing C++ structures to their JSON form and vice-versa. */

struct JsonAuthenticationRequest;

struct JsonRestProxy : HttpRestProxy {
    JsonRestProxy(const std::string & url);

    /* authentication token */
    std::string authToken;

    int protocolDate;

    /* number of exponential backoffs, -1 = unlimited */
    int maxRetries;

    /* maximum number of seconds to sleep before a retry, as computed before
       randomization */
    int maxBackoffTime;

    bool authenticate(const JsonAuthenticationRequest & creds);

    HttpRestProxy::Response get(const std::string & resource)
        const
    {
        return performWithBackoff("GET", resource, "");
    }

    HttpRestProxy::Response post(const std::string & resource,
                                 const std::string & body)
        const
    {
        return performWithBackoff("POST", resource, body);
    }

    HttpRestProxy::Response put(const std::string & resource,
                                const std::string & body)
        const
    {
        return performWithBackoff("PUT", resource, body);
    }

    template<typename R>
    R getTyped(const std::string & resource, int expectedCode=-1)
        const
    {
        auto resp = performWithBackoff("GET", resource, "");
        using std::to_string;
        if (expectedCode > -1) {
            if (resp.code() != expectedCode) {
                throw ML::Exception("expected code: "
                                    + to_string(expectedCode));
            }
        }

        R data;
        try {
            data = jsonDecodeStr<R>(resp.body());
        }
        catch (...) {
            std::cerr << "exception decoding payload: " + resp.body() + "\n";
            throw;
        }

        return data;
    }

    template<typename R, typename T>
    R postTyped(const std::string & resource,
                const T & payload, int expectedCode=-1)
        const
    {
        return uploadWithBackoffTyped<R>("POST", resource, payload, expectedCode);
    }

    template<typename R, typename T>
    R putTyped(const std::string & resource,
               const T & payload, int expectedCode=-1)
        const
    {
        return uploadWithBackoffTyped<R>("PUT", resource, payload, expectedCode);
    }

private:
    static void sleepAfterRetry(int retryNbr, int maxBaseTime);

    HttpRestProxy::Response performWithBackoff(const std::string & method,
                                               const std::string & resource,
                                               const std::string & body) const;

    template<typename R, typename T>
    R uploadWithBackoffTyped(const std::string & method,
                             const std::string & resource,
                             const T & payload,
                             int expectedCode = -1)
        const
    {
        std::string uploadData = jsonEncodeStr<T>(payload);

        auto resp = performWithBackoff(method, resource, uploadData);
        if (expectedCode > -1) {
            if (resp.code() != expectedCode) {
                using std::to_string;
                throw ML::Exception("expected code: "
                                    + to_string(expectedCode));
            }
        }

        R data;
        try {
            data = jsonDecodeStr<R>(resp.body());
        }
        catch (...) {
            std::cerr << "exception decoding payload: " + resp.body() + "\n";
            throw;
        }

        return data;
    }
};


/****************************************************************************/
/* JSON AUTHENTICATION REQUEST                                              */
/****************************************************************************/

/* JsonAuthenticationRequest is a holder for username (email) and password
 * data. */

struct JsonAuthenticationRequest {
    std::string email;
    std::string password;
};

DECLARE_STRUCTURE_DESCRIPTION(JsonAuthenticationRequest);


/****************************************************************************/
/* JSON AUTHENTICATION RESPONSE                                             */
/****************************************************************************/

struct JsonAuthenticationResponse {
    std::string token;
};

DECLARE_STRUCTURE_DESCRIPTION(JsonAuthenticationResponse);

} // namespace Datacratic
