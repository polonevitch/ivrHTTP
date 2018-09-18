#pragma once

#ifdef MOCKUP
  #define ILOG fprintf
  #define IL_ERR stderr
  #define IL_DB1 stderr
  #define IL_DB4 stderr
#endif

static const char XMLNS_TAG[] = "xmlns";
static const char XML_CONTENT_TYPE[] = "Content-Type: application/xml";
static const char JSON_CONTENT_TYPE[] = "Content-Type: application/json";

typedef enum {XML = 1, JSON, NO_BODY} HTTPContentType;
typedef enum {GET = 1, POST} HTTPMethodType;

#ifdef __cplusplus

#include <map>

class IHttpClient
{
public:
  IHttpClient();
  virtual ~IHttpClient();

  virtual bool postData(const char *urlData,
                        const char *contentType,
                        const char *requestData,
                        char *responseData,
                        int responseLength,
                        char *errorMessage,
                        int errorLength,
                        int *httpStatusCode) = 0;

  virtual bool getData(const char *urlData,
                        const char *queryData,
                        const char *contentType,
                        char *responseData,
                        int responseLength,
                        char *errorMessage,
                        int errorLength,
                        int *httpStatusCode) = 0;

  string getStrContentType(HTTPContentType t);

protected:
  map< HTTPContentType, string > contTypes;

};

class IvrHTTPTest : public IHttpClient
{
public:
  IvrHTTPTest() {};
  virtual ~IvrHTTPTest() {};

  bool postData(const char *urlData,
    const char *contentType,
    const char *requestData,
    char *responseData,
    int responseLength,
    char *errorMessage,
    int errorLength,
    int *httpStatusCode);

  bool getData(const char *urlData,
    const char *queryData,
    const char *contentType,
    char *responseData,
    int responseLength,
    char *errorMessage,
    int errorLength,
    int *httpStatusCode);
};

class IvrHTTPClient : public IHttpClient
{
public:
  IvrHTTPClient() {};
  virtual ~IvrHTTPClient() {};

  bool postData(const char *urlData,
    const char *contentType,
    const char *requestData,
    char *responseData,
    int responseLength,
    char *errorMessage,
    int errorLength,
    int *httpStatusCode);

  bool getData(const char *urlData,
    const char *queryData,
    const char *contentType,
    char *responseData,
    int responseLength,
    char *errorMessage,
    int errorLength,
    int *httpStatusCode);
};
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

void* getClientObj();
void* getClientMoc();
void destroyClientObj( void *clientObj );
int request( void* clientObj,
             HTTPContentType contentType,
             HTTPMethodType methodType,
             const char *url,
             const char *queryData,
             char *responseData,
             int responseLength,
             char *errorMessage,
             int errorLength);

#ifdef __cplusplus
}
#endif