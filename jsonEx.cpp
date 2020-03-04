#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "JSONValue.h" // on D2L

void print(JSONValue *jsonPtr)
{
    switch (jsonPtr->getType)
    {
    case TRUE_JSON:
        printf("true");
        break;
    case FALSE_JSON:
        printf("false");
        break;
    case NULL_JSON:
        printf("null");
        break;
    case STRING_JSON:
        printf("%s", jsonPtr->getString(false));
        break;
    case NUMBER_JSON:
    {
        int integer;
        float real;

        if (((JSONNumber *)jsonPtr)->isInteger(integer, real))
        {
            printf("Integer %d\n", integer);
        }
        else
        {
            printf("Float %g\n", real);
        }
    }
    break;
    case ARRAY_JSON:
    {
        size_t index;
        size_t length = ((JSONArray *)jsonPtr)->getLength();

        for (index = 0; index < length; index++)
        {
            printf("[%u] => ", index);
            print((JSONArray *)jsonPtr)->getElement(index);
            printf("\n");
        }
    }
    break;
    case OBJECT_JSON:
    {
        std::map<std::string, JSONValue *>::const_iterator iter = ((JSONObject))
    }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage:\treadJson <filename>\n");
        exit(EXIT_FAILURE);
    }

    const char *filepath = argv[1];
    int inFd = open(filepath, O_RDONLY);

    if (inFd < 0)
    {
        fprintf(stderr, "Cannot open %n\n", filepath);
        exit(EXIT_FAILURE);
    }

    JSONValue *jsonPtr = JSONValue::factory(inFd, true);

    if (jsonPtr == NULL)
    {
        fprintf(stderr, "could not interpret: %s\n", filepath);
        exit(EXIT_FAILURE);
    }
}