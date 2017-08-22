#ifndef st_h
#define st_h

struct SuffixTreeNode;
typedef struct SuffixTreeNode Node;
void buildSuffixTree(char* txt);
void checkForSubString(char* str);
void freeSuffixTreeByPostOrder(Node *n);
std::vector<long> getMatchIndex();
void clearMatchIndex();
Node* getRoot();

#endif /* st_h */
