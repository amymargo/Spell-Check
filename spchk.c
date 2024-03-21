#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_DICTIONARY_WORDS 1000000
#define MAX_WORD_LENGTH 256
#define MAX_LINE_LENGTH 1024
#define BUFFER_SIZE 1024

char* dictionary[MAX_DICTIONARY_WORDS];
int dictionarySize = 0;
char dictword[MAX_WORD_LENGTH];
char dictword2[MAX_WORD_LENGTH];

void loadDictionary(const char* dictionaryPath);
int isWordInDictionaryCaseSensitive(const char* word);
int isWordInDictionaryNotCaseSensitive(const char* word);
void checkSpellingInFile(const char* filePath);
void traverseDirectory(const char* path);
int endsWith(const char* str, const char* suffix);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dictionary_path> <file_or_directory_path>...\n", argv[0]);
        return EXIT_FAILURE;
    }

    loadDictionary(argv[1]);

    for (int i = 2; i < argc; i++) {
        struct stat pathStat;
        if (stat(argv[i], &pathStat) == -1) {
            perror("Failed to get file status");
            continue;
        }
        if (S_ISDIR(pathStat.st_mode)) {
            traverseDirectory(argv[i]);
        }
        else {
            checkSpellingInFile(argv[i]);
        }
    }

    // Cleanup dictionary
    for (int i = 0; i < dictionarySize; i++) {
        free(dictionary[i]);
    }

    return EXIT_SUCCESS;
}

void loadDictionary(const char* dictionaryPath) {
    int fd = open(dictionaryPath, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char word[MAX_WORD_LENGTH];
    int bytes_read, word_index = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n' || word_index >= MAX_WORD_LENGTH - 1) {
                word[word_index] = '\0'; // Null-terminate the word
                dictionary[dictionarySize] = strdup(word);
                if (!dictionary[dictionarySize]) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }
                dictionarySize++;
                word_index = 0; // Reset for the next word
            }
            else {
                word[word_index++] = buffer[i];
            }
        }
    }

    if (bytes_read == -1) {
        perror("Failed to read from dictionary file");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

/*int isWordInDictionaryCaseSensitive(const char *word) {
    int low = 0;
    int high = dictionarySize - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(dictionary[mid], word);
        printf("%s ", dictionary[mid]);
        if (cmp == 0) {
            return 1; // Word found
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
        printf("%d %d %d\n", low, mid, high);
    }

    return 0; // Word not found
}*/

void removeApostrophes(char* str) {
    char* src = str;
    char* dst = str;

    while (*src) {
        if (*src != '\'') {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0'; // Null-terminate the string
}

int isWordInDictionaryCaseSensitive(const char* word) {
    int low = 0;
    int high = dictionarySize - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        int cmp = strcmp(dictionary[mid], word);

        if (strcmp != 0) {
            strcpy(dictword2, dictionary[mid]);
            removeApostrophes(dictword2);
            cmp = strcmp(dictword2, word);
        }

        if (cmp == 0) {
            return 1; // Word found
        }
        else if (cmp < 0) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }
    return 0; // Word not found
}

int isWordInDictionaryNotCaseSensitive(const char* word) {
    for (int i = 0; i < dictionarySize; ++i) {
        if (strcasecmp(dictionary[i], word) == 0) {
            strcpy(dictword, dictionary[i]);
            return 1; // Word found
        }
    }
    return 0; // Word not found
}

void processWord(const char* filePath, int lineNumber, int columnNumber, char* word, int word_index);

void checkSpellingInFile(const char* filePath) {
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open file: %s\n", filePath);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char word[MAX_WORD_LENGTH];
    int word_index = 0;
    int lineNumber = 1;
    int columnNumber = 1;

    int bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        for (int i = 0; i < bytes_read; ++i) {
            char ch = buffer[i];

            if (!isalpha(ch) && (ch != '-')) {
                // Check if we've already encountered letters forming a word
                if (word_index > 0) {
                    word[word_index] = '\0'; // Null-terminate the word

                    // Process the word (without punctuation) to check its spelling
                    processWord(filePath, lineNumber, columnNumber, word, word_index);
                    word_index = 0; // Reset for the next word
                }
            }
            else {
                // Handle word characters
                if (word_index < MAX_WORD_LENGTH - 1) {
                    word[word_index++] = ch;
                }
            }

            // Update columnNumber and lineNumber
            if (ch == '\n') {
                lineNumber++;
                columnNumber = 1;
            }
            else {
                columnNumber++;
            }
        }
    }

    close(fd);
}

void processWord(const char* filePath, int lineNumber, int columnNumber, char* word, int word_index) {
    // Handle hyphenated words
    char* hyphenPart = strchr(word, '-');
    if (hyphenPart != NULL) {
        // Save the position of the beginning of the whole word
        int wholeWordColumnNumber = columnNumber - word_index;

        // Create a buffer to store the full hyphenated word
        char fullword[MAX_WORD_LENGTH];
        strcpy(fullword, word);

        // Split hyphenated word into parts and check spelling of each part
        char* part = strtok(word, "-");
        while (part != NULL) {
            int isLowerCase = islower(part[0]);
            int isUpperCase = isupper(part[0]);

            if (isLowerCase) {
                // Check if the case of the whole word matches the dictionary word
                if (!isWordInDictionaryCaseSensitive(part)) {
                    printf("%s (%d,%d): %s\n", filePath, lineNumber, wholeWordColumnNumber, fullword);
                    break;
                }
            }
            else if (isUpperCase) {
                int hasUpper = 0;
                int hasLower = 0;
                // Check if there are any other uppercase letters in the word
                for (int i = 1; part[i] != '\0'; i++) {
                    if (isupper(part[i])) {
                        hasUpper = 1;
                    }
                    if (islower(part[i])) {
                        hasLower = 1;
                    }
                }
                if (hasUpper && !hasLower) {
                    if (!isWordInDictionaryNotCaseSensitive(part)) {
                        printf("%s (%d,%d): %s\n", filePath, lineNumber, wholeWordColumnNumber, fullword);
                        break;
                    }
                }
                else if (!hasUpper) {
                    int dictwordhasupper = 0;
                    if (isWordInDictionaryNotCaseSensitive(part)) {
                        for (int i = 1; dictword[i] != '\0'; i++) {
                            if (isupper(dictword[i])) {
                                dictwordhasupper = 1;
                                break;
                            }
                        }
                    }
                    if (!isWordInDictionaryNotCaseSensitive(part) || dictwordhasupper) {
                        printf("%s (%d,%d): %s\n", filePath, lineNumber, wholeWordColumnNumber, fullword);
                        break;
                    }
                }
                else if (hasUpper) {
                    // Check spelling with case exactly matching the dictionary word
                    if (!isWordInDictionaryCaseSensitive(part)) {
                        printf("%s (%d,%d): %s\n", filePath, lineNumber, wholeWordColumnNumber, fullword);
                        break;
                    }
                }
            }
            part = strtok(NULL, "-");
        }

    }
    else {
        // Check the case of the word
        int isLowerCase = islower(word[0]);
        int isUpperCase = isupper(word[0]);

        if (isLowerCase) {
            // Check if the case of the whole word matches the dictionary word
            if (!isWordInDictionaryCaseSensitive(word)) {
                printf("%s (%d,%d): %s\n", filePath, lineNumber, columnNumber - word_index, word);
            }
        }
        else if (isUpperCase) {
            int hasUpper = 0;
            int hasLower = 0;
            // Check if there are any other uppercase letters in the word
            for (int i = 1; word[i] != '\0'; i++) {
                if (isupper(word[i])) {
                    hasUpper = 1;
                }
                if (islower(word[i])) {
                    hasLower = 1;
                }
            }
            if (hasUpper && !hasLower) {
                if (!isWordInDictionaryNotCaseSensitive(word)) {
                    printf("%s (%d,%d): %s\n", filePath, lineNumber, columnNumber - word_index, word);
                }
            }
            else if (!hasUpper) {
                int dictwordhasupper = 0;
                if (isWordInDictionaryNotCaseSensitive(word)) {
                    for (int i = 1; dictword[i] != '\0'; i++) {
                        if (isupper(dictword[i])) {
                            dictwordhasupper = 1;
                            break;
                        }
                    }
                }
                if (!isWordInDictionaryNotCaseSensitive(word) || dictwordhasupper) {
                    printf("%s (%d,%d): %s\n", filePath, lineNumber, columnNumber - word_index, word);
                }
            }
            else if (hasUpper) {
                // Check spelling with case exactly matching the dictionary word
                if (!isWordInDictionaryCaseSensitive(word)) {
                    printf("%s (%d,%d): %s\n", filePath, lineNumber, columnNumber - word_index, word);
                }
            }
        }
    }
}

void traverseDirectory(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip the '.' and '..' entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // Skip hidden files and directories
        if (entry->d_name[0] == '.') continue;

        // Construct full path of the current entry
        char fullPath[MAX_LINE_LENGTH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        // Check if entry is a directory or file
        struct stat entryStat;
        if (stat(fullPath, &entryStat) == -1) {
            perror("Failed to get file status");
            continue;
        }

        if (S_ISDIR(entryStat.st_mode)) {
            // If entry is a directory, recurse into it
            traverseDirectory(fullPath);
        }
        else if (S_ISREG(entryStat.st_mode) && endsWith(entry->d_name, ".txt")) {
            // If entry is a regular file with a .txt extension, check its spelling
            checkSpellingInFile(fullPath);
        }
    }

    closedir(dir);
}

// Function to check if a string ends with another string
int endsWith(const char* str, const char* suffix) {
    if (!str || !suffix) return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
