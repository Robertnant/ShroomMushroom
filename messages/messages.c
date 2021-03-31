#include <stdio.h>
#include <stdlib.h>
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
    parsed -> type = (enum MESSAGE_TYPE*) json_object_get_int(type);
    parsed -> content = json_object_get_string(content);
    parsed -> time = json_object_get_string(time);
    parsed -> sender = json_object_get_string(sender);
    parsed -> receiver = json_object_get_string(receiver);
    parsed -> filename = json_object_get_string(filename);

}

void printStruct(struct message *parsed)
{
    printf("User: %s sending message %s to User %s\n", 
            parsed->sender, parsed->content, parsed->receiver);

    printf("Message of type %d sent at %s with file name %s\n",
            (int) parsed->type, parsed->time, parsed->filename);
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
