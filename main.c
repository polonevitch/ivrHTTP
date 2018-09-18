#include "ivrHttp.h"
#include <stdio.h>

#define POST_URL "10.252.151.53:9399/stub"
#define GET_URL "10.252.151.53:9399/stub/pin/12345678901"
#define REQUEST_DAT "{ \"vars\": { \"pin\": \"12345678901\", \"ctlnum\": \"43214321432\" } }"

int main()
{

  void* client;
#ifdef MOCKUP
  printf("-=MOCKUP=-\n");
  client = getClientMoc();
#else
  client = getClientObj();
#endif
  char resp[1024];
  char err[1024];
  int code;

  printf("POST test\n");
  code = request(client, JSON, POST, POST_URL, REQUEST_DAT, resp, sizeof(resp), err, sizeof(err));
  if(code==200)
    printf("%s\n", resp);
  else
    printf("%s RET = %d\n", resp, code);

  printf("GET test\n");
  code = request(client, NO_BODY, GET, GET_URL, "", resp, sizeof(resp), err, sizeof(err));
  if(code==200)
    printf("%s\n", resp);
  else
    printf("%s RET = %d\n", resp, code);

  destroyClientObj(client);
  return 0;
}

