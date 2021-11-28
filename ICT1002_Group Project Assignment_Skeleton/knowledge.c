/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chat1002.h"

// INTENT_PTR head = NULL;
ENTITY_PTR head = NULL;
ENTITY_PTR end = NULL;
/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n)
{

	if (chatbot_is_question(intent) == 0)
	{
		return KB_INVALID;
	}
	ENTITY_PTR current = head;
	while (current != NULL)
	{
		if (compare_token(current->intent, intent) == 0)
		{
			if (compare_token(current->entity, entity) == 0)
			{
				snprintf(response, n, "%s", current->response); // Response var will be set to the entity found
				return KB_OK;
			}
		}
		current = current->next;
	}
	return KB_NOTFOUND;
}

/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
char knowledge_put(char *intent, char *entity, char *response)
{
	ENTITY_PTR current = head;
	ENTITY_PTR insert = (ENTITY_PTR)malloc(sizeof(ENTITY));
	if (insert != NULL) {
		strcpy(insert->intent, intent);
		strcpy(insert->entity, entity);
		strcpy(insert->response, response);
	} else {
		return KB_NOMEM;
	}

	if (head == NULL) {
		head = insert;
	} else {
		current = head;
	}

	while (current != NULL)
	{
		if (compare_token(current->intent, intent) == 0)
		{
			if (compare_token(current->entity, entity) == 0)
			{
				strcpy(current->response, response);
				break;
			}
			else
			{
				insert->next = current->next;
				current->next = insert;
				break;
			}
		}
		else
		{
			if (current->next == NULL)
			{
				current->next = insert;
				break;
			}
			else
			{
				current = current->next;
			}
		}
	}
	return KB_OK;
}

/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f)
{

	/* to be implemented */
	char readline[164];
	char *entity;
	char *reply;
	int readIntent;
	char intent;

	while (fgets(readline, MAX_INPUT, f))
	{
		//To remove additional newLine on the specific line that is being read.
		readline[strcspn(readline, "\n")] = 0;
		int lineLength = strlen(readline);
		for (int i = 0; i < lineLength; i++)
		{
			if (!isalpha(readline[i]) && !strstr(readline, "="))
			{
				char *linePtr = strtok(readline, "[");
				char *endLinePtr = strtok(linePtr, "]");

				if (compare_token(endLinePtr, "who") == 0)
				{
					readIntent = 0;
				}
				else if (compare_token(endLinePtr, "what") == 0)
				{
					readIntent = 1;
				}
				else if (compare_token(endLinePtr, "where") == 0)
				{
					readIntent = 2;
				}
				else if (compare_token(endLinePtr, "when") == 0)
				{
					readIntent = 3;
				}
				else if (compare_token(endLinePtr, "why") == 0)
				{
					readIntent = 4;
				}
				else if (compare_token(endLinePtr, "how") == 0)
				{
					readIntent = 5;
				}
			}
			else if (strrchr(readline, '='))
			{
				char *entity = strtok(readline, "=");
				char *reply = strtok(NULL, "=");

				switch (readIntent)
				{
				case 0:
					knowledge_put("who", entity, reply);
					break;
				case 1:
					knowledge_put("what", entity, reply);
					break;
				case 2:
					knowledge_put("where", entity, reply);
					break;
				case 3:
					knowledge_put("when", entity, reply);
					break;
				case 4:
					knowledge_put("why", entity, reply);
					break;
				case 5:
					knowledge_put("how", entity, reply);
					break;
				default:
					break;
				}
			}
		}
	}
	if (readIntent >= 0 && readIntent <= 10)
	{
		return 0;
	}
	else
	{
		return KB_NOTFOUND;
	}
}

/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset()
{

	/* to be implemented */
}

/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f)
{
	int occurence = 0;
	ENTITY_PTR current = head;
	ENTITY_PTR next = current;
	char *intent = (char *)malloc(MAX_INTENT);
	while(current != NULL) {
		char *line = (char *)malloc(MAX_ENTITY+1+MAX_RESPONSE+1);
		intent = current->intent;
		occurence++;
		if(occurence == 1) {
			fprintf(f, "[%s]\n", intent);
		}
		strcat(line, current->entity);
		strcat(line, "=");
		strcat(line, current->response);
		fprintf(f, "%s\n", line);
		free(line);
		next = current->next;
		if(intent != next->intent) {
			fprintf(f, "\n");
			occurence = 0;
		}
		current = current->next;
	}
	// fprintf(f, "%s %s %s", current->intent, current->entity, current->response);
	fclose(f);
	free(intent);
}
