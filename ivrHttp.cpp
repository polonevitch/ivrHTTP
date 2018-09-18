using namespace std;

#include <string>
#include <stdio.h>
#include "ivrHttp.h"

#include <iostream>
#include <sstream>

#include <AppInterfaceLogRec.hpp>
#include <CurlWrapper.hpp>

#include "base.h"
#ifndef MOCKUP
  #include <ilog.h>
#endif
#include <string.h>

void* getClientObj()
{
  IHttpClient *out = new IvrHTTPClient();
  return(reinterpret_cast<void*>(out));
}

void* getClientMoc()
{
  IHttpClient *out = new IvrHTTPTest();
  return(reinterpret_cast<void*>(out));
}

void destroyClientObj( void *clientObj )
{
  delete(reinterpret_cast<IHttpClient*>(clientObj));
}

int request(void * obj, 
            HTTPContentType contentType, 
            HTTPMethodType methodType, 
            const char * url, 
            const char * queryData, 
            char * responseData, 
            int responseLength, 
            char * errorMessage, 
            int errorLength)
{
  IHttpClient* cli = reinterpret_cast<IHttpClient*>(obj);
  int err = 405; //405 status code as Method Not Allowed
  
  if(methodType == POST)
    cli->postData(url, cli->getStrContentType(contentType).c_str(), queryData, responseData, responseLength, errorMessage, errorLength, &err);

  if(methodType == GET)
    cli->getData(url, cli->getStrContentType(contentType).c_str(), "", responseData, responseLength, errorMessage, errorLength, &err);

  return err;
}

string IHttpClient::getStrContentType(HTTPContentType t)
{
  return contTypes[t];
}

IHttpClient::IHttpClient()
{
  contTypes[JSON]    = JSON_CONTENT_TYPE;
  contTypes[XML]     = XML_CONTENT_TYPE;
  contTypes[NO_BODY] = "";
}

IHttpClient::~IHttpClient()
{}

bool IvrHTTPClient::postData(const char * urlData, const char * contentType, const char * requestData, char * responseData, int responseLength, char * errorMessage, int errorLength, int * httpStatusCode)
{
    static const char me[] = "IvrHTTPClient::postData";
    string response, hdr, body, errmsg;
    bool timedOut;
    CurlWrapper  curlw;
    string request(requestData);
    string url(urlData);
    AppInterfaceLogRec logRec(false);

    logRec.setDestURL(url.c_str());
    logRec.logRequest(request.c_str());


    // Make sure the url data is not empty
    if (url.empty()) {
        ILOG(IL_ERR, "%s: HttpClient URL is empty", me);
        return false;
    }

    //
    //	Let's set up for the POST with the content
    //

    curlw.addHttpHeaderOption(contentType);

    //
    //	Let's do the POST to the desired URL
    //

    if (!curlw.post(url, request, response, timedOut)) {
        ILOG(IL_ERR, "%s: HTTP POST failed -- URL: %s -- Body: %s",
             me, url.c_str(), request.c_str());
        errmsg.assign("HTTP POST failed for ");

        logRec.setResultType(timedOut ? AppInterfaceLogRec::RESULT_TIMEOUT
                                      : AppInterfaceLogRec::RESULT_ERROR);
        logRec.setResultDescription(errmsg.c_str());
        logRec.logResponse(response.c_str());

        *httpStatusCode = (int) curlw.getHttpResponseCode();
        return false;
    }

    //
    //	Get HTTP Status Code
    //

    *httpStatusCode = (int) curlw.getHttpResponseCode();

    //
    //	Stopwatch time -- let OPS know the time it took
    //

    logRec.setElapsedTime();

    // Get POST body

    curlw.extract(response, hdr, body);

    //
    //	If response buffer is too small then this
    //	is fatal
    //

    if ((int) body.length() > responseLength) {
        ILOG(IL_DB1, "%s: Response (%d) is too big for buffer (%d)",
             me, body.length(), responseLength);
        errmsg.assign("Response buffer is too small for response");
        return false;
    }
    else {
        strcpy(responseData, body.c_str());
    }

    //
    //	Don't consider this fatal as there maybe good information
    //	in the parsed XML
    //

    if ((int) errmsg.length() > errorLength) {
        ILOG(IL_DB1, "%s: Error message (%d) is too big for buffer (%d)",
             me, errmsg.length(), errorLength);
        errmsg.assign("Error buffer is too small for error message");
        logRec.setResultDescription(errmsg.c_str());
        logRec.logResponse(response.c_str());
        strncpy(errorMessage, errmsg.c_str(), errorLength - 1);
        errorMessage[errorLength] = '\0';
    }
    else {
        strcpy(errorMessage, errmsg.c_str());
    }

    return true;
}

bool IvrHTTPClient::getData(const char * urlData, const char * queryData, const char * contentType, char * responseData, int responseLength, char * errorMessage, int errorLength, int * httpStatusCode)
{
    static const char me[] = "IvrHTTPClient::getData";
    string response, hdr, body, errmsg;

    bool timedOut;
    CurlWrapper  curlw;
    string query(queryData);
    string url(urlData);
    AppInterfaceLogRec logRec(false);


    // Make sure the url data is not empty
    if (url.empty()) {
        ILOG(IL_ERR, "%s: HttpClient URL is empty", me);
        return false;
    }

    logRec.setDestURL(url.c_str());
    logRec.logRequest(query.c_str());

    curlw.addHttpHeaderOption(contentType);


    //
    //	Let's do the GET to the desired URL
    //

    if (!curlw.get(url, query, response, timedOut)) {
        ILOG(IL_ERR, "%s: HTTP GET failed -- URL: %s -- Query: %s",
             me, url.c_str(), query.c_str());
        errmsg.assign("HTTP GET failed for ");

        logRec.setResultType(timedOut ? AppInterfaceLogRec::RESULT_TIMEOUT
                                      : AppInterfaceLogRec::RESULT_ERROR);
        logRec.setResultDescription(errmsg.c_str());
        logRec.logResponse(response.c_str());

        *httpStatusCode = (int) curlw.getHttpResponseCode();

        return false;
    }

    //
    //	Get the HTTP Status Code
    //

    *httpStatusCode = (int) curlw.getHttpResponseCode();

    //
    //	Stopwatch time -- let OPS know the time it took
    //

    logRec.setElapsedTime();

    // Get GET body

    curlw.extract(response, hdr, body);

    //
    //	If response buffer is too small then this
    //	is fatal
    //

    if ((int) body.length() > responseLength) {
        ILOG(IL_DB1, "%s: Response (%d) is too big for buffer (%d)",
             me, body.length(), responseLength);
        errmsg.assign("Response buffer is too small for response");
        return false;
    }
    else {
        strcpy(responseData, body.c_str());
    }

    //
    //	Don't consider this fatal as there maybe good information
    //	in the parsed XML
    //

    if ((int) errmsg.length() > errorLength) {
        ILOG(IL_DB1, "%s: Error message (%d) is too big for buffer (%d)",
             me, errmsg.length(), errorLength);
        errmsg.assign("Error buffer is too small for error message");
        logRec.setResultDescription(errmsg.c_str());
        logRec.logResponse(response.c_str());
        strncpy(errorMessage, errmsg.c_str(), errorLength - 1);
        errorMessage[errorLength] = '\0';
    }
    else {
        strcpy(errorMessage, errmsg.c_str());
    }

    return true;
}

bool IvrHTTPTest::postData(const char * urlData, const char * contentType, const char * requestData, char * responseData, int responseLength, char * errorMessage, int errorLength, int * httpStatusCode)
{
  *httpStatusCode = 412; //Precondition Failed
  if(!responseData)
    return false;
  if (!httpStatusCode)
    return false;
  std::string resp = "{\"sub\":\"my_menu\",\"xsub\":\"\",\"menuurl\":\"\",\"vars\":{\"TAG2\":\"value2\",\"tag1\":\"value1\"}}\n"; // \n - as it in Jim's server
  if (resp.length() > responseLength)
    return false;
  strcpy(responseData, resp.c_str());
  *httpStatusCode = 200;
  return true;
}

bool IvrHTTPTest::getData(const char * urlData, const char * queryData, const char * contentType, char * responseData, int responseLength, char * errorMessage, int errorLength, int * httpStatusCode)
{
  *httpStatusCode = 412; //Precondition Failed
  if (!responseData)
    return false;
  if (!httpStatusCode)
    return false;
  std::string resp = "{\"sub\":\"my_menu\",\"xsub\":\"\",\"menuurl\":\"\",\"vars\":{\"TAG22\":\"value22\",\"tag11\":\"value11\"}}\n"; // \n - as it in Jim's server
  if (resp.length() > responseLength)
    return false;
  strcpy(responseData, resp.c_str());
  *httpStatusCode = 200;
  return true;
}
