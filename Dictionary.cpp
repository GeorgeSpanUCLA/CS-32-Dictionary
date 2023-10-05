// Dictionary.cpp

// This is a correct but horribly inefficient implementation of Dictionary
// functionality.  Your job is to change DictionaryImpl to a more efficient
// correct implementation.  You can change it any way you like, subject to
// restrictions in the spec (e.g., that the only C++ library container you
// are allowed to use are vector, list, stack, and queue (and string); if you
// want anything fancier, implement it yourself).                                                                                                                    

#include "Dictionary.h"
#include <string>
#include <list>
#include <cctype>
#include <utility>  // for swap
#include<vector>
#include <algorithm>
#include <functional>
using namespace std;

void removeNonLetters(string& s);

// This class does the real work of the implementation.

class DictionaryImpl
{
public:
    DictionaryImpl(int i);
    ~DictionaryImpl();
    void insert(string word);
    void lookup(string letters, void callback(string)) const;
    void Resize();

    
private:
    int maxsize;        //max size of the hash table 
    double loadF;       //hash table load factor 
     
    struct cont {       //struct containing the word and its sorted version
        string sorted;
        string word;
    };
    class HashTable {
    public:
        HashTable(int size);
        void insert(string s);      //hash table insert function
        //int hash(string word, string& sorted);
        hash<string> hasher;        //hash key function 
        int numofItems; //number of items in the hash table 
        int size;       //number of hash table conntainers 
        int search(string word, bool& contains, string& sorted);    //hash table search function 
       typedef list<cont> l;        // link list to the cont struct 
        vector<l*> buckets;         //hash table vector with pointers to the linked list 
    };
    HashTable* current;         //pointer to the dictionary's hash table 
   
};


DictionaryImpl::DictionaryImpl(int max) // dictionary constructor 
    :maxsize(max), loadF(0)
{
    int hassize;
   
        hassize = max;
    if(max == 20000)            //if the hash table initial size it too lagre create a smaller one 
        hassize = max / 4;
    current = new HashTable(hassize);       //create the hash table with that size
    
}




void removeNonLetters(string& s)// function that removes all non letter characters from the input 
{
    string::iterator to = s.begin();
    for (string::const_iterator from = s.begin(); from != s.end(); from++)
    {
        if (isalpha(*from))
        {
            *to = tolower(*from);
            to++;
        }
    }
    s.erase(to, s.end());  // chop everything off from "to" to end.
}

DictionaryImpl::HashTable::HashTable(int siz)       // hash table constructor
     :buckets(siz, nullptr) {       //create a vector of size siz with nullptrs 
    
    size = siz;
    numofItems = 0;
}

//hash table search function implementation 
int DictionaryImpl::HashTable::search(string word, bool& contains, string& sorted) {
    sorted = word;
    sort(sorted.begin(), sorted.end()); //get the sorted version of word 
    size_t index = hasher(sorted)%size;  //pass it through the has function and use remainder of size to make sure its within the table size 
    
    if (buckets[index] == nullptr) {        //if there is nothing stored with the same hash function the word does not have anagrams  
        contains = false;
        return -1;
    }

    if (buckets[index]->empty()) {      //if all elements in the bucket have been removed no anagrams exist 
        contains = false;
        return-1;
    }
    //else search the bucket list for words with the same sorted value 
    for (list<cont>::iterator p = buckets[index]->begin(); p != buckets[index]->end();p++) {
        if (p->sorted == sorted) {
            contains = true;
            return index;   //return the bucket address 
        }
    }
    contains = false;   //if no sorted match is found return false 
    return -1;
    
}
void DictionaryImpl::lookup(string letters, void callback(string)) const    //dictionary lookup function 
{
    if (callback == nullptr)        //if there is nothing to do with anagrams end 
        return;

    removeNonLetters(letters);      //remove non letter characters 
    if (letters.empty())            //if there is nothing left end 
        return;

    bool isthere;
    string sorted;
    int index = current->search(letters, isthere, sorted);      //call the hash table search function to get index 
    if (isthere == true) {      //check if there are any anagrams 
        for (list<cont>::iterator i = current->buckets[index]->begin(); i != current->buckets[index]->end(); i++) {
            if (i->sorted == sorted) {  //go through anagrams and call the callback function 
                callback(i->word);
            }
        }
    }
}

DictionaryImpl::~DictionaryImpl() {         //dictionary distructor 
    for (int i = 0; i < current->buckets.size(); i++) {     // go through containers 
        if (current->buckets[i] != nullptr) {       // check if the container is empty 
            current->buckets[i]->clear();           //if its not clear the linked list 
            delete current->buckets[i];             //delete the pointer in the contianer 
        }
    }
    delete current;                     //deleter the hashtable 
}

//hash table resize
void DictionaryImpl::Resize() {
   
        int newSize = current->size * 2;        //increase the max size of the container by 2 
        if (newSize > maxsize)                  // if the new size excides the max size set that into the new size 
            newSize = maxsize;
        HashTable* p = new HashTable(newSize);      //create a new hash table of the new size 

        for (int i = 0; i < current->buckets.size(); i++) {     //go through the old hash table 
            if (current->buckets[i] != nullptr && !current->buckets[i]->empty()) {  //if bucket is not empty 
                for (list<cont>::iterator s = current->buckets[i]->begin(); s != current->buckets[i]->end(); s++) { //go through the linked list 
                    p->insert(s->word); //insert words into the new hash table 
                    current->numofItems--;      //reduce the number of items in the old table 
                }
                current->buckets[i]->clear();       //delete the old table 
                delete current->buckets[i];
                current->size--;
            }
        }

        delete current;
        current = p;    //swap their addresses
    }

//hash table insert implementation 
void DictionaryImpl::HashTable::insert(string word) {
    string s = word;    
    sort(s.begin(), s.end());       //sort the word 
    size_t bucket = hasher(s) % size;       //get hash value for sorterd word 
    if (buckets[bucket] == nullptr)     //if the bucket is empty
        buckets[bucket] = new l;        //create a new one 
    cont* p = new cont;                 // create a new struct cont 
    p->sorted = s;              // add the sorted version in the struct 
    p->word = word;             // add the unsorted version in the struct 
    buckets[bucket]->push_back(*p); //push the new struct into linked list 
    numofItems++;       //increament the number of items in the hash table 
    delete p;           //delete the pointer to the new struct 
}

//dictionary insert 
void DictionaryImpl::insert(string word)
{
    removeNonLetters(word); //remove all non letter characters
    if (!word.empty())
        current->insert(word);  //insert word into hash table 


    loadF = static_cast<double>(current->numofItems) / current->size;//check if the load size exceeds optimal level 
    if (current->size != maxsize) {     //check if the size is already max
        if (loadF >= 0.75) {
            Resize();       //if not max resize 
        }
    }
    
}






//******************** Dictionary functions ******************************

// These functions simply delegate to DictionaryImpl's functions
// You probably don't want to change any of this code

Dictionary::Dictionary(int maxBuckets)
{
    m_impl = new DictionaryImpl(maxBuckets);
}

Dictionary::~Dictionary()
{
    delete m_impl;
}

void Dictionary::insert(string word)
{
    m_impl->insert(word);
}

void Dictionary::lookup(string letters, void callback(string)) const
{
    m_impl->lookup(letters, callback);
}
