/**------------------------------------------
    Program 1: Library Management System
    Program creates a library management system utilizing a simplified data set, with specific set of instructions that the management system can perform
    Course: CS 251, Fall 2023, UIC
    System: Windows 11
    Author: Nathan Trinh, starter code provided by Adam T. Koehler
 ---------------------------------------------**/

// You cannot use any libraries beyond the ones we have given below.
// You must use output streams, file streams, vectors, and strings.
// You are not allowed to use any other containers or objects.
// You may not use structs, pointers, or dynamic memory allocations.

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>

using namespace std;

// Function definitions for all the glorious function decomposition
// Tip: alphabetical order makes it easy to find things if you have
//      a lot of function decomposition.

/// @brief print out the main menu of the management system

// Function to output the main menu
void printMenu()
{
  cout << "Library Management Menu" << endl;
  cout << "-----------------------" << endl;
  cout << "A - Add Item To Library" << endl;
  cout << "C - Clear The Library Of All Entries" << endl;
  cout << "D - Display Library Entries" << endl;    
  cout << "I - List Invalid Library Entries" << endl;
  cout << "L - Load Library From File" << endl;    
  cout << "O - Output Library To File" << endl;
  cout << "P - Print Out Checkout Stats" << endl;
  cout << "R - Remove A Library Entry" << endl;      
  cout << "S - Search For A Library Entry" << endl;    
  cout << "X - Exit Program" << endl;
}

// Function to get the description for a checkout status
// Parameter: string copy of status
// Return: string description of given status
string statusCheck(string status) 
{
  if (status == "0") {
    return "In Library";
  } 
  else if (status == "1") {
    return "Checked Out";
  } 
  else if (status == "2") {
    return "On Loan";
  } 
  else if (status == "3") {
    return "Unknown State";
  } 
  else {
    return "Invalid State";
  } 
}

// Function to calculate and validate ISBN
// Parameter: string copy of ISBN
// Return: boolean that validates the ISBN
bool validISBN(string isbn) 
{
  if (isbn.length() != 17) {
    return false; // ISBN must be exactly 17 characters (13 digits, 4 dashes)
  }

  // Initialize the array and index counter to store digits
  int arr[13] = {0};  
  size_t arrIndex = 0;

  // Convert characters to integers and store them in the array
  for (int i = 0; i < isbn.length(); ++i) {
    if (isbn.at(i) == '-') {
      continue;
    }
      
    else {
      arr[arrIndex] = isbn.at(i) - '0'; 
      ++arrIndex;  
    }
  }
  
  // Validate the ISBN checksum
  int sum = 0;

  // Loop to add to sum, mutiplying by 3 if the index is odd
  for (int i = 0; i < 12; i++) {
    if (i % 2 == 0) 
      sum += arr[i];
    else
      sum += 3 * arr[i];
  }

  int checksum = 10 - (sum % 10);
  if (checksum == 10) {
    checksum = 0; // Avoid double digits edge case
  }

  return checksum == arr[12];  // Compare calculated checksum with last digit
}

// Function to add a library entry
// Parameters: Passed by reference of 3 book data vectors
void addEntry(vector<string>& name, vector<string>& id, vector<string>& status) 
{
  string inputName;
  string inputID;

  // Prompt the user to enter book title
  cout << "What is the book title? ";
  getline(cin, inputName);

  // Check if book title contains commas
  if (inputName.find(",") != string::npos) {
    cout << endl << "The book title cannot contain commas." << endl;
    return;
  }

  // Prompt the user to enter book ISBN and populate the vectors with user inputs
  name.push_back(inputName);
  cout << endl << "What is the 13-digit ISBN (with hyphens)? ";
  getline(cin, inputID);
  id.push_back(inputID);
  status.push_back("0");

  // Display added entry
  cout << endl << "The Following Entry Was Added" << endl;
  cout << "-----------------------------" << endl;
  cout << inputName << " --- " << inputID << " --- " << "In Library" << endl;
}

// Function to clear all library entries
// Parameters: Passed by reference of 3 book data vectors
void clear(vector<string>& name, vector<string>& id, vector<string>& status) 
{
  // Clear all data in vectors
  name.clear();
  id.clear();
  status.clear();

  cout << "Your library is now empty." << endl;
}

// Function to display library entries
// Parameters: Copy of 3 book data vectors
void displayEntries(vector<string> name, vector<string> id, vector<string> status) 
{
  cout << "Your Current Library" << endl;
  cout << "--------------------" << endl;

  // Check if the library is empty
  if (name.size() == 0) {
    cout << "The library has no books." << endl;
  }

  // Loop through the vectors and display each library entry
  for (int i = 0; i < name.size(); i++) {
  cout << name.at(i) << " --- " << id.at(i) << " --- "
    << statusCheck(status.at(i)) << endl;
  }
}

// Function to list invalid library entries
// Parameters: Copy of 3 book data vectors
void listInvalid(vector<string> name, vector<string> id, vector<string> status) 
{
  bool check = false;
  
  cout << "The Following Library Entries Have Invalid Data" << endl;
  cout << "-----------------------------------------------" << endl;

  // Check if the library is empty
  if (name.size() == 0) {
    cout << "The library has no books." << endl;
    return;
  }

  // Iterate through vectors to check and display invalid entries
  for (int i = 0; i < id.size(); i++) {
    if (statusCheck(status.at(i)) == "Invalid State" || !validISBN(id.at(i))) {
      cout << name.at(i) << " --- " << id.at(i) << " --- "
        << statusCheck(status.at(i)) << endl; 
      check = true;
    } 
  }

  // Outputs if no invalid entries were found
  if (!check)
    cout << "The library has no invalid entries." << endl;
}

// Function to load library data from a file
// Parameters: Passed by reference of 3 book data vectors
void loadFile(vector<string>& name, vector<string>& id, vector<string>& status) 
{
  string inputFile;
  string inputName;
  ifstream inFile;
  string inputID = "000-0-00-000000-0";
  string inputStatus = "0";
  string line;
  int count = 0;

  // Prompt user to enter file name
  cout << "What database to read from? ";
  getline(cin, inputFile);
  inFile.open(inputFile);

  // Check if the file could not be opened
  if (!inFile.is_open()) {
    cout << endl << "Could not find the database file." << endl;
    return;
  }

  // Read data from the file and populate the vectors
  while (getline(inFile, line)) {
    int pos1 = line.find(','); // Find position of the first comma
    int pos2 = line.find(',', pos1 + 1); // Find position of the second comma

    // Populate the vectors with default ID and status if both are not found
    if (pos1 == string::npos) {
      name.push_back(line);
      id.push_back("000-0-00-000000-0");
      status.push_back("0");
    }

    // Populate the vectors with default status if not found
    else if (pos2 == string::npos) {
      name.push_back(line.substr(0, pos1));
      id.push_back(line.substr(pos1 + 2, line.length() -1));
      status.push_back("0");
    }

    // Populate the vectors with given data
    else {
      name.push_back(line.substr(0, pos1));
      id.push_back(line.substr(pos1 + 2, pos2 - pos1 - 2));
      status.push_back(line.substr(pos2 + 2));
    }
    count++;
  }

  inFile.close();
  cout << endl << "Read in " << count << " lines from the file." << endl;
}

// Function to output library data to a file
// Parameters: Copy of 3 book data vectors
void outputToFile(vector<string> name, vector<string> id, vector<string> status) 
{
  ofstream outStream;
  string outFile;
  int count = 0;

  // Prompt user to enter file name
  cout << "Where should the database output to? ";
  getline(cin, outFile);
  outStream.open(outFile);

  // Check if the output file could not be opened
  if (!outStream.is_open()) {
      cout << "Could not open file " << outFile << endl;
      return;
   }

  // Write data to the output file
  for (int i = 0; i < name.size(); i++) {
    outStream << name.at(i) << ", " << id.at(i) << ", " << status.at(i) << endl;
    count++;
  }

  outStream.close();
  cout << count << " lines of data written to " << outFile << endl;
}

// Function to print checkout statistics
// Parameter: Copy of book status vector
void printCheckOutStats(vector<string> status) 
{
  // Initialize counters for different book statuses
  int inLibrary = 0, checkedOut = 0, onLoan = 0, unknown = 0, other = 0;

  // Display library statistics header
  cout << "Your Current Library's Stats" << endl;
  cout << "----------------------------" << endl;
  cout << "Total Books: " << status.size() << endl;

  // Loop through the status vector to count different book statuses
  for (int i = 0; i < status.size(); i++) {
    if (status.at(i) == "0")
      inLibrary++;
    else if (status.at(i) == "1")
      checkedOut++;
    else if (status.at(i) == "2")
      onLoan++;
    else if (status.at(i) == "3")
      unknown++;
    else
      other++;
  }

  // Display counts of each book status
  cout << "   In Library: " << inLibrary << endl;
  cout << "   Checked Out: " << checkedOut << endl;
  cout << "   On Loan: " << onLoan << endl;
  cout << "   Unknown: " << unknown << endl;
  cout << "   Other: " << other << endl;
}

// Function to search for a book entry
// Parameters: Copy of 3 book data vectors
void searchEntry(vector<string> name, vector<string> id, vector<string> status) 
{
  string inputString;
  string choice;

  // Prompt the user to choose search option
  cout << "Would you like to search by (1) name or (2) ISBN." << endl;
  cout << "Enter the numeric choice: ";
  getline(cin, choice);

  // Prompt the user to choose search term, exiting if search option was invalid
  switch (stoi(choice)) {
    case 1:
      cout << endl << "Enter the book name: ";
      break;
    case 2:
      cout << endl << "Enter the book 13-digit ISBN (with dashes): ";
      break;
    default:
      cout << endl << "Invalid search by choice option." << endl;
      return;
  }

  
  getline(cin, inputString);
  cout << endl;

  // Display search results header
  cout << "The Following Are Your Search Results" << endl;
  cout << "-------------------------------------" << endl;

  // Loop through the book information vectors to find matching entries
  for (int i = name.size() - 1; i >= 0; i--) {
    if ((choice == "1" && inputString == name.at(i)) || (choice == "2" && inputString == id.at(i))) {
      // Display matching entry details
      cout << name.at(i) << " --- " << id.at(i) << " --- " << statusCheck(status.at(i)) << endl; 
      return;
    }
  }

  // Outputs if no matching entry is found
  cout << "No matching entry found in the library." << endl;
}

// Function to remove a book entry from the library
// Parameters: Passed by reference of 3 book data vectors
void removeEntry(vector<string>& name, vector<string>& id, vector<string>& status) 
{
  string inputString;
  string choice;

  // Prompt the user to choose removal option
  cout << "Would you like remove by (1) name or (2) ISBN." << endl;
  cout << "Enter the numeric choice: ";
  getline(cin, choice);

  // Prompt the user to choose removal term, exiting if removal option was invalid
  switch (stoi(choice)) {
    case 1:
      cout << endl << "Enter the book name: ";
      break;
    case 2:
      cout << endl << "Enter the book 13-digit ISBN (with dashes): ";
      break;
    default:
      cout << endl << "Invalid remove by choice option." << endl;
      return;
  }
  
  getline(cin, inputString);
  cout << endl;

  // Display removed entry header
  cout << "The Following Entry Was Removed From The Library" << endl;
  cout << "------------------------------------------------" << endl;

  // Loop through the book information vectors to find and remove the matching entry
  for (int i = name.size() - 1; i >= 0; i--) {
    if ((choice == "1" && inputString == name.at(i)) || (choice == "2" && inputString == id.at(i))) {
        // Display the removed entry details
        cout << name.at(i) << " --- " << id.at(i) << " --- " << statusCheck(status.at(i)) << endl;

        // Remove the entry from all vectors
        name.erase(name.begin() + i);
        id.erase(id.begin() + i);
        status.erase(status.begin() + i);
      
        return;
    }
  }

  // Outputs if no matching entry is found
  cout << "No matching entry found in the library." << endl;
}

int main()
{
  string command;
  vector<string> bookName;
  vector<string> ISBN;
  vector<string> checkOutStatus;

  // Display program header
  cout << "Welcome to the Library Management System" << endl;
  cout << "----------------------------------------" << endl;
  cout << endl;

  do 
  {
    // Output the menu and acquire a user selection
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // We use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "A" || command == "a") {
      addEntry(bookName, ISBN, checkOutStatus);
    }
    else if (command == "C" || command == "c") {
      clear(bookName, ISBN, checkOutStatus);
    }
    else if (command == "D" || command == "d") {
      displayEntries(bookName, ISBN, checkOutStatus);
    }
    else if (command == "I" || command == "i") {
      listInvalid(bookName, ISBN, checkOutStatus);
    }
    else if (command == "L" || command == "l") {
      loadFile(bookName, ISBN, checkOutStatus);
    }     
    else if (command == "O" || command == "o") {
      outputToFile(bookName, ISBN, checkOutStatus);
    }
    else if (command == "P" || command == "p") {
      printCheckOutStats(checkOutStatus);
    } 
    else if (command == "S" || command == "s") {
      searchEntry(bookName, ISBN, checkOutStatus);
    }
    else if (command == "R" || command == "r") {
      removeEntry(bookName, ISBN, checkOutStatus);
    } 
    else if (command == "X" || command == "x") {
      break;
    }
    else {
      cout << endl << "Unknown command." << endl;  
    }   
      cout << endl;
  } while (!(command == "X" || command == "x"));

  return 0;
}