/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "chat1002.h"

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname()
{

	return "Chatbot";
}

/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username()
{

	return "User";
}

/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n)
{

	/* check for empty input */
	if (inc < 1)
	{
		snprintf(response, n, "");
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else
	{
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}
}

/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent)
{

	return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;
}

/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n)
{

	snprintf(response, n, "Goodbye!");

	return 1;
}

/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent)
{
	return compare_token(intent, "load") == 0;
}

/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n)
{
	
	FILE* f;
	
	if (inc >= 1)
	{
		f = fopen(inv[1], "r");
		int checkRead = knowledge_read(f);
		if (checkRead == 0)
		{
			fclose(f);
			snprintf(response, n, "%s has been loaded successfully.", inv[1]);
		}
		else
		{
			snprintf(response, n, "Sorry, file is not loaded. Please ensure that the file name or file exist.");
		}
	}

	return 0;
}

/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is 5W1H
 *  0, otherwise
 */
int chatbot_is_question(const char *intent)
{
	char intentqns[6][6] = {WHAT, WHERE, WHO, WHEN, WHY, HOW};
	for (int i = 0; i < 6; i++)
	{
		if (compare_token(intentqns[i], intent) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {
	int index, find_entity, success = 0;
	char *user_entity;
	user_entity = (char *)malloc(MAX_ENTITY);
	char *answer = (char *)malloc(MAX_RESPONSE);
	int try_put_knowledge;
	if (user_entity == NULL) {
		snprintf(response, n, "Insfficient memory space");									// No space means your computer is lousy, and has run out of space
		return 1;
	}
	if (inc > 1) {																												// If the sentence is more than a word
		if (!compare_token(inv[1], "is") || !compare_token(inv[1], "are")) {
			index = 2;																												// Setting the index to 2 (3rd word) to skip "is/are" which is the 2nd word. 
		} else {
			index = 1;																												// Setting the index to 1 (2nd word) if the words are neither "is" nor "are".
		}
		if (inv[index]) {
			for (int i = index; i < inc; i++) {
				strcat(user_entity, inv[i]);
				if (i != inc - 1) {																							// If current word is not the last word, add a space
					strcat(user_entity, " ");
				}
			}
		} else {
			snprintf(response, n, "No entity was found.");
		}
		find_entity = knowledge_get(inv[0], user_entity, response, n);			// Get entity
	} else {
		snprintf(response, n, "No entity was found.");
	}
	
	if (find_entity == KB_NOTFOUND) {																			// If there was no record of that entity
		char *question = (char *)malloc(MAX_RESPONSE);											
		for (int i = 0; i < index; i++) {																		// Loop through the words in the original question and add it to question var
			strcat(question, inv[i]);
				if (i != inc - 1) {																							// If current word is not the last word, add a space
					strcat(question, " ");
				}
		}
		strcat(question, user_entity);																			// Concatenate entity word to the back of the question variable from above
		prompt_user(answer, MAX_RESPONSE, "I don't know. %s?", question);		// Ask user what is the answer
		for (int i = 0; i < strlen(answer); i++) {
			if (isspace(answer[i]) != 0) {
				success = 0;
			} else {
				success = 1;
				break;
			}
		}
		if (strcmp(answer, "") == 0 || success != 1) {
			snprintf(response, n, ":-(");
			return 0;
		} else {
			try_put_knowledge = knowledge_put(inv[0], user_entity, answer);			// Puts user answer into the knowledge base linked list
		}
		if (try_put_knowledge == KB_OK) {																		// If successful, say thank you ;)
			snprintf(response, n, "Thank you.");
		} else if (try_put_knowledge == KB_NOMEM) {
			snprintf(response, n, "Insfficient memory space");								// Not enough space, your computer must be very old 
		} else if (try_put_knowledge == KB_INVALID) {
			snprintf(response, n, "Invalid intent specified");								// We do not understand your intent
		}
		free(question);
		// free(answer);
	}
	free(user_entity);
	return 0;
}

/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent)
{
	if (!compare_token("Reset", intent)) {
		return 1;
	}
	return 0;
}

/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n)
{
  knowledge_reset();
  snprintf(response, n, "All data has been reset!");
	return 0;
}

/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "save"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent)
{
	return compare_token(intent, "save") == 0;
}

/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n)
{
	FILE* f;
	char *fileName;
	if (compare_token(inv[1], "as") == 0) {
		fileName = inv[2];
	} else if (inc == 2) {
		fileName = inv[1];
	}
	f = fopen(fileName, "w");
	knowledge_write(f);
	snprintf(response, n, "Saved!");
	return 0;
}

/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent)
{

	/* to be implemented */
	return compare_token(intent, "hello") == 0 ||
		   compare_token(intent, "hey") == 0 ||
		   compare_token(intent, "hi") == 0 ||
		   compare_token(intent, "wassup") == 0 ||
		   compare_token(intent, "it's") == 0 ||
		   compare_token(intent, "greetings") == 0 ||
		   compare_token(intent, "goodbye") == 0 ||
       compare_token(intent, "bye") == 0;
}

/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n)
{

	/* to be implemented */
	if (compare_token(inv[0], "goodbye") == 0 || compare_token(inv[0], "bye") == 0)
	{
		snprintf(response, n, "Goodbye!");
		return 1;
	} else if (compare_token(inv[0], "it's") == 0 ) {
		snprintf(response, n, "Indeed it is.");
		return 0;
	}

	snprintf(response, n, "Hello! What would you like to chat about?");
	return 0;
}
