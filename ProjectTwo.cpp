
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


// Data Structure Definitions
//=============================================================

// Define a structure to hold course information
struct Course {
    string courseId;
    string title;
    vector<string> prerequisites;
    
    // Default constructor
    Course() {}
};

// Internal structure for tree node
struct Node {
    Course course;
    Node *left;
    Node *right;

    // Default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // Initialize with a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }
};


// Binary Search Tree Class
//===================================================================

class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course);
    void inOrder(Node* node);
    void destroyRecursive(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void Insert(Course course);
    void PrintCourseList();
    Course Search(string courseId);
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    destroyRecursive(root);
}

/**
 * Recursively destroy nodes to free memory
 */
void BinarySearchTree::destroyRecursive(Node* node) {
    if (node != nullptr) {
        destroyRecursive(node->left);
        destroyRecursive(node->right);
        delete node;
    }
}

/**
 * Insert a course into the tree
 */
void BinarySearchTree::Insert(Course course) {
    // If root is null, this course becomes the root
    if (root == nullptr) {
        root = new Node(course);
    } else {
        // Otherwise, add to the tree
        addNode(root, course);
    }
}

/**
 * Recursive addNode function
 */
void BinarySearchTree::addNode(Node* node, Course course) {
    // If courseId is less than the current node's courseId, traverse left
    if (course.courseId < node->course.courseId) {
        if (node->left == nullptr) {
            node->left = new Node(course);
        } else {
            addNode(node->left, course);
        }
    } 
    // Otherwise, traverse right
    else {
        if (node->right == nullptr) {
            node->right = new Node(course);
        } else {
            addNode(node->right, course);
        }
    }
}

/**
 * Print the schedule in alphanumeric order (in-order traversal)
 */
void BinarySearchTree::PrintCourseList() {
    inOrder(root);
}

/**
 * Recursive In-Order traversal function
 */
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseId << ", " << node->course.title << endl;
        inOrder(node->right);
    }
}

/**
 * Search for a course by its ID
 */
Course BinarySearchTree::Search(string courseId) {
    Node* current = root;

    while (current != nullptr) {
        // Match found
        if (current->course.courseId == courseId) {
            return current->course;
        }
        // If search ID is smaller, go left
        if (courseId < current->course.courseId) {
            current = current->left;
        } 
        // Otherwise, go right
        else {
            current = current->right;
        }
    }

    // Return an empty course if not found
    Course emptyCourse;
    return emptyCourse;
}


// Helper Functions
//===================================================================

/**
 * Helper function to convert a string to uppercase
 * Ensures case-insensitive searching
 */
string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

/**
 * Split a string by a delimiter into a vector
 * Used for parsing CSV lines
 */
vector<string> splitString(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    
    while (getline(tokenStream, token, delimiter)) {
        // Clean up any carriage return characters at the end of the line
        if (!token.empty() && token.back() == '\r') {
            token.pop_back();
        }
        // Exclude empty tokens (trailing commas)
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

/**
 * Load courses from a CSV file into the binary search tree
 */
void loadCourses(string csvPath, BinarySearchTree* bst) {
    ifstream file(csvPath);
    
    if (!file.is_open()) {
        cout << "Error: File '" << csvPath << "' not found." << endl;
        return;
    }

    string line;
    int count = 0;
    
    while (getline(file, line)) {
        vector<string> tokens = splitString(line, ',');
        
        // A valid course line must have at least an ID and a Title
        if (tokens.size() < 2) {
            continue; 
        }

        Course course;
        course.courseId = tokens[0];
        course.title = tokens[1];
        
        // Any remaining tokens are prerequisites
        for (size_t i = 2; i < tokens.size(); ++i) {
            course.prerequisites.push_back(tokens[i]);
        }
        
        bst->Insert(course);
        count++;
    }
    
    file.close();
    cout << "Successfully loaded " << count << " courses from file." << endl;
}


// Main Program
//======================================================================

int main() {
    BinarySearchTree* bst = new BinarySearchTree();
    int choice = 0;
    string filename;
    string searchKey;
    Course foundCourse;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << "\n  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course." << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";
        
        cin >> choice;

        // Input validation for non-integer inputs
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1:
                cout << "Enter the file name to load (e.g., CS 300 ABCU_Advising_Program_Input.csv): ";
                cin.ignore();
                getline(cin, filename);
                loadCourses(filename, bst);
                break;

            case 2:
                cout << "Here is a sample schedule:" << endl;
                cout << "---------------------------------------" << endl;
                bst->PrintCourseList();
                break;

            case 3:
                cout << "What course do you want to know about? ";
                cin >> searchKey;
                
                // Convert to uppercase for exact matching
                searchKey = toUpper(searchKey);
                foundCourse = bst->Search(searchKey);

                if (!foundCourse.courseId.empty()) {
                    cout << "\n" << foundCourse.courseId << ", " << foundCourse.title << endl;
                    cout << "Prerequisites: ";
                    
                    if (foundCourse.prerequisites.empty()) {
                        cout << "None" << endl;
                    } else {
                        // Loop through and print out prerequisites separated by commas
                        for (size_t i = 0; i < foundCourse.prerequisites.size(); ++i) {
                            cout << foundCourse.prerequisites[i];
                            if (i != foundCourse.prerequisites.size() - 1) {
                                cout << ", ";
                            }
                        }
                        cout << endl;
                    }
                } else {
                    cout << "Course not found." << endl;
                }
                break;

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    delete bst;
    return 0;
}
