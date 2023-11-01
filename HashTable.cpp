#include<string>
#include<iostream>
using namespace std;
class HashTable {
private:
	typedef struct element {
		string key;
		int value;
	}element;
	typedef struct vNode {
		bool isEmpty;
		element e;
		struct vNode* next;
	}vNode;

	vNode** table;//Ö¸ÕëÊý×é
	int bucketSize;
	int valueAmount;
public:
	HashTable();
	HashTable(int bucketSize);
	bool put(string k, int v);
	bool delet(string k);
	bool isContain(string k);
	int getValue(string k);
	int getAll(string k[], int v[],int arraySize);
	int hash(const string k);
	bool init(int bs);
	int getValueAmount();
	int getBucketSize();
	double getFillFactor();

};

HashTable::HashTable()
{
	init(10);
}
HashTable::HashTable(int bucketSize) 
{
	init(bucketSize);
}

bool HashTable::put(string k, int v)
{
	int position = hash(k);
	vNode* ptr = table[position];
	if (ptr->isEmpty == false) {
		if (ptr->e.key == k) {
			ptr->e.value += v;
			return true;
		}
		while (ptr->next != NULL) {
			if (ptr->e.key == k) {
				ptr->e.value += v;
				return true;
			}
			else {
				ptr = ptr->next;
			}
		}
		vNode* listNode = (vNode*)calloc(1, sizeof(vNode));
		if (listNode == NULL)
			return false;
		listNode->isEmpty = false;
		listNode->e.key = k.substr(0);
		listNode->e.value = v;
		ptr->next = listNode;
		listNode->next = NULL;
	}
	else {
		ptr->isEmpty = false;
		ptr->e.key = k.substr(0);
		ptr->e.value = v;
	}
	valueAmount++;
	/*string* s = new string[20];
	int* t = new int[20];
	int num=getAll(s, t, 20);
	cout << "-----------------------" << endl;
	for (int i = 0; i < num; i++) {
		cout << "s: " << s[i] << "        v:" << t[i] << endl;
	}*/
	return true;
}

bool HashTable::delet(string k)
{
	int position = hash(k);
	vNode* ptr = table[position];
	if (ptr->next == NULL)
		ptr->isEmpty = true;
	else {
		while (ptr->next != NULL && ptr->next->e.key != k)
			ptr = ptr->next;
		ptr->next = ptr->next->next;
	}

	return true;
}

bool HashTable::isContain(string k)
{
	int position = hash(k);
	vNode* ptr = table[position];
	if (ptr->next == NULL)
		return ptr->isEmpty;
	else {
		while (ptr != NULL && ptr->e.key != k)
			ptr = ptr->next;
		if (ptr == NULL)
			return false;
		else
			return true;
	}
}

int HashTable::getValue(string k)
{
	int position = hash(k);
	vNode* ptr = table[position];
	if (ptr->isEmpty == true)
		return 0;
	while (ptr != NULL && ptr->e.key != k)
		ptr = ptr->next;
	return ptr->e.value;

}

int HashTable::getAll(string k[], int v[],int arraySize)
{
	if (arraySize <  valueAmount)
		return false;

	int pairSize = 0;
	for (int i = 0; i < bucketSize; i++) {
		vNode* ptr = table[i];
		if (ptr->isEmpty == true)
			continue;
		else {
			while ((ptr) != NULL) {
				k[pairSize]=(ptr->e.key).substr(0) ;
				v[pairSize]=ptr->e.value;
				pairSize++;
				ptr = ptr->next;
			}
			
		}
	}
	return valueAmount;
}



int HashTable::hash(const string k)
{
	const char* ck = k.c_str();
	unsigned int v = 0;
	while (*ck != '\0') {
		v = (v << 5) + *ck;
		ck++;
	}
	return v % bucketSize;
}

bool HashTable::init(int bs)
{
	vNode** newTable = (vNode**)calloc(bs, sizeof(vNode*));
	for (int i = 0; i < bs; i++) {
		newTable[i] = (vNode*)calloc(1, sizeof(vNode));
		newTable[i]->isEmpty = true;
		newTable[i]->next = NULL;
	}
	if (newTable == NULL)
		return false;
	table = newTable;
	bucketSize = bs;
	valueAmount = 0;
	return true;
}

int HashTable::getValueAmount()
{
	return valueAmount;
}

int HashTable::getBucketSize()
{
	return bucketSize;
}

double HashTable::getFillFactor()
{
	return (double)valueAmount / (double)bucketSize;
}



