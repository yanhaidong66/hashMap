#include<shared_mutex>
#include<thread>
#include<string>
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
	int getAll(string k[], int v[], int arraySize);
	int hash(const string k);
	bool init(int bs);
	int getValueAmount();
	int getBucketSize();
	double getFillFactor();

};
class HashMap {
private:
	HashTable* table[2];
	HashTable* usingTable;
	volatile int usingTableId;
	shared_mutex rwLock;
	const double rehashFactor = 0.8;
	const double growth = 1.5;
	volatile bool rehashing = false;
public:
	HashMap();
	bool changeUsingTable(HashTable* newTable);
	bool rehash();
	int get(string key);
	int put(string key, int value);
	int delet(string key);
	int getAll(string* key, int* value, int arraySize);
	bool init(int bucketSize);
	int getKeyAmount();
	int getBucketAmount();
	HashTable* createTable(int bucketSize, string keys[], int value[], int pairAmount);
	HashTable* getUsingTable();
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

int HashTable::getAll(string k[], int v[], int arraySize)
{
	if (arraySize < valueAmount)
		return false;

	int pairSize = 0;
	for (int i = 0; i < bucketSize; i++) {
		vNode* ptr = table[i];
		if (ptr->isEmpty == true)
			continue;
		else {
			while ((ptr) != NULL) {
				k[pairSize] = (ptr->e.key).substr(0);
				v[pairSize] = ptr->e.value;
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

HashMap::HashMap() {

	init(10);
}

HashTable* HashMap::createTable(int bucketSize, string keys[], int values[], int pairAmount) {
	HashTable* r = new HashTable(bucketSize);
	for (int i = 0; i < pairAmount; i++) {
		r->put(keys[i], values[i]);
	}
	return r;

}
bool HashMap::changeUsingTable(HashTable* newTable)
{
	std::unique_lock<std::shared_mutex> writeLock(rwLock);
	if (usingTableId == 0) {
		delete(table[1]);
		table[1] = newTable;
		usingTable = table[1];
	}
	else {
		delete(table[0]);
		table[0] = newTable;
		usingTable = table[0];
	}
	return true;
}

HashTable* HashMap::getUsingTable() {
	std::shared_lock<std::shared_mutex> readLock(rwLock);
	return usingTable;
}

bool HashMap::rehash()
{
	rehashing = true;
	int valueAmount = usingTable->getValueAmount();
	int bucketAmount = usingTable->getBucketSize();
	string* keys = new string[valueAmount];
	int* values = new int[valueAmount];
	getUsingTable()->getAll(keys, values, valueAmount);
	HashTable* newTable = createTable((int)(valueAmount * growth), keys, values, valueAmount);
	changeUsingTable(newTable);
	rehashing = false;
	delete[] keys;
	delete[] values;
	return true;
}

int HashMap::get(string key)
{
	return getUsingTable()->getValue(key);

}

int HashMap::put(string key, int value)
{
	if (rehashing == false && getUsingTable()->getFillFactor() - rehashFactor >= 0.00001) {
		rehashing = true;
		thread rehashThread(&HashMap::rehash, this);
		rehashThread.detach();
	}
	getUsingTable()->put(key, value);
	return 1;
}

int HashMap::delet(string key)
{
	return getUsingTable()->delet(key);

}

int HashMap::getAll(string* key, int* value, int arraySize)
{
	int num = getUsingTable()->getAll(key, value, arraySize);
	return num;
}

bool HashMap::init(int bucketSize) {
	table[0] = new HashTable(10);
	table[1] = new HashTable(10);
	table[0]->init(bucketSize);
	usingTableId = 0;
	usingTable = table[0];
	return true;
}

int HashMap::getKeyAmount()
{
	return getUsingTable()->getValueAmount();
}

int HashMap::getBucketAmount()
{
	return getUsingTable()->getBucketSize();
}