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
    struct json_object *time;
    struct json_object *sender;
    struct json_object *receiver;
    struct json_object *type;
    struct json_object *filename;
    
    parsed_json = json_tokener_parse(data);

    json_object_object_get_ex(parsed_json, "content", &content);
    json_object_object_get_ex(parsed_json, "time", &time);
    json_object_object_get_ex(parsed_json, "sender", &sender);
    json_object_object_get_ex(parsed_json, "receiver", &receiver);
    json_object_object_get_ex(parsed_json, "type", &type);
    json_object_object_get_ex(parsed_json, "filename", &filename);

    // Store objects from json.
    parsed -> type =  (enum MESSAGE_TYPE) json_object_get_int(type);
    free(type);

    //parsed -> content = json_object_get_string(content);
    if (content != NULL)
    {
        printf("address of string -> %p\n", json_object_get_string(content));
        strcpy(parsed->content, json_object_get_string(content));
        free(content);
    }

    strcpy(parsed->time, json_object_get_string(time));
    free(time);

    strcpy(parsed->sender, json_object_get_string(sender));
    free(sender);

    strcpy(parsed->receiver, json_object_get_string(receiver));
    free(receiver);

    strcpy(parsed->filename, json_object_get_string(filename));
    free(filename);
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
\"time\":\"%s\",\
\"sender\":\"%s\",\
\"receiver\":\"%s\",\
\"filename\":\"%s\"}",\
message->type, message->content, message->time,\
message->sender, message->receiver, message->filename);
    return res;
}

void printStruct(struct message *parsed)
{
    printf("User: %s sending message %s to User %s\n", 
            parsed->sender, parsed->content, parsed->receiver);

    printf("Message of type %d sent at %s with file name %s\n",
            parsed->type, parsed->time, parsed->filename);
}

/*
// Probably useless
void freeMessage(struct message *message)
{
    free(message->type);
    free(message->content);
    free(message->time);
    free(message->sender);
    free(message->receiver);
    free(message->filename);
}
*/


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
