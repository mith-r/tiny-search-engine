# Tiny Search Engine

Mithun Rameshkumar · Dartmouth CS50 · Spring 2025

A search engine written in C across Labs 4–6:

- **Crawler:** downloads linked webpages and saves a local page collection.
- **Indexer:** builds a word-to-document index from the saved pages.
- **Querier:** evaluates search queries against the index and ranks results.

`common/` contains shared indexing, page-directory, and word utilities.
`libcs50/` contains the course support library. Each stage has its own README,
Makefile, and test scripts; the indexer and querier also include design and
implementation documents.

## Build

Run `make` from the repository root in the original CS50 Linux environment.
The repository retains the instructor-provided `libcs50-given.a`; that prebuilt
library is platform-specific. Existing tests may reference Dartmouth course
paths and datasets that are not bundled with this repository.

## History

`main` contains the completed Lab 6 submission, with larger changes organized
into separate implementation, testing, and documentation commits. Split changes
retain their original commit timestamps. The original `submit4`, `submit5`, and
`submit6` branches preserve each lab stage, including original authors and commit
IDs. `archive/original-main` and `archive/pre-reorganization-main` retain the
earlier main branches. Documentation updates are dated when made.
