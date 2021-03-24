#include <stdio.h>
#include <json-c/json.h>

int main(int argc, char **argv)
{
    // Must be replaced later by a socket pointer for client/server connection.
    FILE *fp;
    char buffer[1024];

    struct json_object *parsed_json;
    struct json_object *sender;
    struct json_object *receiver;
    struct json_object *mess_type;
    struct json_object *message;
    struct json_object *time;
    struct json_object *file_name;

    // Open file.
    fp = fopen("test.json", "r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "sender", &sender);
    json_object_object_get_ex(parsed_json, "receiver", &receiver);
    json_object_object_get_ex(parsed_json, "mess_type", &mess_type);
    json_object_object_get_ex(parsed_json, "message", &message);
    json_object_object_get_ex(parsed_json, "time", &time);
    json_object_object_get_ex(parsed_json, "file_name", &file_name);

    // Store objects from json.
    size_t sender_obj = json_object_get_int(sender);
    size_t receiver_obj = json_object_get_int(receiver);
    const char *mess_type_obj = json_object_get_string(mess_type);
    const char *message_obj = json_object_get_string(message);
    size_t time_obj = json_object_get_int(time);
    const char *file_name_obj = json_object_get_string(file_name);

    // Case if message type is a file.
    if (mess_type_obj == "FILE")
    {
        printf("File name is %s\n", file_name_obj);
    }

    // Print content.
    printf("Sender %li to Receiver %li\n", sender_obj, receiver_obj);
    printf("Sender sent %s %s\n", mess_type_obj, message_obj);

    return 0;
}
