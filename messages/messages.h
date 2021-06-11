enum MESSAGE_TYPE
{
    TEXT,
    INIT, // to "register"
    ADD, // to add user
    IMAGE,
    DOCUMENT,
    IDENTIFICATION
};


// all pointers to make it easier to transform a JSON to a struct
struct message
{
    enum MESSAGE_TYPE type;
    unsigned char *content;
    char *p;
    size_t size;
    size_t compressedSize; // No compression case: size 0.

    char *time;
    char *sender;
    char *receiver;
    char *filename; // For image and document type
};

void parseMessage(unsigned char *data, struct message *parsed);
void printStruct(struct message *parsed);
void freeMessage(struct message *message);
unsigned char *genMessage(struct message* message, int* l);
