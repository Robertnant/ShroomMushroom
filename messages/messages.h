enum MESSAGE_TYPE
{
    TEXT,
    INIT, // to "register"
    ADD, // to add user
    IMAGE,
    DOCUMENT
};


// all pointers to make it easier to transform a JSON to a struct
struct message
{
    enum MESSAGE_TYPE* type;
    char *content;
    char *time;
    char *sender;
    char *receiver;
    char *filename; // For image and document type
};