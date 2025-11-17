# Recursive Directory Scanner & Dictionary-Based Spell Checker
### A C Program for Efficient Binary-Search Spell Checking With Hyphen, Case, and Punctuation Logic

This project implements a high‑performance spell checker in C that recursively scans files and directories, parses words with detailed linguistic rules, and checks each word against a dictionary using **binary search**.   It demonstrates systems‑programming concepts such as directory traversal, low‑level text parsing, memory management, and efficient search algorithms.

---

## Overview

This spell checker:
- Loads a **sorted dictionary** into memory (exact case preserved)
- Parses ASCII text from `.txt` files using `read()`
- Recursively traverses directory structures with `opendir()` and `readdir()`
- Handles punctuation, capitalization rules, and hyphenated words
- Reports incorrect words along with **file, line number, and column position**

---

## Features

### **Efficient Dictionary Lookup**
- Dictionary is loaded into a dynamically allocated array
- Words must be in ASCII lexicographic order  
- Performant: **O(log n)** lookup using Binary Search
- Case-sensitive exact matching for lowercase words
- Apostrophes removed from dictionary words before comparison

---

### **Case Handling**

#### **1. lowercase words**
Checked using **binary search** (case-sensitive).  
Example valid: `hello` → must match dictionary entry exactly.

#### **2. Initial-capital words** (“Hello”)
Case‑insensitive match first, then validated against dictionary’s stored capitalization.

If dictionary word contains uppercase letters after index 0 → it's a proper noun → must match exactly (`MacDonald` logic).

#### **3. ALL CAPS words** (“HELLO”)
Allowed ONLY if the dictionary version is also all‑caps or initial‑caps.  
Checked through **case-insensitive scan + dictionary case inspection**.

Mixed-case words such as `HeLlO` are always invalid unless **exactly** in dictionary.

---

### **Hyphenated Word Logic**

- Split on `-`  
- Each component word is checked independently  
- If **any component** is misspelled → report the **entire hyphenated word**  
- The column number refers to the **start of the full hyphenated token**

---

### **Punctuation & Quote Handling**

The parser applies the following rules:

#### Strips punctuation **only at the boundaries**:
- Leading: `' " ( [ {`  
- Trailing: `. , ! ? : ; ) ] } "`  

This allows:
```
"hello,"   →  hello
(bracketed →  bracketed
```

#### Apostrophes are removed from dictionary words before strict comparison  
This allows matching:
```
children’s → childrens  (after apostrophe stripping)
MacDonald's → MacDonalds
```

#### Internal punctuation (like mid‑word quotes) ends the token  

---

### **Recursive Directory Traversal**

- Fully recursive search through directories
- Skips:
  - `"."`, `".."`  
  - any file or folder beginning with `"."`
- Only `.txt` files are checked during directory traversal

Implemented using:
- `opendir()`
- `readdir()`
- `lstat()`
- Recursive calls into subdirectories

---

### **Error Reporting**

The program outputs:
```
<file> (<line>,<column>): <word>
```

Example:
```
essays/bad.txt (1,1): Badd
folder/sub/bar.txt (8,19): almost-correkt
```

Column number is precisely the starting position of the word.

---

## Dictionary Format

A valid dictionary MUST:
- Contain one word per line
- Be sorted in ASCII lexicographic order
- Preserve exact capitalization intended for matching
- Contain entries for ALL allowed variants if desired  
  (e.g., `hello`, `Hello`, `HELLO` all must appear separately)

Example dictionary:
```
Apple
HELLO
MacDonald
aardvark
banana
hello
world
```

---

## Usage

### **Compile**
```
make
```

### **Run**
```
./spchk dictionary.txt file_or_directory
```

### **Examples**
```
./spchk dict.txt testfile.txt
./spchk dict.txt documents/
./spchk dict.txt essays/notes.txt
```

---

## Example Output

Given:
```
My favrite color is gren.
```

Output:
```
testfile.txt (1,4): favrite
testfile.txt (1,20): gren
```

With hyphens:
```
almost-correkt → entire token is printed
```

---

## Testing

Testing includes:
- Words with correct/incorrect capitalization
- Hyphenated words (multi-part and nested)
- Words with punctuation
- Proper noun capitalization (MacDonald rules)
- Hamlet monologue with modifications
- Recursive directory tests
- Invalid file path tests
- Mixed-case rejection tests

`testfile.txt` contains edge cases used during development

---

## Known Limitations

- Must use a sorted dictionary for binary search to be correct  
- Large dictionaries consume more memory (stored fully in RAM)  
- Case-insensitive search is O(n) because it scans dictionary linearly  

---

# Authors
Amy Margolina, Daniel Wang

This project demonstrates practical systems‑programming skills in C, including file I/O, directory traversal, efficient search structures, and detailed token parsing.
