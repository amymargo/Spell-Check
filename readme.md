Amy Margolina
Daniel Wang

This program detects and reports spelling errors in text files by comparing words to a dictionary. It recursively traverses directories, utilizing binary search for efficient error identification, and considers punctuation, capitalization, and hyphenation

Design Notes:

The program is structured to first load the entire dictionary into memory, facilitating binary search for efficient lookups. This design choice is suitable for fast searches but might increase memory usage for very large dictionaries.
Dictionary words are stored in an array of strings, allowing for case-sensitive comparisons and enabling a straightforward implementation of binary search.
The program distinguishes between case-sensitive and not case-sensitive searches to handle capitalization variations, employing two separate functions for these purposes.
The program handles hyphenated words by spellchecking each part, and if any part is misspelled it will print the whole hyphenated word and the location of the whole word, not just the part that is misspelled.
For directory traversal, the program uses a recursive approach, ensuring that all subdirectories and files with the .txt extension are checked, excluding those beginning with a dot.

Limitations:

The current implementation perfectly address the requirement for exact case matching of proper nouns in the dictionary. 
Memory usage could be a concern for extremely large dictionaries since the dictionary is loaded into memory in its entirety.
Performance optimization was not a primary focus, and for massive datasets or very large directory structures, the program might exhibit latency. 

Testing:

To test the accuracy of the spell check, we have a test file that includes words that are correctly spelled and misspelled, hyphenated, punctuated, capitalized in various ways, etc.
We also included the Hamlet monologue and added some capitalizations and hyphenations as well.
To test our error handling messages, we entered commandline arguments with correct and incorrect paths to files.
We tested our directory transversing function as well by running the code with a directory with other directories and text files.
