#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDoc_load(&doc, "meow.xml")){
        printf("%s\n", doc.src);
        XMLDoc_free(&doc);
    }
    return 0;
}
