#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "XML_PARSER.h"

int main() {
    XMLDocument doc;
    if(XMLDocument_load(&doc, "/home/ziegler/Documents/XML_PArser_SpeedRun/meow.xml")){

        XMLNode node = *doc.root;
        printf("Attrs\n");
        for (int i = 0; i < node.attributes.size; i++) {
            XMLAttribute attr = node.attributes.data[i];
            printf("%s : %s\n", attr.key, attr.value);
        }


        XMLDocument_free(&doc);
    }
    return 0;
}
