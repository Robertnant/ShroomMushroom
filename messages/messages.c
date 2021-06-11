#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "messages.h"

void parseMessage(char *data, struct message *parsed)
{
    // Step 1: Split messaged into unsigned content and strign parts.

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
    json_object_put(type);
    //free(type);
    if (&parsed->type == NULL)
    {
        free(parsed_json);
        parsed->content = NULL;
        parsed->p = NULL;
        parsed->size = 0;
        parsed->time = NULL;
        parsed->sender = NULL;
        parsed->receiver = NULL;
        parsed->filename = NULL;
        return;
    }
    

    len = strlen(json_object_get_string(content));
    parsed->content = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->content, json_object_get_string(content));
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

unsigned char *genMessage(struct message* message, int *l)
{
    
    // Retrieve content.
    // {"content":"COMPRESSION",
    // char part1[] = "{\"content\":\"%s\",";
    char part1[] = "{\"content\":\"";
    int size1 = strlen(part1);

    char *part2;
    *l = asprintf(&part2, "\",\"type\":%d,\
            \"p\":\"%s\",\
            \"size\":%lu,\
            \"time\":\"%s\",\
            \"sender\":\"%s\",\
            \"receiver\":\"%s\",\
            \"filename\":\"%s\"}",\
            message->type, message->p, message->size, message->time, 
            message->sender, message->receiver, message->filename);

    // Create result unsigned string.
    unsigned char *res = malloc((*l + size1 + message->contentSize) * 
            sizeof(unsigned char));

    // Copy part1.
    unsigned char *tmp = memcpy(res, part1, size1);

    // Copy content.
    tmp = memcpy(tmp+size1, message->content, message->contentSize);

    // Copy part2.
    tmp = memcpy(tmp + message->contentSize, part2, *l);
    
    // Update *l to final size.
    *l += size1 + message->contentSize;

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

