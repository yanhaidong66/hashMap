#include"HashTable.cpp"
#include<shared_mutex>
#include<thread>
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
	int getAll(string* key, int* value,int arraySize);
	bool init(int bucketSize);
	int getKeyAmount();
	int getBucketAmount();
	HashTable* createTable(int bucketSize,string keys[],int value[],int pairAmount);
	HashTable* getUsingTable();
};

HashMap::HashMap() {
	
	init(10);
}

HashTable* HashMap::createTable(int bucketSize,string keys[],int values[],int pairAmount) {
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
	cout << "rehashing";
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
	if (rehashing==false&&getUsingTable()->getFillFactor() - rehashFactor >= 0.00001) {
		rehashing = true;
		thread rehashThread(&HashMap::rehash,this);
		rehashThread.detach();
	}
	getUsingTable()->put(key, value);
	/*cout << "bucketAmount:" << getBucketAmount();
	cout << "          factor:" << getUsingTable()->getFillFactor();
	cout << "        keyAmount:" << getKeyAmount() << endl;*/
	/*string* s = new string[20];
	int* t = new int[20];
	int num=getUsingTable()->getAll(s, t, 20);
	cout << "--------------------" << endl;
	for (int i = 0; i < num; i++) {
		cout <<"k:"<< s[i]<<"       v:" << t[i] << endl;
	}*/
	return 1;
}

int HashMap::delet(string key)
{
	return getUsingTable()->delet(key);
	
}

int HashMap::getAll(string* key, int* value,int arraySize)
{
	int num=getUsingTable()->getAll(key, value,arraySize);
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

int main() {
	HashMap* ht = new HashMap();
	ht->put("1", 1);
	ht->put("2", 2);
	ht->put("3", 3);
	ht->put("4", 4);
	ht->put("5", 5);
	ht->put("6", 6);
	string s[29];
	int v[29];
	ht->put("7", 7);
	ht->put("8", 8);
	ht->put("9", 9);
	ht->put("10", 10);
	ht->put("11", 11);
	ht->put("12", 12);
	ht->put("13", 13);
	ht->put("14", 14);
	ht->put("15", 15);
	ht->put("16", 16);
	ht->put("17", 17);
	ht->put("18", 18);
	ht->put("19", 19);
	ht->put("20", 20);
	ht->put("21", 21);
	ht->put("22", 22);
	/*int si = ht->getAll(s, v, ht->getKeyAmount());
	cout << "-------------" << endl;
	for (int i = 0; i < si; i++) {
		cout << " k:" << s[i];
		cout << "         v:" << v[i];
		cout << "       bucketAmount:" << ht->getBucketAmount();
		cout << "          factor:" << ht->getUsingTable()->getFillFactor();
		cout << "        keyAmount:" << ht->getKeyAmount() << endl;
	}*/

}