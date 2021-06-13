#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <gmodule.h>
#include "messages.h"

void parseMessage(char *data, struct message *parsed)
{
    // Prevent parsing HTTP request.
    if (!(data[0] == '{'))
    {
        printf("Got incorrect message request\n");
        printf("First char: %c\n", data[0]);
        parsed->content = NULL;
        parsed->p = NULL;
        parsed->time = NULL;
        parsed->sender = NULL;
        parsed->receiver = NULL;
        parsed->filename = NULL;

        return;
    }

    // Must be replaced later by a socket pointer for client/server connection.
    printf("Attempting to parse message: %s\n", data);
    struct json_object *parsed_json;
    struct json_object *content;
    struct json_object *p;
    struct json_object *size;
    struct json_object *compSize;
    struct json_object *time;
    struct json_object *sender;
    struct json_object *receiver;
    struct json_object *type;
    struct json_object *filename;

    parsed_json = json_tokener_parse(data);

    json_object_object_get_ex(parsed_json, "content", &content);
    json_object_object_get_ex(parsed_json, "p", &p);
    json_object_object_get_ex(parsed_json, "size", &size);
    json_object_object_get_ex(parsed_json, "compSize", &compSize);
    json_object_object_get_ex(parsed_json, "time", &time);
    json_object_object_get_ex(parsed_json, "sender", &sender);
    json_object_object_get_ex(parsed_json, "receiver", &receiver);
    json_object_object_get_ex(parsed_json, "type", &type);
    json_object_object_get_ex(parsed_json, "filename", &filename);

    // Check if objects are parsed.
    if (!(content && p && size 
            && compSize && time && sender && receiver && type && filename))
    {
        printf("Objects not parsed\n");
        return;
    }

    size_t len;
    // Store objects from json.
    parsed -> type =  (enum MESSAGE_TYPE) json_object_get_int(type);
    json_object_put(type);
    //free(type);
    if (&parsed->type == NULL)
    {
        free(parsed_json);
        parsed->content = NULL;
        parsed->p = NULL;
        parsed->size = 0;
        parsed->compSize = 0;
        parsed->time = NULL;
        parsed->sender = NULL;
        parsed->receiver = NULL;
        parsed->filename = NULL;
        return;
    }

    len = strlen(json_object_get_string(content));
    // parsed->content = (char *) malloc(sizeof(char) * len + 1);
    char *tmpContent = (char *) malloc(sizeof(char) * len + 1);
    strcpy(tmpContent, json_object_get_string(content));
    parsed->content = (char *) tmpContent;
    json_object_put(content);
    //free(content);

    len = strlen(json_object_get_string(p));
    parsed->p = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->p, json_object_get_string(p));
    json_object_put(p);
    //free(p);

    parsed->size = json_object_get_int(size);
    json_object_put(size);
    //free(size);

    parsed->compSize = json_object_get_int(compSize);
    json_object_put(compSize);
    //free(compSize);
    
    len = strlen(json_object_get_string(time));
    parsed->time = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->time, json_object_get_string(time));
    json_object_put(time);
    //free(time);

    len = strlen(json_object_get_string(sender));
    parsed->sender = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->sender, json_object_get_string(sender));
    json_object_put(sender);
    //free(sender);

    len = strlen(json_object_get_string(receiver));
    parsed->receiver = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->receiver, json_object_get_string(receiver));
    json_object_put(receiver);
    //free(receiver);

    len = strlen(json_object_get_string(filename));
    parsed->filename = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->filename, json_object_get_string(filename));
    json_object_put(filename);
    //free(filename);

    // json_object_put(parsed_json);
    free(parsed_json);
}

char *genMessage(struct message* message, int *l)
{
    char *res;
    *l = asprintf(&res, "{\"size\":%lu,\
\"compSize\":%lu,\
\"content\":\"%s\",\
\"type\":%d,\
\"p\":\"%s\",\
\"time\":\"%s\",\
\"sender\":\"%s\",\
\"receiver\":\"%s\",\
\"filename\":\"%s\"}",\
message->size, message->compSize, 
(char*) message->content, message->type, message->p,
message->time, message->sender, message->receiver, message->filename);

    return res;
}

void printStruct(struct message *parsed)
{
    printf("User: %s sending message %s with second cypher %s\
            and original msg size %lu with compressed size %lu to User %s\n", 
            parsed->sender, parsed->content, parsed->p, parsed->size, 
            parsed->compSize, parsed->receiver);

    printf("Message of type %d sent at %s with file name %s\n",
            parsed->type, parsed->time, parsed->filename);
}


// Probably useful
void freeMessage(struct message *message)
{
    if (message->content)
        free(message->content);
    if (message->p)
        free(message->p);
    if (message->time)
        free(message->time);
    if (message->sender)
        free(message->sender);
    if (message->receiver)
        free(message->receiver);
    if (message->filename)
        free(message->filename);
        
    //free(message);
}

/*
int main()
{
    // Test message generation and retrieval.

    struct message *parsed = malloc(sizeof(struct message));
    parseMessage(test, parsed);

    // Print structure.
    printStruct(parsed);
}
*/

