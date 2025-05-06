/*
 * crawler.c
 * Mithun Rameshkumar       May 2025
 * 
 * Implements a web crawler for CS50 Tiny Search Engine (Lab 4)
 * Functions:
 *  parseArgs - validate command-line arguments
 *  crawl     - perform crawling
 *  pageScan  - scan a page to queue new links
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bag.h"
#include "hashtable.h"
#include "webpage.h"
#include "mem.h"
#include "pagedir.h"

// Constants
static const int MAX_DEPTH = 10;//max crawl depth
static const int MIN_DEPTH = 0; //minimum crawl depth
static const int NUM_ARGS = 4;  //number of necessary arguments
static const int HT_SIZE = 200; //Size of hashtable

// function prototypes
static void parseArgs(const int argc, char* argv[], 
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/* main()
 *  argc: number of command-line arguments
 *  argv: array of arguments
 * 
 *  Calls parseArgs and crawl
 *  Exits 0 on success; non-zero on error
 */
int main(const int argc, char* argv[])
{
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;

    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);

    return 0;
}

/* parseArgs()
 *  argc: number of command-line arguments
 *  argv: array of arguments
 *  seedURL: out-parameter: seedURl from command line to validate and modify
 *  pageDirectory: out-parameter: pageDirectory from command line to validate and modify
 *  maxDepth: out-parameter: maxDepth given from command line to validate
 * 
 * Given arguments from the command line, extract them into the function parameters:
 *  seedURL - normalize the URL and validate it as an internal URL
 *  pageDirectory - call pagedir_init()
 *  maxDepth - ensure it is an integer in specifed range
 * 
 * If any trouble is found, print error to stderr and exit non-zero
 */
static void parseArgs(const int argc, char* argv[], 
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
    // validate number of arguments
    if (argc != NUM_ARGS) {
        fprintf(stderr, "Usage: ./crawler seedURL pageDirectory maxDepth\n");
        exit(EXIT_FAILURE);
    }

    // validate seedURL
    *seedURL = normalizeURL(argv[1]);
    if (!isInternalURL(*seedURL)) {
        fprintf(stderr, "Invalid seedURL: %s\n", argv[1]);
        mem_free(*seedURL);
        exit(EXIT_FAILURE);
    }

    //validate pageDirectory
    if (!pagedir_init(argv[2])) {
        fprintf(stderr, "Failed to initialize pageDirectory: %s\n", argv[2]);
        mem_free(*seedURL);
        exit(EXIT_FAILURE);
    }
    *pageDirectory = argv[2];

    //validate maxDepth
    if (sscanf(argv[3], "%d", maxDepth) != 1
        || *maxDepth < MIN_DEPTH || *maxDepth > MAX_DEPTH) {
        fprintf(stderr,
            "Invalid maxDepth '%s': must be integer between %d and %d\n",
            argv[3], MIN_DEPTH, MAX_DEPTH);
        exit(EXIT_FAILURE);
    }
}

/* crawl()
 *  seedURL: normalized, internal URL to start crawling from
 *  pageDirectory: Path to directory where pages live
 *  maxDepth: maximum depth to crawl
 *  
 *  Fetches pages, saves them to disk, and queues new links until
 *  no more pages or maxDepth is reached.
 *  
 *  On error, prints to stderr and exits non-zero
 */

static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    int docID = 1;

    // Initialize the hashtable and add the seedURL
    hashtable_t* pagesSeen = hashtable_new(HT_SIZE);

    if (pagesSeen == NULL) {
        fprintf(stderr, "Failed to create hashtable\n");
        exit(EXIT_FAILURE);
    }

    hashtable_insert(pagesSeen, seedURL, "");

    // Initialize the bag and add a webpage respresenting the seedURL at depth 0
    bag_t* pagesToCrawl = bag_new();

    if (pagesToCrawl == NULL) {
        fprintf(stderr, "Failed to create bag\n");
        exit(EXIT_FAILURE);
    }

    webpage_t* page = webpage_new(seedURL, 0, NULL);

    if (page == NULL) {
        fprintf(stderr, "Failed to create webpage\n");
        exit(EXIT_FAILURE);
    }

    bag_insert(pagesToCrawl, page);

    // While bag is not empty, pull a webpage from the bag
    while ((page = bag_extract(pagesToCrawl)) != NULL) { 

        // Check if able to fetch the HTML of Webpage
        if (webpage_fetch(page)) { 

            // Save the webpage to pageDirectory
            pagedir_save(page, pageDirectory, docID);
            docID += 1;

            //If the webpage not at maxDepth, pageScan the HTML
            if (webpage_getDepth(page) < maxDepth) {

                pageScan(page, pagesToCrawl, pagesSeen);
            }
        }
    
        // Delete that webpage
        webpage_delete(page);
    }

    //delete the hashtable
    hashtable_delete(pagesSeen, NULL);

    //delete the bag
    bag_delete(pagesToCrawl, webpage_delete);
}

/* pageScan()
 *  page: webpage object whose HTML to scan
 *  pagesToCrawl: bag into which new pages will be queued
 *  pagesSeen: hashtable tracking URLs already seen
 * 
 * Scans page's HTML for embedded URLs
 * 
 * On error, prints to stderr and exits(EXIT_FAILURE).
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    // while there is another URL in the page
    int pos = 0;
    char *url;
    while ((url = webpage_getNextURL(page, &pos)) != NULL) {
        char* normalizedURL = normalizeURL(url);
        mem_free(url);

        //if that URL is internal
        if (isInternalURL(normalizedURL)) {

            // Insert the webpage into the hashtable
            if (hashtable_insert(pagesSeen, normalizedURL, "")) {

                //If that succeeded, create and insert the webpage into bag
                webpage_t* newPage = webpage_new(normalizedURL, webpage_getDepth(page) + 1, NULL);
                bag_insert(pagesToCrawl, newPage);
            } else {
                mem_free(normalizedURL);
            }
        }
    }
}