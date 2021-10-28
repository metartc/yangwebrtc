
#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <limits.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#define HTTP_AUTOMATIC 0x8
#ifdef __cplusplus
extern "C" {
#endif

// String type used to read the request details. The char pointer is NOT null
// terminated.
struct http_string_s {
  char const * buf;
  int len;
};


typedef struct {
  int index;
  int len;
  int type;
} http_token_t;

typedef struct {
  http_token_t* buf;
  int capacity;
  int size;
} http_token_dyn_t;


typedef void (*epoll_cb_t)(struct epoll_event*);


typedef struct http_ev_cb_s {

  epoll_cb_t handler;

} ev_cb_t;

typedef struct {
  char* buf;
  int64_t total_bytes;
  int32_t capacity;
  int32_t length;
  int32_t index;
  int32_t anchor;
  http_token_t token;
  uint8_t flags;
} hs_stream_t;

typedef struct {
  int64_t content_length;
  int64_t body_consumed;
  int16_t match_index;
  int16_t header_count;
  int8_t state;
  int8_t meta;
} http_parser_t;

typedef struct http_request_s {

  epoll_cb_t handler;
  epoll_cb_t timer_handler;
  int timerfd;

  void (*chunk_cb)(struct http_request_s*);
  void* data;
  hs_stream_t stream;
  http_parser_t parser;
  int state;
  int socket;
  int timeout;
  struct http_server_s* server;
  http_token_dyn_t tokens;
  char flags;
} http_request_t;

typedef struct http_server_s {

  epoll_cb_t handler;
  epoll_cb_t timer_handler;

  int64_t memused;
  int socket;
  int port;
  int loop;
  int timerfd;
  socklen_t len;
  void (*request_handler)(http_request_t*);
  struct sockaddr_in addr;
  void* data;
  char date[32];
} http_server_t;

typedef struct http_header_s {
  char const * key;
  char const * value;
  struct http_header_s* next;
} http_header_t;

typedef struct http_response_s {
  http_header_t* headers;
  char const * body;
  int content_length;
  int status;
} http_response_t;

typedef struct http_string_s http_string_t;


enum hs_token {
  HS_TOK_NONE,        HS_TOK_METHOD,     HS_TOK_TARGET,     HS_TOK_VERSION,
  HS_TOK_HEADER_KEY,  HS_TOK_HEADER_VAL, HS_TOK_CHUNK_BODY, HS_TOK_BODY,
  HS_TOK_BODY_STREAM, HS_TOK_REQ_END,    HS_TOK_EOF,        HS_TOK_ERROR
};

enum hs_state {
  ST, MT, MS, TR, TS, VN, RR, RN, HK, HS, HV, HR, HE,
  ER, HN, BD, CS, CB, CE, CR, CN, CD, C1, C2, BR, HS_STATE_LEN
};

enum hs_char_type {
  HS_SPC,   HS_NL,  HS_CR,    HS_COLN,  HS_TAB,   HS_SCOLN,
  HS_DIGIT, HS_HEX, HS_ALPHA, HS_TCHAR, HS_VCHAR, HS_ETC,   HS_CHAR_TYPE_LEN
};

enum hs_meta_state {
  M_WFK, M_ANY, M_MTE, M_MCL, M_CLV, M_MCK, M_SML, M_CHK, M_BIG, M_ZER, M_CSZ,
  M_CBD, M_LST, M_STR, M_SEN, M_BDY, M_END, M_ERR
};

enum hs_meta_type {
  HS_META_NOT_CONTENT_LEN, HS_META_NOT_TRANSFER_ENC, HS_META_END_KEY,
  HS_META_END_VALUE,       HS_META_END_HEADERS,      HS_META_LARGE_BODY,
  HS_META_TYPE_LEN
};



// Returns the event loop id that the server is running on. This will be an
// epoll fd when running on Linux or a kqueue on BSD. This can be used to
// listen for activity on sockets, etc. The only caveat is that the user data
// must be set to a struct where the first member is the function pointer to
// a callback that will handle the event. i.e:
//
// For kevent:
//
//   struct foo {
//     void (*handler)(struct kevent*);
//     ...
//   }
//
//   // Set ev.udata to a foo pointer when registering the event.
//
// For epoll:
//
//   struct foo {
//     void (*handler)(struct epoll_event*);
//     ...
//   }
//
//   // Set ev.data.ptr to a foo pointer when registering the event.
int http_server_loop(struct http_server_s* server);

// Allocates and initializes the http server. Takes a port and a function
// pointer that is called to process requests.
struct http_server_s* http_server_init(int port, void (*handler)(struct http_request_s*));

// Stores a pointer for future retrieval. This is not used by the library in
// any way and is strictly for you, the application programmer to make use
// of.
void http_server_set_userdata(struct http_server_s* server, void* data);

// Starts the event loop and the server listening. During normal operation this
// function will not return. Return value is the error code if the server fails
// to start. By default it will listen on all interface. For the second variant
// provide the IP address of the interface to listen on, or NULL for any.
int http_server_listen(struct http_server_s* server);
int http_server_listen_addr(struct http_server_s* server, const char* ipaddr);

// Use this listen call in place of the one above when you want to integrate
// an http server into an existing application that has a loop already and you
// want to use the polling functionality instead. This works well for
// applications like games that have a constant update loop. By default it will
// listen on all interface. For the second variant provide the IP address of
// the interface to listen on, or NULL for any.
int http_server_listen_poll(struct http_server_s* server);
int http_server_listen_addr_poll(struct http_server_s* server, const char* ipaddr);

// Call this function in your update loop. It will trigger the request handler
// once if there is a request ready. Returns 1 if a request was handled and 0
// if no requests were handled. It should be called in a loop until it returns
// 0.
int http_server_poll(struct http_server_s* server);

// Returns 1 if the flag is set and false otherwise. The flags that can be
// queried are listed below
int http_request_has_flag(struct http_request_s* request, int flag);

// This flag will be set when the request body is chunked or the body is too
// large to fit in memory are once. This means that the http_request_read_chunk
// function must be used to read the body piece by piece.
#define HTTP_FLG_STREAMED 0x1

// Returns the request method as it was read from the HTTP request line.
struct http_string_s http_request_method(struct http_request_s* request);

// Returns the full request target (url) as it was read from the HTTP request
// line.
struct http_string_s http_request_target(struct http_request_s* request);

// Returns the request body. If no request body was sent buf and len of the
// string will be set to 0.
struct http_string_s http_request_body(struct http_request_s* request);

// Returns the request header value for the given header key. The key is case
// insensitive.
struct http_string_s http_request_header(struct http_request_s* request, char const * key);

// Procedure used to iterate over all the request headers. iter should be
// initialized to zero before calling. Each call will set key and val to the
// key and value of the next header. Returns 0 when there are no more headers.
int http_request_iterate_headers(
  struct http_request_s* request,
  struct http_string_s* key,
  struct http_string_s* val,
  int* iter
);

// Retrieve the opaque data pointer that was set with http_request_set_userdata.
void* http_request_userdata(struct http_request_s* request);

// Retrieve the opaque data pointer that was set with http_server_set_userdata.
void* http_request_server_userdata(struct http_request_s* request);

// Stores a pointer for future retrieval. This is not used by the library in
// any way and is strictly for you, the application programmer to make use
// of.
void http_request_set_userdata(struct http_request_s* request, void* data);

#define HTTP_KEEP_ALIVE 1
#define HTTP_CLOSE 0

// By default the server will inspect the Connection header and the HTTP
// version to determine whether the connection should be kept alive or not.
// Use this function to override that behaviour to force the connection to
// keep-alive or close by passing in the HTTP_KEEP_ALIVE or HTTP_CLOSE
// directives respectively. This may provide a minor performance improvement
// in cases where you control client and server and want to always close or
// keep the connection alive.
void http_request_connection(struct http_request_s* request, int directive);

// When reading in the HTTP request the server allocates a buffer to store
// the request details such as the headers, method, body, etc. By default this
// memory will be freed when http_respond is called. This function lets you
// free that memory before the http_respond call. This can be useful if you
// have requests that take a long time to complete and you don't require the
// request data. Accessing any http_string_s's will be invalid after this call.
void http_request_free_buffer(struct http_request_s* request);

// Allocates an http response. This memory will be freed when http_respond is
// called.
struct http_response_s* http_response_init();

// Set the response status. Accepts values between 100 and 599 inclusive. Any
// other value will map to 500.
void http_response_status(struct http_response_s* response, int status);

// Set a response header. Takes two null terminated strings.
void http_response_header(struct http_response_s* response, char const * key, char const * value);

// Set the response body. The caller is responsible for freeing any memory that
// may have been allocated for the body. It is safe to free this memory AFTER
// http_respond has been called.
void http_response_body(struct http_response_s* response, char const * body, int length);

// Starts writing the response to the client. Any memory allocated for the
// response body or response headers is safe to free after this call.
void http_respond(struct http_request_s* request, struct http_response_s* response);

// Writes a chunk to the client. The notify_done callback will be called when
// the write is complete. This call consumes the response so a new response
// will need to be initialized for each chunk. The response status of the
// request will be the response status that is set when http_respond_chunk is
// called the first time. Any headers set for the first call will be sent as
// the response headers. Headers set for subsequent calls will be ignored.
void http_respond_chunk(
  struct http_request_s* request,
  struct http_response_s* response,
  void (*notify_done)(struct http_request_s*)
);

// Ends the chunked response. Any headers set before this call will be included
// as what the HTTP spec refers to as 'trailers' which are essentially more
// response headers.
void http_respond_chunk_end(struct http_request_s* request, struct http_response_s* response);

// If a request has Transfer-Encoding: chunked or the body is too big to fit in
// memory all at once you cannot read the body in the typical way. Instead you
// need to call this function to read one chunk at a time. To check if the
// request requires this type of reading you can call the http_request_has_flag
// function to check if the HTTP_FLG_STREAMED flag is set. To read a streamed body
// you pass a callback that will be called when the chunk is ready. When
// the callback is called you can use `http_request_chunk` to get the current
// chunk. When done with that chunk call this function again to request the
// next chunk. If the chunk has size 0 then the request body has been completely
// read and you can now respond.
void http_request_read_chunk(
  struct http_request_s* request,
  void (*chunk_cb)(struct http_request_s*)
);


void hs_add_server_sock_events(struct http_server_s* serv);
void hs_server_init(struct http_server_s* serv);
void hs_delete_events(struct http_request_s* request);
void hs_add_events(struct http_request_s* request);
void hs_add_write_event(struct http_request_s* request);
void hs_process_tokens(http_request_t* request);



void hs_server_listen_cb(struct epoll_event* ev);
void hs_session_io_cb(struct epoll_event* ev);
void hs_server_timer_cb(struct epoll_event* ev);
void hs_request_timer_cb(struct epoll_event* ev);
void hs_generate_date_time(char* datetime);




// Returns the current chunk of the request body. This chunk is only valid until
// the next call to `http_request_read_chunk`.
struct http_string_s http_request_chunk(struct http_request_s* request);

#define http_request_read_body http_request_read_chunk

#ifdef __cplusplus
}
#endif

// Minimal example usage.
/**
#ifdef HTTPSERVER_EXAMPLE

#define RESPONSE "Hello, World!"

void handle_request(struct http_request_s* request) {
  struct http_response_s* response = http_response_init();
  http_response_status(response, 200);
  http_response_header(response, "Content-Type", "text/plain");
  http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
  http_respond(request, response);
}

int main() {
  struct http_server_s* server = http_server_init(8080, handle_request);
  http_server_listen(server);
}

#endif
**/
#endif


