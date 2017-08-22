// C++ program for building LCP array for given text
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>


// Structure to store information of a suffix
struct suffix
{
    int index;  // To store original index
    int rank[2]; // To store ranks and next rank pair
};

// A comparison function used by sort() to compare two suffixes
// Compares two pairs, returns 1 if first pair is smaller
int cmp(struct suffix a, struct suffix b)
{
    return (a.rank[0] == b.rank[0])? (a.rank[1] < b.rank[1] ?1: 0):
    (a.rank[0] < b.rank[0] ?1: 0);
}

// This is the main function that takes a string 'txt' of size n as an
// argument, builds and return the suffix array for the given string
std::vector<int> buildSuffixArray(std::string txt, int n)
{
    // A structure to store suffixes and their indexes
    struct suffix suffixes[n];
    
    // Store suffixes and their indexes in an array of structures.
    // The structure is needed to sort the suffixes alphabatically
    // and maintain their old indexes while sorting
    for (int i = 0; i < n; i++)
    {
        suffixes[i].index = i;
        suffixes[i].rank[0] = txt[i] - 'a';
        suffixes[i].rank[1] = ((i+1) < n)? (txt[i + 1] - 'a'): -1;
    }
    
    // Sort the suffixes using the comparison function
    // defined above.
    std::sort(suffixes, suffixes+n, cmp);
    
    // At his point, all suffixes are sorted according to first
    // 2 characters.  Let us sort suffixes according to first 4
    // characters, then first 8 and so on
    int ind[n];  // This array is needed to get the index in suffixes[]
    // from original index.  This mapping is needed to get
    // next suffix.
    for (int k = 4; k < 2*n; k = k*2)
    {
        // Assigning rank and index values to first suffix
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;
        
        // Assigning rank to suffixes
        for (int i = 1; i < n; i++)
        {
            // If first rank and next ranks are same as that of previous
            // suffix in array, assign the same new rank to this suffix
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i-1].rank[1])
            {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            }
            else // Otherwise increment rank and assign
            {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }
        
        // Assign next rank to every suffix
        for (int i = 0; i < n; i++)
        {
            int nextindex = suffixes[i].index + k/2;
            suffixes[i].rank[1] = (nextindex < n)?
            suffixes[ind[nextindex]].rank[0]: -1;
        }
        
        // Sort the suffixes according to first k characters
        std::sort(suffixes, suffixes+n, cmp);
    }
    
    // Store indexes of all sorted suffixes in the suffix array
    std::vector<int>suffixArr;
    for (int i = 0; i < n; i++)
        suffixArr.push_back(suffixes[i].index);
    
    // Return the suffix array
    return  suffixArr;
}

// A suffix array based search function to search a given pattern
// 'pat' in given text 'txt' using suffix array suffArr[]
//int search(char *pat, char *txt, int *suffArr)
//{
//    int m = strlen(pat);  // get length of pattern, needed for strncmp()
//    int n = strlen(txt);
//    
//    // Do simple binary search for the pat in txt using the
//    // built suffix array
//    int l = 0, r = n-1;  // Initilize left and right indexes
//    while (l <= r)
//    {
//        // See if 'pat' is prefix of middle suffix in suffix array
//        int mid = l + (r - l)/2;
//        int res = strncmp(pat, txt+suffArr[mid], m);
//        
//        // If match found at the middle, print it and return
//        if (res == 0)
//        {
//            return suffArr[mid];
//        }
//        
//        // Move to left half if pattern is alphabtically less than
//        // the mid suffix
//        if (res < 0) r = mid - 1;
//        
//        // Otherwise move to right half
//        else l = mid + 1;
//    }
//    
//    // We reach here if return statement in loop is not executed
//    return -1;
//}

// Utility function to print an array
void printArr(std::vector<int>arr, int n)
{
    for (int i = 0; i < n; i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

// Driver program
//int main()
//{
//    string str = "banana";
//    
//    vector<int>suffixArr = buildSuffixArray(str, str.length());
//    int n = suffixArr.size();
//    
//    cout << "Suffix Array : \n";
//    printArr(suffixArr, n);
//    
//    return 0;
//}