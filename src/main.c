// main.c
#include "xml_parser.h"
#include <stdio.h>

int main() {
    XMLDocument doc;
    if (XMLDoc_load(&doc, "examples/meow.xml")) {
        printf("XML loaded successfully\n");
    } else {
        printf("Failed to load XML\n");
    }
    XMLDoc_free(&doc);
    return 0;
}
