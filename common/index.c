/*
 * index.c - CS50 TSE Lab 5
 *
 * 
 * Mithun Rameshkumar May 2025
 * See index.h for usage details
 */

#include "index.h" 
#include "hashtable.h"
#include "counters.h"
#include "mem.h"
#include "bag.h"
#include "file.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
 
// Constants
static const int numSlots = 500;


// Global types
typedef struct index {
   hashtable_t* hashtable;
} index_t;
 
//functions
 
/* index_new()
 *  
 * See index.h for more info
 */ 
index_t* index_new()
{
   //initialize index with 500 slots
   index_t* index = mem_malloc_assert(sizeof(index_t), "No memory for index");
   index->hashtable = hashtable_new(numSlots);

   //Null check
   if (index->hashtable == NULL) {
      fprintf(stderr, "No memory for hashtable\n");
      mem_free(index);
      return NULL;
   }

   return index;
}

/* index_insert()
 *
 * See index.h for more info
 */
bool index_insert(index_t* index, const char* word, const int docID)
{
   //Checking for Null values
   if (index == NULL || word == NULL) {
      fprintf(stderr, "Invalid parameter for index_insert\n");
      return false;
   }

   if (docID < 0) {
      fprintf(stderr, "Invalid docID for index_insert\n");
      return false;
   }

   // Checking if word already in hashtable
   counters_t* existingCounter = hashtable_find(index->hashtable, word);
   
   if (existingCounter != NULL) {
      if (counters_add(existingCounter, docID) == 0) {
         fprintf(stderr, "Counter error in index_insert\n");
         return false;
      }

      return true;
   }

   //else create new counter to insert
   counters_t* newCounter = counters_new();
   if (newCounter == NULL || counters_add(newCounter, docID) == 0) {
      fprintf(stderr, "Counter error in index_insert\n");
      counters_delete(newCounter);
      return false;
   }

   //insert into hashtable 
   if (!hashtable_insert(index->hashtable, word, newCounter)){
      fprintf(stderr, "Unable to insert into hashtable in index_insert\n");
      counters_delete(newCounter);
      return false;
   }

   return true;
}

/* index_find()
 *
 * See index.h for more info
 */
counters_t* index_find(index_t* index, const char* word)
{

   //Null check
   if (index == NULL || word == NULL) {
      fprintf(stderr, "NULL parameter in index_find\n");
      return NULL;
   }

   // Search for counter to return
   return hashtable_find(index->hashtable, word);
}

/* index_print()
 *
 * See index.h for more info
 */
void index_print(index_t* index, FILE* fp)
{
   if (index == NULL || fp == NULL) {
      fprintf(stderr, "index_print() has NULL parameters\n");
      return;
   }

   hashtable_iterate(index->hashtable, fp, index_printPairs); //iterate over hashtable
}

/* index_save()
 *
 * See index.h for more info
 */
index_t* index_save(FILE* fp)
{
   //Null check
   if (fp == NULL) {
      return NULL;
   }

   //Ensure new index is created
   index_t* returnIndex = index_new();
   if (returnIndex == NULL) { 
      return NULL;
   }

   char *docID, *count, *word, *line; //Initialize variables

   while ((line = file_readLine(fp) != NULL)) { //Read lines until done
      word = strtok(line, " "); //split line by spaces
      
      if (word == NULL) { //If no words, continue;
         mem_free(line);
         continue;
      }

      //Initialize newCounter
      counters_t* newCounter = counters_new();
      if (newCounter == NULL) {
         index_delete(returnIndex);
         mem_free(line);
         return NULL;
      }

      //Ensure counter can be inserted
      if (!hashtable_insert(returnIndex->hashtable, word, newCounter)) {
         counters_delete(newCounter);
         mem_free(line);
         index_delete(returnIndex);
         return NULL;
      }

      while ((docID = strtok(NULL, " ")) != NULL) { //Loop until none left in line
         count = strtok(NULL, " "); //get count for docID
         if (count == NULL) { //Error check
            index_delete(returnIndex);
            mem_free(line);
            return NULL;
         }

         if (!counters_set(newCounter, atoi(docID), atoi(count))) { //Check if counter can be set
            index_delete(returnIndex);
            mem_free(line);
            return NULL;
         }
      }
      mem_free(line);
   }
   return returnIndex;
}

/* index_delete()
 *
 * see index.h for more info
 */
void index_delete(index_t* index)
{
   //Null check
   if (index == NULL) {
      return;
   }

   hashtable_iterate(index->hashtable, NULL, index_countersDelete); //delete counter items
   hashtable_delete(index->hashtable, NULL); //delete hashtable
   mem_free(index); //delete index
}

/* index_countersDelete
 *
 * Helper for index_delete
 * Deletes each counter
 */
static void index_countersDelete(void* arg, const char* key, void* item)
{
   //avoiding parameter not used notification
   (void) arg;
   (void) key;

   counters_delete((counters_t*) item); //delete counters
}


/* index_printPairs()
 *
 * helper for index_print()
 * Prints word and docID/counterPairs
 */
static void index_printPairs(void* arg, const char* key, void* item)
{
   //Null checks
   if (arg == NULL || key == NULL || item == NULL){
      fprintf(stderr, "index_printHelper has NULL parameters\n");
      return;
   }

   FILE* fp = (FILE*) arg;
   fprintf(fp, "%s", key); //print word
   counters_iterate(item,fp, index_printCounters); //iterate over counters
   fprintf(fp, "\n");
}

/* index_printCounters()
 *
 * Helper for index_printPairs()
 * prints eachdocID and count
 */
static void index_printCounters(void* arg, const int key, int item) 
{  
   //Null checks
   if (arg == NULL) {
      fprintf(stderr, "printCounters has NULL argument");
      return;
   }

   FILE* fp = (FILE*) arg;
   fprintf(fp, " %d %d", key, item); //print
}

