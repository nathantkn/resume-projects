/**------------------------------------------
    Program 3: Dynamically Evil Hangman
    Program builds a evil word guessing game that actively keeps full record of all possible words with size of choosing and quickly analyze the word list for an optimally evil strategy to make the game as difficult as possible for the guesser
    Course: CS 211, Fall 2023, UIC
    System: Windows 11
    Author: Nathan Trinh, starter code provided by Scott Reckinger and Ellen Kidane
 ---------------------------------------------**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct Pattern_struct {
  char* pat; // string pattern dynamically allocated
  int count; // frequency of pattern in the current word list
  int changes; // number of occurences of new letter in pattern
} Pattern;

//-------------------------------------------------------------------
// Function adds new word to dynamic allocated array of C-strings, where heap-space is allocated for the new word. Function then copies chars from newWord to newly allocated heap-space; if adding new word pushes numWords above maxWords, function reallocates the words array with double the capacity
// Parameters: 
//   char*** words - array of C-strings passed-by-pointer
//   int* numWords - passed-by-pointer of current number of words in *words array
//   int* maxWords - passed-by-pointer of current capacity of *words array
//   char* newWord - C-string word to be added to words
//-------------------------------------------------------------------
void addWord(char*** words, int* numWords, int* maxWords, char* newWord) {
  // Check if adding the new word exceeds the maximum capacity
  if ((*numWords) >= (*maxWords)) {
    (*maxWords) *= 2; // Double the maximum capacity

    // Allocate memory for a new array with increased capacity
    char** newWordArr = malloc(sizeof(char*) * (*maxWords));

    // Copy existing words to the new array
    for (int i = 0; i < (*numWords); i++) {
        newWordArr[i] = (*words)[i];
    }

    free(*words); // Free the memory of the old array
    (*words) = newWordArr; // Update the array pointer to point to the new array
  }

  // Allocate memory for the new word and copy it to the array
  (*words)[*numWords] = malloc(sizeof(char) * (strlen(newWord) + 1)); 
  strcpy((*words)[*numWords], newWord);

  (*numWords)++; // Increment the count of words in the array
}

//-------------------------------------------------------------------
// Function returns count of character differences between two strings; includes extra characters in longer string if different lengths
// Parameters:
//    char* str1 & str2 - C-string word used for comparison
//-------------------------------------------------------------------
int strNumMods(char* str1, char* str2) {
  int count = 0;
  int minLength;

  // Determine the minimum length of the two strings and count extra characters
  if (strlen(str1) < strlen(str2)) {
    minLength = strlen(str1);
    count += (strlen(str2) - strlen(str1));
  }
  else if (strlen(str1) > strlen(str2)) {
    minLength = strlen(str2);
    count += (strlen(str1) - strlen(str2));
  }
  else {
    minLength = strlen(str1);
  }

  // Iterate through the characters of the strings and count different characters between two strings
  for (int i = 0; i < minLength; i++) {
    if (*str1 != *str2) {
      count++;
    }

    // Pointers to move to next character in strings
    str1++;
    str2++;
  }
  
  return count;
}

//-------------------------------------------------------------------
// Function returns index of the character where two strings first differ, & returns strlen(str1) = strlen(str2) if no differences
// Parameters:
//    char* str1 & str2 - C-string word used for comparison
//-------------------------------------------------------------------
int strDiffInd(char* str1, char* str2) {
  // Determine the maximum length of the two strings
  int maxLength = (strlen(str1) >= strlen(str2)) ? strlen(str1) : strlen(str2);

  // Iterate through the characters of the strings and find the first differing character
  for (int i = 0; i < maxLength; i++) {
    // Check if we have reached the end of both strings or if the characters differ
    if ((*str1 == '\0' && *str2 == '\0') || (*str1 != *str2)) {
      return i; // Return the index of the first differing character
    }

    // Move to the next character in both strings
    str1++;
    str2++;
  }
}

//-------------------------------------------------------------------
// Function prints status of given mode
// Parameters:
//    bool status - status of current mode
//-------------------------------------------------------------------
void printMode(bool status) {
  if (status) {
    printf("ON\n");
  }
  else {
    printf("OFF\n");
  }
}


//-------------------------------------------------------------------
// Function generates an array of patterns based on word guesses and updates pattern information, while updating capacity of pattern array if maximum size is reached
// Parameters:
//    char** wordList - array of C-strings passed-by-pointer
//    int wordSum - number of words in the list
//    char guessLetter - letter guessed by the player
//    char* finalWord - C-string word of current pattern
//    Pattern* patArr - array of patterns
//    int* patArrCurrSize - passed-by-pointer of current number of words in *patArr array
//    int* patArrCapacity - passed-by-pointer of current capacity of *patArr array
//    int wordSize - length of word given by user
//-------------------------------------------------------------------
Pattern* makePatternArray(char** wordList, int wordSum, char guessLetter, char* finalWord, Pattern* patArr, int* patArrCurrSize, int* patArrCapacity, int wordSize) {

  // Iterate through the list of words
  for (int i = 0; i < wordSum; i++) {
    bool foundPat = false;
    char comparePat[50];
    int changes = 0;

    // Copies current pattern to a separate pattern used for comparing
    strcpy(comparePat, finalWord);

    // Check each character in the current word
    for (int j = 0; j < strlen(wordList[i]); j++) {
      // If the character matches guessed letter
      if (wordList[i][j] == guessLetter) {
        comparePat[j] = guessLetter; // updates the compare pattern
        changes++;
      }
    }

    // Check if the pattern already exists in the pattern array
    for (int j = 0; j < *patArrCurrSize; j++) {
      // If the pattern exists, increment its count and update changes
      if (strcmp(patArr[j].pat, comparePat) == 0) {
        (patArr[j].count)++;
        patArr[j].changes = changes;
        foundPat = true;
        break;
      }
    }

    // If the pattern doesn't exist, add it to the pattern array
    if (!foundPat) {
      // If the pattern array is full, double its capacity
      if (*patArrCurrSize >= *patArrCapacity) {
        *patArrCapacity *= 2;
        Pattern* newPatArr = (Pattern*)malloc(sizeof(Pattern) * (*patArrCapacity));

        // Copy patterns from the old array to the new one
        for (int copyIdx = 0; copyIdx < *patArrCurrSize; copyIdx++) {
          newPatArr[copyIdx] = patArr[copyIdx];
        }

        // Free the old pattern array and update the pointer
        free(patArr);
        patArr = newPatArr;
      }

      // Allocate memory for the new pattern and copy it
      patArr[*patArrCurrSize].pat = malloc(sizeof(char) * wordSize + 1);
      strcpy(patArr[*patArrCurrSize].pat, comparePat);

      // Initialize the count and changes for the new pattern
      (patArr[*patArrCurrSize].count) = 1;
      patArr[*patArrCurrSize].changes = changes;
      (*patArrCurrSize)++; // Increment the pattern array size
    }
  }
  return patArr; // Return the updated pattern array
}

//-------------------------------------------------------------------
// Function finds and returns the index of the pattern with the highest count, using number of character changes and comparison of index difference
// Parameters:
//    Pattern* patArr - array of patterns
//    int patArrCurrSize - current number of words in *patArr array
//    char* finalWord - C-string word of current pattern
//-------------------------------------------------------------------
int findMaxPatternIndex(Pattern* patArr, int patArrCurrSize, char* finalWord) {
  int maxIdx = 0;
  int maxCount = 0;

  // Iterate through the pattern array to find the pattern with the highest count
  for (int i = 0; i < patArrCurrSize; i++) {
    if (patArr[i].count > maxCount) {
      maxCount = patArr[i].count;
      maxIdx = i;
    } 
    // In case of a tie in count, compare changes
    else if (patArr[i].count == maxCount) {
      if (patArr[i].changes < patArr[maxIdx].changes) {
        maxIdx = i;
      }
      // In case of a tie in changes, compare the patterns character by character
      else if (patArr[i].changes == patArr[maxIdx].changes) {
        char* pFinalWord = finalWord;
        char* pCurrPat = patArr[i].pat;
        char* pMaxPat = patArr[maxIdx].pat;

        // Using helper function to find if strings have different characters at index
        while (strDiffInd(pFinalWord, pCurrPat) == strDiffInd(pFinalWord, pMaxPat)) {

          // Increment pointer to move to next character in strings
          int tempIdx = strDiffInd(pFinalWord, pMaxPat);
          pFinalWord = &(pFinalWord[tempIdx + 1]);
          pCurrPat = &(patArr[i].pat[tempIdx + 1]);
          pMaxPat = &(patArr[maxIdx].pat[tempIdx + 1]);
        }

        // If while loop breaks, choose the pattern with the lower index in difference
        if (strDiffInd(pFinalWord, pCurrPat) < strDiffInd(pFinalWord, pMaxPat)) {
            maxIdx = i;
        }
      }
    }
  }

  return maxIdx; // Return the index of the pattern with the highest count
}

//-------------------------------------------------------------------
// Function updates the filtered word list based on the patterns made by the guessed letter
// Parameters:
//    char*** wordList - array of C-strings passed-by-pointer
//    int* wordSum - passed-by pointer of number of words in the list
//    char* currPat - C-string of current pattern
//    char* finalWord - C-string of final pattern
//    char guessLetter - letter guessed by user
//    int wordSize - length of word given by user
//-------------------------------------------------------------------
void updateFilteredWordList(char*** wordList, int* wordSum, char* currPat, char* finalWord, char guessLetter, int wordSize) {
    char** newWordArr = (char**)malloc(sizeof(char*) * (*wordSum)); // create new array of C-strings
    int wordArrIdx = 0;
    char tempPat[wordSize];

    // Iterate through the current word list
    for (int i = 0; i < *wordSum; i++) {
        strcpy(tempPat, currPat); // Copies current pattern to a separate pattern for constructing

      // Check each character in the word for a match with the guessed letter
      for (int j = 0; j < wordSize; j++) {
          if ((*wordList)[i][j] == guessLetter) {
              tempPat[j] = guessLetter; // Find and fill the separate pattern with the guessed letter
          }
      }

      // If the word matches the final pattern, add it to the new filtered list
      if (strcmp(tempPat, finalWord) == 0) {
          newWordArr[wordArrIdx] = (*wordList)[i];
          wordArrIdx++;
      } 
      else {
          free((*wordList)[i]); // Free memory for words that don't match
      }
    }

    free(*wordList); // Free memory for the old word list
    *wordList = newWordArr; // Update the word list pointer to the new filtered list
    *wordSum = wordArrIdx; // Update the number of words in the filtered list
}

//-------------------------------------------------------------------
// Function prints stats about the remaining possible words if chosen modes are selected
// Parameters:
//    char*** wordList - array of C-strings passed-by-pointer
//    int wordSum - number of words in the wordList array
//    bool stats - activation status of stats mode
//    bool wordMode - activation status of word mode
//-------------------------------------------------------------------
void printWordsHintModes(char*** wordList, int wordSum, bool stats, bool wordMode) {
  // Print stats of possible words remaining if stats mode is on
  if (stats) {
    printf("Number of possible words remaining: %d\n", wordSum);
  }

  // Iterate in wordList array to print possible words remaining if word mode is on
  if (wordMode) {
    printf("Possible words are now:\n");
    for (int i = 0; i < wordSum; i++) {
      printf("  %s\n", (*wordList)[i]);
    }
  }
}

int main(int argc, char* argv[]) {
  int guesses = 26;
  int wordSize = 5;
  bool statsMode = false;
  bool wordListMode = false;
  bool letterListMode = false;
  bool patListMode = false;

  printf("Welcome to the (Evil) Word Guessing Game!\n\n");

  // Parse command-line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-n") == 0) {
      if (atoi(argv[i + 1]) > 1) {
        wordSize = atoi(argv[i + 1]);
        i++;
      } 
      // Handle invalid word size
      else {
        printf("Invalid word size.\n");
        printf("Terminating program...");
        return 0;
      }
    } 
    
    else if (strcmp(argv[i], "-g") == 0) {
      if (atoi(argv[i + 1]) > 0) {
        guesses = atoi(argv[i + 1]);
        i++;
      } 
      // Handle invalid number of guesses
      else {
        printf("Invalid number of guesses.\n");
        printf("Terminating program...");
        return 0;
      }
    } 
    // Set all modes on if verbose mode was selected
    else if (strcmp(argv[i], "-v") == 0) {
      statsMode = true;
      wordListMode = true;
      letterListMode = true;
      patListMode = true;
    } 
    
    else if (strcmp(argv[i], "-s") == 0) {
      statsMode = true;
    } 
    
    else if (strcmp(argv[i], "-l") == 0) {
      letterListMode = true;
    } 
    
    else if (strcmp(argv[i], "-p") == 0) {
      patListMode = true;
    } 
    
    else if (strcmp(argv[i], "-w") == 0) {
      wordListMode = true;
    } 
    else {
      printf("Invalid command-line argument.\n");
      printf("Terminating program...");
      return 0;
    }
  }

  // Print game settings
  printf("Game Settings:\n");
  printf("  Word Size = %d\n", wordSize);
  printf("  Number of Guesses = %d\n", guesses);
  printf("  View Stats Mode = ");
  printMode(statsMode);
  printf("  View Word List Mode = ");
  printMode(wordListMode);
  printf("  View Letter List Mode = ");
  printMode(letterListMode);
  printf("  View Pattern List Mode = ");
  printMode(patListMode);

  int capacity = 4;
  char inputWord[50];
  int wordSum = 0;

  // Create dynamically heap-allocated array of C-strings
  char** wordList = (char**)malloc(capacity*sizeof(char*)); 

  // file-read the word list from dictionary.txt
  FILE* inFile = fopen("dictionary.txt", "r"); 
  
  int dictSum = 0;
  char maxLengthWord[50] = "a";

  // Populate wordList with words from "dictionary.txt"
  while (!feof(inFile)) {
    fscanf(inFile, "%s", inputWord);
    dictSum += 1;
    // Store only words of length set by wordSize
    if (strlen(inputWord) == wordSize) {
      addWord(&wordList, &wordSum, &capacity, inputWord);
    }

    // Finding the word with the longest length
    if (strlen(inputWord) > strlen(maxLengthWord)) {
      strcpy(maxLengthWord, inputWord);
    }
  }
  
  fclose(inFile); // Close file to avoid memory leak

  //-------------------------------------------------------------------
  // TODO - Task IV: display dictionary stats [-s] & word list [-w] 
  //                 if the proper command-line flags are turned ON;
  //                 see sample output for proper formatting 
  //-------------------------------------------------------------------

  // Display statistics if statsMode is enabled
  if (statsMode) {
    printf("The dictionary contains %d words total.\n", dictSum);
    printf("The longest word %s has %d chars.\n", maxLengthWord, (int)strlen(maxLengthWord));
    printf("The dictionary contains %d words of length %d.\n", wordSum, wordSize);
    printf("Max size of the dynamic words array is %d.\n", capacity);
  }

  // Display words added to word list if the wordListMode is enabled
  if (wordListMode) {
    printf("Words of length %d:\n", wordSize);
    if (wordSum == 0) {
      printf("Dictionary has no words of length %d.\n", wordSize);
      printf("Terminating program...");
      return 0;
    }
    for (int i = 0; i < wordSum; i++) {
      printf("  %s\n", wordList[i]);
    }
  }

  // Display the initial word pattern with dashes
  printf("The word pattern is: ");
  for (int i = 0; i < wordSize; i++) {
      printf("-");
  }
  printf("\n");
  
  // Initialize game variables
  char guessLetter;
  bool solved = false;
  bool letterArr[26] = {false};
  bool validGuess = true;
  int patArrCurrSize = 0;
  
  // Initialize the initial word pattern with dashes
  char finalWord[50];
  for (int i = 0; i < wordSize; i++) {
    finalWord[i] = '-';
  }
  finalWord[wordSize] = '\0';

  // Main game loop
  do {
    // Display remaining guesses if it's a valid guess
    if (validGuess) {
        printf("\nNumber of guesses remaining: %d\n", guesses);
    }
    validGuess = true;
      
    // Display previously guessed letters if letterListMode is enabled
    if (letterListMode) {
      printf("Previously guessed letters: ");
      // Iterate through boolean array and print letter in their index is set to true
      for(int i = 0; i < 26; i++){
        if (letterArr[i] == true) {
          printf("%c ", (i + 'a'));
        } 
      }
      printf("\n");
    }

    // Prompt the user for a letter guess
    printf("Guess a letter (# to end game): ");
    scanf(" %c", &guessLetter);
    
    // Handle invalid guesses and quit input
    if (!isalpha(guessLetter) || isupper(guessLetter) || letterArr[guessLetter - 'a'] == true) {
        if (guessLetter == '#') {
          printf("\nTerminating game...");
          return 0;
        }
        else if (!isalpha(guessLetter) || isupper(guessLetter)) {
          printf("\nInvalid letter...\n");
        }
        // If user has already guessed the letter
        else if (letterArr[guessLetter - 'a'] == true) {
          printf("\nLetter previously guessed...\n");
        }
        validGuess = false;
    }
    
    else {
      // Reset and initialize variables for processing the current guess
      patArrCurrSize = 0;
      int patArrCapacity = 4;

      // Set boolean array at current letter index to true
      letterArr[guessLetter - 'a'] = true;

      // Dynamically allocate memory for pattern array
      Pattern* patArr = malloc(sizeof(Pattern) * patArrCapacity);

      // Populate pattern array and update game state
      patArr = makePatternArray(wordList, wordSum, guessLetter, finalWord, patArr, &patArrCurrSize, &patArrCapacity, wordSize);

      // Display patterns if patListMode is enabled
      if (patListMode) {
        printf("All patterns for letter %c: \n", guessLetter);
        for (int i = 0; i < patArrCurrSize; i++) {
          printf("  %s count = %d changes = %d\n", patArr[i].pat, patArr[i].count, patArr[i].changes);
        }
      }

      // Copy the current pattern to currPat
      char currPat[wordSize + 1];
      strcpy(currPat, finalWord);

      // Find the index of the pattern with the most occurrences
      int maxIdx = findMaxPatternIndex(patArr, patArrCurrSize, finalWord);

      // Update the final word pattern with the selected pattern
      strcpy(finalWord, patArr[maxIdx].pat);

      // Update the filtered word list based on the guess
      updateFilteredWordList(&wordList, &wordSum, currPat, finalWord, guessLetter, wordSize);

      // If no guessed letters were found in pattern
      if (patArr[maxIdx].changes == 0) {
        printf("\nOops, there are no %c's. You used a guess.\n", guessLetter);
        guesses--;
      }
      // If any guessed letters were found in pattern
      else {
        printf("\nGood guess! The word has at least one %c.\n", guessLetter);
      }

      // Print word hints and the current word pattern
      printWordsHintModes(&wordList, wordSum ,statsMode, wordListMode);
      printf("The word pattern is: %s\n", finalWord);
    
      // Check if the word is solved
      if (strchr(finalWord, '-') == NULL) {
        solved = true;
        printf("You solved the word!\n");
        printf("The word is: %s\n", finalWord);
        printf("Game over.\n");
      }

      // Free memory for patterns
      for (int i = 0; i < patArrCurrSize; i++) {
        free(patArr[i].pat); 
      }
      free(patArr);
    }
  } while (guessLetter != '#' && !solved && guesses != 0);

  // If user run out of guesses, print solution with first word in wordList array
  if (guesses == 0) {
      printf("\nYou ran out of guesses and did not solve the word.\n");
      printf("The word is: %s\n", wordList[0]);
      printf("Game over.");
  }

  // Free dynamically allocated memory for word list
  for (int i = 0; i < wordSum; i++) {
      free(wordList[i]);
  }
  free(wordList);
  
  return 0;
}