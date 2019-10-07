#include <stdio.h>
#include "postgres_ext.h"

typedef struct CSV_DocumentData CSV_Document;

CSV_Document *CSV_parse_file(FILE *file);

int CSV_number_of_rows(CSV_Document *document);
char *CSV_get_field_as_string(CSV_Document* document, int row_number, int field_number);
int CSV_get_field_as_int(CSV_Document* document, int row_number, int field_number);
Oid CSV_get_field_as_oid(CSV_Document *document, int row_index, int field_index);

void CSV_clear_document(CSV_Document *document);
