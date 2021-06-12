#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "messages.h"

void parseNormalMessage(char *data, struct message *parsed)
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

void parseMessage(unsigned char *data, struct message *parsed)
{
    // Split message into unsigned content and string parts.
    
    // Step 0: Retrieve size of part 1.
    char tmp[60];
    bzero(tmp, 60);

    int size1 = 0;
    // Increment till begining of content size part.
    // Then till begining of content part.
    for (int c = 0; c < 3; c++)
    {
        while (data[size1] != ':')
            size1++;

        size1++;
    }

    // Include last character (double quote).
    size1++;

    // Step 1: Create JSON to retrieve size of content (message->compSize).
    // Terminate part1 with '}' and add fake data for content field.
    char part1[size1+4];
    for (int i = 0; i < size1; i++)
        part1[i] = data[i];

    // memcpy(data, part1, size1);
    part1[size1+3] = '\0';
    strcpy(part1+size1, "1\"}");

    // printf("Part1 %s\n", part1);

    struct json_object *parsed_size;
    parsed_size = json_tokener_parse(part1);

    struct json_object *parsed_json;
    struct json_object *p;
    struct json_object *size;
    struct json_object *compSize;
    struct json_object *time;
    struct json_object *sender;
    struct json_object *receiver;
    struct json_object *type;
    struct json_object *filename;
    
    // Step 2: Retrieve content.
    unsigned char *contentStart = data + size1;
    // printf("\nContent till NULL: %s\n\n", (char*) contentStart);
    
    // Step 3: Retrieve part 2.
    json_object_object_get_ex(parsed_size, "size", &size);
    parsed->size = json_object_get_int(size);
    json_object_put(size);
    //free(size);

    json_object_object_get_ex(parsed_size, "compSize", &compSize);
    parsed->compSize = json_object_get_int(compSize);
    json_object_put(compSize);
    //free(compSize);
    
    // Add character '{' for JSON.
    // printf("CompSize: %lu\n Size: %lu\n", parsed->compSize, parsed->size);
    char *part2 =(char*)  (data + size1 + parsed->compSize + 1);
    *part2 = '{';
    // char *part2 = (char*) p2;
    
    parsed_json = json_tokener_parse(part2);

    // json_object_object_get_ex(parsed_json, "content", &content);
    json_object_object_get_ex(parsed_json, "p", &p);
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
    
    // Save content to struct.
    parsed->content = calloc(parsed->compSize + 1, sizeof(unsigned char));
    memcpy(parsed->content, contentStart, parsed->compSize);

    len = strlen(json_object_get_string(p));
    parsed->p = (char *) malloc(sizeof(char) * len + 1);
    strcpy(parsed->p, json_object_get_string(p));
    json_object_put(p);
    //free(p);

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
    free(parsed_size);
}

unsigned char *genMessage(struct message* message, int *l)
{
    // Retrieve content.
    // {"content":"COMPRESSION",
    // char part1[] = "{\"size\":%lu,\"content\":\"%s\",";
    char *part1;
    int size1 = asprintf(&part1, "{\"size\":%lu,\
\"compSize\":%lu,\"content\":\"", message->size, message->compSize);

    char *part2;
    *l = asprintf(&part2, "\",\"type\":%d,\
\"p\":\"%s\",\
\"time\":\"%s\",\
\"sender\":\"%s\",\
\"receiver\":\"%s\",\
\"filename\":\"%s\"}",\
message->type, message->p, message->time, 
message->sender, message->receiver, message->filename);

    // Create result unsigned string (NULL terminated).
    unsigned char *res = calloc((size1 + message->compSize + *l + 1),
            sizeof(unsigned char));

    // Copy part1.
    unsigned char *tmp = memcpy(res, part1, size1);

    // Copy content.
    tmp = memcpy(tmp+size1, message->content, message->compSize);

    // Copy part2.
    tmp = memcpy(tmp + message->compSize, part2, *l);
    
    // Update *l to final size.
    *l += size1 + message->compSize;

    /*
    printf("\nTEST\n");
    for (int i = 0; i < *l; i++)
        printf("%c", res[i]);
    */

    // Free memory.
    free(part1);
    free(part2);

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

