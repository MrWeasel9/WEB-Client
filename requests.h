#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
                          char *cookies, char *token);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char *content_type, char *body_data,
                           char *cookies, char *token);
char *compute_delete_request(char *host, char *url, char *query_params,
                             char *cookies, char *token);

#endif
