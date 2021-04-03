#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "messages.h"

void parseMessage(char *data, struct message *parsed)
{
    // Must be replaced later by a socket pointer for client/server connection.

    struct json_object *parsed_json;
    struct json_object *content;
    struct json_object *p;
    struct json_object *size;
    struct json_object *time;
    struct json_object *sender;
    struct json_object *receiver;
    struct json_object *type;
    struct json_object *filename;
    
    parsed_json = json_tokener_parse(data);

    json_object_object_get_ex(parsed_json, "content", &content);
    json_object_object_get_ex(parsed_json, "p", &p);
    json_object_object_get_ex(parsed_json, "size", &size);
    json_object_object_get_ex(parsed_json, "time", &time);
    json_object_object_get_ex(parsed_json, "sender", &sender);
    json_object_object_get_ex(parsed_json, "receiver", &receiver);
    json_object_object_get_ex(parsed_json, "type", &type);
    json_object_object_get_ex(parsed_json, "filename", &filename);

    size_t len;
    // Store objects from json.
    parsed -> type =  (enum MESSAGE_TYPE) json_object_get_int(type);
    free(type);

    len = strlen(json_object_get_string(content));
    parsed->content = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->content, json_object_get_string(content));
    free(content);

    len = strlen(json_object_get_string(p));
    parsed->p = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->p, json_object_get_string(p));
    free(p);

    parsed->size = json_object_get_int(size);
    free(size);

    len = strlen(json_object_get_string(time));
    parsed->time = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->time, json_object_get_string(time));
    free(time);

    len = strlen(json_object_get_string(sender));
    parsed->sender = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->sender, json_object_get_string(sender));
    free(sender);

    len = strlen(json_object_get_string(receiver));
    parsed->receiver = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->receiver, json_object_get_string(receiver));
    free(receiver);

    len = strlen(json_object_get_string(filename));
    parsed->filename = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->filename, json_object_get_string(filename));
    free(filename);

    free(parsed_json);
}

/*
struct message
{
    enum MESSAGE_TYPE type;
    char *content;
    char *time;
    char *sender;
    char *receiver;
    char *filename; // For image and document type
};
*/

char *genMessage(struct message* message, int *l)
{
    char *res;
    *l = asprintf(&res, "{\"type\":%d,\
\"content\":\"%s\",\
\"p\":\"%s\",\
\"size\":%lu,\
\"time\":\"%s\",\
\"sender\":\"%s\",\
\"receiver\":\"%s\",\
\"filename\":\"%s\"}",\
message->type, message->content, message->p, message->size,\
message->time, message->sender, message->receiver, message->filename);
    return res;
}

void printStruct(struct message *parsed)
{
    printf("User: %s sending message %s with second cypher %s\
            and original msg size %lu to User %s\n", 
            parsed->sender, parsed->content, parsed->p, parsed->size, parsed->receiver);

    printf("Message of type %d sent at %s with file name %s\n",
            parsed->type, parsed->time, parsed->filename);
}


// Probably useful
void freeMessage(struct message *message)
{
    free(message->content);
    free(message->p);
    free(message->time);
    free(message->sender);
    free(message->receiver);
    free(message->filename);
    free(message);
}



/*
int main()
{
    // Test JSON parser.
    char *test = "{\"sender\": \"123456789\",\
    \"receiver\": \"987654321\",\
    \"type\": 5,\
    \"content\": \"Encrypted message\",\
    \"time\": \"1845689\",\
    \"filename\": \"file.txt\"\
    }";

    struct message *parsed = malloc(sizeof(struct message));
    parseMessage(test, parsed);

    // Print structure.
    printStruct(parsed);
}
*/
