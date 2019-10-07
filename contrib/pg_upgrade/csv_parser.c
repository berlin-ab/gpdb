#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "csv_parser.h"
#include "postgres_fe.h"

#define atooid(x)  ((Oid) strtoul((x), NULL, 10))
#define MAX_ROW_LENGTH 2000
#define MAX_NUMBER_OF_COLUMNS 10

typedef char **CSV_Row; /* array of strings */

struct CSV_DocumentData
{
	int  number_of_rows;
	CSV_Row *rows;
};

static void
parse_row(int current_row_index, CSV_Document *document, char row[MAX_ROW_LENGTH])
{
	char *tokens[MAX_NUMBER_OF_COLUMNS];
	int  number_of_tokens = 0;

	char *token = NULL;
	char *token_position;
	char *newline_position;

	token = strtok_r(strdup(row), ",", &token_position);

	while (token != NULL)
	{
		tokens[number_of_tokens] = strtok_r(token, "\n", &newline_position);
		number_of_tokens++;
		token = strtok_r(NULL, ",", &token_position);
	}

	char **array_of_strings = palloc0(sizeof(char *) * number_of_tokens);

	for (int i = 0; i < number_of_tokens; i++)
		array_of_strings[i] = tokens[i];

	document->rows[current_row_index] = array_of_strings;
}

int 
CSV_number_of_rows(CSV_Document *document)
{
	return document->number_of_rows;
}

static CSV_Document *
make_csv_document(int number_of_rows)
{
	CSV_Document *document = palloc0(sizeof(CSV_Document));
	document->number_of_rows = number_of_rows;
	document->rows = palloc0(sizeof(document->rows) * document->number_of_rows);
	return document;
}

static int
get_number_of_rows_in_file(FILE *file)
{
	int number_of_rows = 0;
	char contents[MAX_ROW_LENGTH];

	while ((fgets(contents, MAX_ROW_LENGTH, file)) != NULL)
		number_of_rows++;

	rewind(file);

	return number_of_rows;
}

static CSV_Document *
parse_rows(CSV_Document *document, FILE *file)
{
	char contents[MAX_ROW_LENGTH];

	int current_row_index = 0;

	while ((fgets(contents, MAX_ROW_LENGTH, file)) != NULL)
		parse_row(current_row_index++, document, contents);

	return document;
}

CSV_Document *
CSV_parse_file(FILE *file)
{
	CSV_Document *document = make_csv_document(
		get_number_of_rows_in_file(file));

	return parse_rows(document, file);
}

char *
CSV_get_field_as_string(CSV_Document *document, int row_number, int field_number)
{
	return document->rows[row_number][field_number];
}

int
CSV_get_field_as_int(CSV_Document *document, int row_index, int field_index)
{
	return atoi(document->rows[row_index][field_index]);
}

Oid
CSV_get_field_as_oid(CSV_Document *document, int row_index, int field_index)
{
	return atooid(document->rows[row_index][field_index]);
}

void
CSV_clear_document(CSV_Document *document)
{
	for (int i = 0; i < document->number_of_rows; i++)
		pfree(document->rows[i]);

	pfree(document);
}
