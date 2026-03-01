#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <curl/curl.h>
#include <yajl/yajl_tree.h>
#include <sys/stat.h>
#include <sys/types.h>

// build: gcc -lcurl -lyajl o2.c -o o2

#define BUFFER 1048576 
#define CURL_BUFFER 1024

typedef struct _data {
    char d[CURL_BUFFER];
    struct _data* next;
    int idx;
} data;

typedef enum
{
    OAUTH2_RESPONSE_CODE = 0,
    OAUTH2_RESPONSE_TOKEN,
    OAUTH2_RESPONSE_TOKEN_AND_CODE
} oauth2_response_type;

typedef enum
{
    OAUTH2_ERROR_NO_ERROR = 0,
    OAUTH2_ERROR_INVALID_REQUEST,
    OAUTH2_ERROR_INVALID_CLIENT,
    OAUTH2_ERROR_UNAUTHORIZED_CLIENT,
    OAUTH2_ERROR_REDIRECT_URI_MISMATCH,
    OAUTH2_ERROR_ACCESS_DENIED,
    OAUTH2_ERROR_UNSUPPORTED_RESPONSE_TYPE,
    OAUTH2_ERROR_INVALID_SCOPE,
    OAUTH2_ERROR_INVALID_GRANT,
    OAUTH2_ERROR_UNSUPPORTED_GRANT_TYPE,
} oauth2_error_type;

//Internal structs
typedef struct _oauth2_error {
    oauth2_error_type error;
    char*             error_description;
    char*             error_uri;
    char*             state;
} oauth2_error;

typedef struct _oauth2_config
{
    char* email;
    char* auth_server;
    char* token_server;
    char* client_id;
    char* client_secret;
    char* redirect_uri;
    char* scope;
    char* state;
} oauth2_config;

typedef struct _oauth2_context
{
    const oauth2_config *conf;
    char* code; /* confirmation code */
    char* auth_code; /* access_token */
    char* refresh_token; /* refresh_token if any */
    int expires_in;
    char* inf;
    oauth2_error last_error;
} oauth2_context;

char* curl_make_request(char* url, char* params);
oauth2_context* create_context(oauth2_config* conf);
void oauth2_set_code(oauth2_context* contex, char* code);
void oauth2_set_inf(oauth2_context* contex, char* inf);
void oauth2_set_auth_code(oauth2_context* contex, char* auth_code);

//Returns URL to redirect user to.
void oauth2_request_auth_code(oauth2_context* conf);
void oauth2_request_access_token(oauth2_context* conf);

char* oauth2_create_auth_uri(oauth2_context* conf);
char* oauth2_create_access_token_uri(oauth2_context* conf);
char* oauth2_create_refresh_token_uri(oauth2_context* conf);

void oauth2_access_refresh_token(oauth2_context* conf);
char* oauth2_request(oauth2_context* conf, char* uri, char* params);
void oauth2_cleanup(oauth2_context* conf);
static void oauth2_parse_conf(oauth2_context*);

oauth2_context* create_context(oauth2_config* conf) {
     oauth2_context* contex = malloc(sizeof(oauth2_context));

     if(contex == NULL)
         return NULL;

     contex->conf = conf;
     contex->auth_code = NULL;
     contex->last_error.error = OAUTH2_ERROR_NO_ERROR;
     contex->last_error.error_description = NULL;
     contex->last_error.error_uri = NULL;
     contex->last_error.state = NULL;
     return contex;
}

void oauth2_set_code(oauth2_context* ctx, char* code)
{
    assert(ctx != NULL);
    ctx->code = malloc(sizeof(char) * (strlen(code)+1));
    strcpy(ctx->code, code);
}

void oauth2_set_inf(oauth2_context* ctx, char* inf) {
    assert(ctx != NULL);
    ctx->inf = malloc(sizeof(char) * (strlen(inf)+1));
    strcpy(ctx->inf, inf);
}

void oauth2_set_auth_code(oauth2_context* ctx, char* auth_code)
{
    int input_strlen;

    assert(ctx != NULL);

    input_strlen = strlen(auth_code)+1;
    ctx->auth_code = malloc(sizeof(char) * input_strlen);
    strcpy(ctx->auth_code, auth_code);
}

char* oauth2_create_auth_uri(oauth2_context* ctx) {
    int scope_len = 1;
    int state_len = 1;
    char* final_str;

    char* core_fmt = "%s?response_type=code&client_id=%s&redirect_uri=%s";
    char* scope_fmt = "&scope=%s";
    char* state_fmt = "&state=%s";

    //Get the string lengths
    int core_len = snprintf(NULL, 0, (const char*)core_fmt, ctx->conf->auth_server, ctx->conf->client_id, ctx->conf->redirect_uri) + 1;
    if(ctx->conf->scope != NULL)
        scope_len = snprintf(NULL, 0, (const char*)scope_fmt, ctx->conf->scope) + 1;
    if(ctx->conf->state != NULL)
        state_len = snprintf(NULL, 0, (const char*)state_fmt, ctx->conf->state) + 1;

    //Actually build the string
    final_str = malloc(((core_len-1)+(scope_len-1)+(state_len-1)+1)*sizeof(char));

    sprintf(final_str, (const char*)core_fmt, ctx->conf->auth_server, ctx->conf->client_id, ctx->conf->redirect_uri);
    if(ctx->conf->scope != NULL)
        sprintf((char*)(final_str+(core_len-1)), (const char*)scope_fmt, ctx->conf->scope);
    if(ctx->conf->state != NULL)
        sprintf((char*)(final_str+(core_len-1)+(scope_len-1)), (const char*)state_fmt, ctx->conf->state);
    return final_str;
}

void oauth2_request_auth_code(oauth2_context* ctx)
{

    char* final_str = oauth2_create_auth_uri(ctx);
    printf("Visit this url and hit authorize: %s\n", final_str);
    printf("Now put the auth token here: ");
    free(final_str);

    char code[4048];
    scanf("%s", code);
    oauth2_set_code(ctx, code);
}

static void oauth2_parse_conf(oauth2_context* ctx) {
    char errbuf[1024];
    errbuf[0] = 0;

    /* we have the whole config file in memory.  let's parse it ... */
    yajl_val node = yajl_tree_parse((const char *) ctx->inf, errbuf, sizeof(errbuf));

    /* printf("\n\n%s\n", ctx->inf); */

    if (node == NULL) {
        fprintf(stderr, "parse_error: ");
        if (strlen(errbuf))
            fprintf(stderr, " %s", errbuf);
        else
            fprintf(stderr, "unknown error");
        fprintf(stderr, "\n");
    }

    const char * path[] = { "access_token", (const char *) 0 };
    yajl_val v = yajl_tree_get(node, path, yajl_t_string);
    assert(v != NULL);
    ctx->auth_code = strdup(YAJL_GET_STRING(v));

    const char * path_rt[] = { "refresh_token", (const char *) 0 };
    yajl_val v_rt = yajl_tree_get(node, path_rt, yajl_t_string);
    assert(v_rt != NULL);
    ctx->refresh_token = strdup(YAJL_GET_STRING(v_rt));

    const char * path_ei[] = { "expires_in", (const char *) 0 };
    yajl_val v_ei = yajl_tree_get(node, path_ei, yajl_t_number);
    assert(v_ei != NULL);
    ctx->expires_in = YAJL_GET_INTEGER(v_ei);

    yajl_tree_free(node);
}

char* oauth2_create_access_token_uri(oauth2_context* ctx) {
    int client_secret_len = 1;

    char* core_fmt = "grant_type=authorization_code&client_id=%s&code=%s&redirect_uri=%s";
    int core_len = snprintf(NULL, 0, core_fmt, ctx->conf->client_id, ctx->code, ctx->conf->redirect_uri) + 1;

    char* client_secret_fmt = "&client_secret=%s";
    if(ctx->conf->client_secret != NULL)
        client_secret_len = snprintf(NULL, 0, (const char*)client_secret_fmt, ctx->conf->client_secret) + 1;


    char* uri = malloc(((core_len - 1) + (client_secret_len - 1) + 1) * sizeof(char));
    sprintf(uri, core_fmt, ctx->conf->client_id, ctx->code, ctx->conf->redirect_uri);

    if(ctx->conf->client_secret != NULL) // append client secret
        sprintf(uri + core_len - 1, client_secret_fmt, ctx->conf->client_secret);

    return uri;
}

void oauth2_request_access_token(oauth2_context* ctx)
{
    assert(ctx->conf != NULL);
    assert(ctx->conf->token_server != NULL);
    assert(ctx->code != NULL);

    char* uri = oauth2_create_access_token_uri(ctx);
    /* printf("\n\nUsing: %s/%s\n\n", ctx->conf->token_server, uri ); */
    oauth2_set_inf(ctx, curl_make_request(ctx->conf->token_server, uri));
    free(uri);
}

char* oauth2_create_refresh_token_uri(oauth2_context* ctx) {
    int client_secret_len = 1;
    char* core_fmt = "grant_type=refresh_token&client_id=%s&refresh_token=%s";
    int core_len = snprintf(NULL, 0, core_fmt, ctx->conf->client_id, ctx->refresh_token) + 1;

    char* client_secret_fmt = "&client_secret=%s";
    if(ctx->conf->client_secret != NULL)
        client_secret_len = snprintf(NULL, 0, (const char*)client_secret_fmt, ctx->conf->client_secret) + 1;

    char* uri = calloc(((core_len - 1) + (client_secret_len - 1) + 1) * sizeof(char), sizeof(char));
    sprintf(uri, core_fmt, ctx->conf->client_id, ctx->refresh_token);

    if(ctx->conf->client_secret != NULL) // append client secret
        sprintf(uri + core_len - 1, client_secret_fmt, ctx->conf->client_secret);

    return uri;
}

void oauth2_access_refresh_token(oauth2_context* ctx)
{
    assert(ctx->conf != NULL);
    assert(ctx->conf->token_server != NULL);
    assert(ctx->refresh_token != NULL);

    char* uri = oauth2_create_refresh_token_uri(ctx);
    char* out = curl_make_request(ctx->conf->token_server, uri);
    oauth2_set_inf(ctx, out);
    free(uri);

}

char* oauth2_request(oauth2_context* ctx, char* uri, char* params)
{
    char* retVal;
    char* uri2;
    int uri_len;

    //Sanity checks
    assert(ctx != NULL);
    assert(ctx->conf->client_id != NULL);
    assert(ctx->auth_code != NULL);
    assert(uri != NULL);

    //Are we POSTing?
    if(params != NULL)
    {
        //Attach the token to the params
        uri_len = snprintf(NULL, 0, "%s&access_token=%s", params, ctx->auth_code);
        uri2 = malloc(sizeof(char)*uri_len);
        sprintf(uri2, "%s&access_token=%s", params, ctx->auth_code);

        retVal = curl_make_request(uri, uri2);
        free(uri2);
        return retVal;
    }
    else
    {
        return NULL; //I'm not doing this now.
    }
}

void oauth2_cleanup(oauth2_context* ctx)
{
    if(ctx == NULL)
        return;
    if (ctx->auth_code != NULL)
        free(ctx->auth_code);
    if (ctx->code != NULL)
        free(ctx->code);
    if (ctx->inf != NULL)
        free(ctx->inf);
    free(ctx);
}

void run(oauth2_config *conf) {
    oauth2_context* ctx = create_context(conf);

    FILE *f;

    char *h = getenv("HOME");
    int uri_len;
    uri_len = snprintf(NULL, 0, "%s/.cache/.%s", h, ctx->conf->email);
    char* pat = malloc(sizeof(char)*uri_len);
    sprintf(pat, "%s/.cache/.%s", h, ctx->conf->email);
    f = fopen(pat, "r+");

    if (f != NULL) { /* there is cache*/
        char buffer[BUFFER];
        fread(buffer, BUFFER, 1, f);

        ctx->inf = malloc(sizeof(char) * (strlen(buffer) + 1));
        sprintf(ctx->inf, buffer);

        oauth2_parse_conf(ctx);
        struct stat attr;
        stat(pat, &attr);

        int s = time(0) - attr.st_mtime;
        /* int s = 9000; */

        /* printf("expire-in: %i, current:%i \n", ctx->expires_in, s); */

        if (s > ctx->expires_in ) {

            oauth2_access_refresh_token(ctx); /* request for access_token based on refresh_token*/

            char errbuf[1024];
            errbuf[0] = 0;

            /* we have the whole config file in memory.  let's parse it ... */
            yajl_val node = yajl_tree_parse((const char *) ctx->inf, errbuf, sizeof(errbuf));
            assert(node != NULL);

            const char * path[] = { "access_token", (const char *) 0 };
            yajl_val v = yajl_tree_get(node, path, yajl_t_string);
            assert(v != NULL);
            ctx->auth_code = strdup(YAJL_GET_STRING(v));
        }
    } else {
        oauth2_request_auth_code(ctx); /* prompt for URI to get code */
        oauth2_request_access_token(ctx); /* get the code to request access_token*/
        oauth2_parse_conf(ctx);

        f = fopen(pat, "w");
        if (f == NULL)
            printf("not able to open file to write conf");
        else
            fprintf(f, ctx->inf);
    }

    printf("%s", ctx->auth_code);

    free(pat);

    if (f != NULL)
        fclose(f);

    oauth2_cleanup(ctx);
}


static void usage(const char * progname)
{
    fprintf(stderr,
            "usage:  %s <email address>\n",
            progname);
    exit(1);
}

static oauth2_config* create_config(yajl_val node) {
    oauth2_config* lconf = malloc(sizeof(oauth2_config));

    const char * path[] = { "email", (const char *) 0 };
    yajl_val v = yajl_tree_get(node, path, yajl_t_string);
    assert(v != NULL);
    lconf->email = strdup(YAJL_GET_STRING(v));

    const char * path_as[] = { "auth_server", (const char *) 0 };
    v = yajl_tree_get(node, path_as, yajl_t_string);
    assert(v != NULL);
    lconf->auth_server = strdup(YAJL_GET_STRING(v));

    const char * path_at[] = { "token_server", (const char *) 0 };
    v = yajl_tree_get(node, path_at, yajl_t_string);
    assert(v != NULL);
    lconf->token_server = strdup(YAJL_GET_STRING(v));

    const char * path_ci[] = { "client_id", (const char *) 0 };
    v = yajl_tree_get(node, path_ci, yajl_t_string);
    assert(v != NULL);
    lconf->client_id = strdup(YAJL_GET_STRING(v));

    const char * path_cs[] = { "client_secret", (const char *) 0 };
    v = yajl_tree_get(node, path_cs, yajl_t_string);
    if (v != NULL) {
        lconf->client_secret = strdup(YAJL_GET_STRING(v));
    } else {
        lconf->client_secret = NULL;
    }

    const char * path_ru[] = { "redirect_uri", (const char *) 0 };
    v = yajl_tree_get(node, path_ru, yajl_t_string);
    assert(v != NULL);
    lconf->redirect_uri = strdup(YAJL_GET_STRING(v));

    const char * path_s[] = { "scope", (const char *) 0 };
    v = yajl_tree_get(node, path_s, yajl_t_string);
    assert(v != NULL);
    lconf->scope = strdup(YAJL_GET_STRING(v));

    lconf->state = NULL;
    return lconf;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 0;
    }

    FILE *f;
    char buffer[BUFFER];
    char *h = getenv("HOME");
    int len;
    len = snprintf(NULL, 0, "%s/.cache/.o2", h);
    char* pat = malloc(sizeof(char)*len);
    sprintf(pat, "%s/.cache/.o2", h);
    f = fopen(pat, "r+");
    if (f != NULL) { /* there is cache*/
        fread(buffer, BUFFER, 1, f);
    } else {
        printf("No config file ~/.cache/.o2");
        exit(1);
    }


    // load conf
    oauth2_config *lconf;
    char errbuf[1024];
    errbuf[0] = 0;
    yajl_val node = yajl_tree_parse((const char *) buffer, errbuf, sizeof(errbuf));
    assert(node != NULL);

    if (YAJL_IS_ARRAY(node)) {
        size_t s = node->u.array.len;
        for (size_t i = 0; i < s; i++) {
            yajl_val obj = node->u.array.values[i];
            assert(obj != NULL);

            const char * path[] = { "email", (const char *) 0 };
            yajl_val v = yajl_tree_get(obj, path, yajl_t_string);
            assert(v != NULL);
            char* email = strdup(YAJL_GET_STRING(v));
            if (strcmp(argv[1], email) == 0) {
                lconf = create_config(obj);
                run(lconf);
                free(lconf);
                return 0;
            }
        }
        printf("No config for %s\n", argv[1]);
    } else {
        lconf = create_config(node);
        if (strcmp(argv[1], lconf->email) == 0) {
            run(lconf);
            free(lconf);
            return 0;
        }
    }

    return 1;
}

size_t curl_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t idx;
    size_t max;
    data* d;
    data* nd;

    d = (data*)userdata;

    idx = 0;
    max = nmemb * size;

    //Scan to the correct buffer
    while(d->next != NULL)
        d = d->next;

    //Store the data
    while(idx < max)
    {
        d->d[d->idx++] = ((char*)ptr)[idx++];

        if(d->idx == CURL_BUFFER)
        {
            nd = malloc(sizeof(data));
            nd->next = NULL;
            nd->idx = 0;
            d->next = nd;
            d = nd;
        }
    }

    return max;
}

void data_clean(data* d)
{
    data* pd;
    while(d)
    {
        pd = d->next;
        free(d);
        d = pd;
    }
}

char* curl_make_request(char* url, char* params)
{
    /* printf("%s - %s\n", url, params); */

    data* storage;
    data* curr_storage;
    CURL* handle;
    int data_len;
    char* retVal;

    assert(url != 0);
    assert(*url != 0);

    storage = malloc(sizeof(data));
    storage->idx = 0;
    storage->next = 0;

    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, storage);

    //Do we need to add the POST parameters?
    if(params != NULL) {
        curl_easy_setopt(handle, CURLOPT_POST, 1);
        curl_easy_setopt(handle, CURLOPT_COPYPOSTFIELDS, params);
    }

    if(curl_easy_perform(handle) != 0) {
        curl_easy_cleanup(handle);
        data_clean(storage);
        return NULL;
    }

    //How long is the data?
    data_len = 0;
    curr_storage = storage;
    while(curr_storage)
    {
        data_len += curr_storage->idx;
        curr_storage = curr_storage->next;
    }

    //Allocate storage
    retVal = calloc(sizeof(char)*data_len, sizeof(char));

    //Now copy in the data
    curr_storage = storage;
    data_len = 0;
    while(curr_storage)
    {
        memcpy(retVal + data_len, curr_storage->d, curr_storage->idx);
        data_len += curr_storage->idx;
        curr_storage = curr_storage->next;
    }

    //Cleanup
    curl_easy_cleanup(handle);
    data_clean(storage);

    return retVal;
}

