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
int knowledge_put(char *intent, char *entity, char *response)
{
	if (chatbot_is_question(intent) == KB_INVALID) {
		return KB_INVALID;
	}
	int exist = 0;
	ENTITY_PTR find = head;
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

	while (find != NULL) {
		if (compare_token(find->intent, intent) == 0 && compare_token(find->entity, entity) == 0) {
			strcpy(current->response, response);
			exist = 1;
			break;
		}
		find = find->next;
	}
	if (exist == 0) {
		while (current != NULL) {
			if (current->next != NULL) {
				if (compare_token(current->intent, intent) == 0 && compare_token(current->next->intent, intent) != 0) {
					insert->next = current->next;
					current->next = insert;
					break;
				} 
			} else {
				current->next = insert;
				break;
			}
			current = current->next;
		}
	}

	// if (exist != 1) {
	// 	while (current != NULL)
	// 	{
	// 		if (compare_token(current->intent, intent) == 0)
	// 		{
	// 			// if (compare_token(current->entity, entity) == 0)					// Check if entity and intent pair exists
	// 			// {
	// 			// 	if (compare_token(current->response, response) == 0) {
	// 			// 		free(insert);
	// 			// 		break;
	// 			// 	} else {
	// 			// 		strcpy(current->response, response);
	// 			// 		free(insert);
	// 			// 		break;
	// 			// 	}
	// 			// }
	// 			// else
	// 			// {
	// 				if (compare_token(current->intent, current->next->intent) != 0)
	// 				insert->next = current->next;
	// 				current->next = insert;
	// 				break;
	// 			// }
	// 		}
	// 		// else
	// 		// {
	// 			if (current->next == NULL)
	// 			{
	// 				current->next = insert;
	// 				break;
	// 			}
	// 			else
	// 			{
	// 				current = current->next;
	// 			}
	// 		// }
	// 	}
	// }
	// while (current != NULL)
	// {
	// 	if (compare_token(current->intent, intent) == 0)
	// 	{
	// 		if (compare_token(current->entity, entity) == 0)					// Check if entity and intent pair exists
	// 		{
	// 			if (compare_token(current->response, response) == 0) {
	// 				free(insert);
	// 				break;
	// 			} else {
	// 				strcpy(current->response, response);
	// 				free(insert);
	// 				break;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			insert->next = current->next;
	// 			current->next = insert;
	// 			break;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (current->next == NULL)
	// 		{
	// 			current->next = insert;
	// 			break;
	// 		}
	// 		else
	// 		{
	// 			current = current->next;
	// 		}
	// 	}
	// }
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

	char readline[164];
	char *entity;
	char *reply;
	int readIntent;
	char intent;

	while (fgets(readline, MAX_INPUT, f))
	{
		/*To remove additional \n on the specific line that is being read.*/
		readline[strcspn(readline, "\n")] = 0;
		int lineLength = strlen(readline);
		for (int i = 0; i < lineLength; i++)
		{
			/*If the Read Line is an intent, it will contain a non-alphabet at the first character.
			It will also check if the line has no "="*/
			if (!isalpha(readline[i]) && !strstr(readline, "="))
			{
				/*Removes "[" and "]" to get the intent that is being read*/
				char *linePtr = strtok(readline, "[");
				char *endLinePtr = strtok(linePtr, "]");

				if (compare_token(endLinePtr, WHO) == 0)
				{
					readIntent = 0;
				}
				else if (compare_token(endLinePtr, WHAT) == 0)
				{
					readIntent = 1;
				}
				else if (compare_token(endLinePtr, WHERE) == 0)
				{
					readIntent = 2;
				}
				else if (compare_token(endLinePtr, WHEN) == 0)
				{
					readIntent = 3;
				}
				else if (compare_token(endLinePtr, WHY) == 0)
				{
					readIntent = 4;
				}
				else if (compare_token(endLinePtr, HOW) == 0)
				{
					readIntent = 5;
				}
			}
			/*Once intent is being read, next in line will be entity and reply.
			This will search for "=". If it contains "=", then it is entity and reply*/
			else if (strrchr(readline, '='))
			{
				/*A pointer to point at the entity and the reply*/
				char *entity = strtok(readline, "=");
				char *reply = strtok(NULL, "=");

				/* Switch function is used to put each intent, followed by its entity and reply */
				switch (readIntent)
				{
				case 0:
					knowledge_put(WHO, entity, reply);
					break;
				case 1:
					knowledge_put(WHAT, entity, reply);
					break;
				case 2:
					knowledge_put(WHERE, entity, reply);
					break;
				case 3:
					knowledge_put(WHEN, entity, reply);
					break;
				case 4:
					knowledge_put(WHY, entity, reply);
					break;
				case 5:
					knowledge_put(HOW, entity, reply);
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
void knowledge_reset() {
  ENTITY_PTR tmp = NULL;

  while (head != NULL) {
  tmp = head->next;
  memset(head, 0, sizeof(ENTITY));
  free(head);
  head = tmp;
  }
  head = NULL;
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
	while (current != NULL) {
		if (occurence == 0) {
			fprintf(f, "[%s]\n", current->intent);
		}
		fprintf(f, "%s%s%s\n", current->entity, "=", current->response);
		if (current->next != NULL) {
			if (compare_token(current->intent, current->next->intent) != 0) {
				fprintf(f, "\n");
				occurence = 0;
			} else {
				occurence++;
			}
		}
		current = current->next;
	}
	fclose(f);
}
