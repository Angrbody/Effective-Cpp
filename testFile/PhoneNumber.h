#include<iostream>
#include<vector>
#include<string>
using namespace std;
class PhoneNumber{};
class ABEntry {
public:
    ABEntry(const string& name, const string& address, const list<PhoneNumber>& phones);
private:
    string theName;
    string theAddress;
    list<PhoneNumber> thePhones;
    int numTimesConsulted;
};
ABEntry::ABEntry(const string& name, const string& address, const list<PhoneNumber>& phones){
    theName = name;         //这些都是赋值assignment，而非初始化initialization
    theAddress = address;
    thePhones = phones;
    numTimesConsulted = 0;
}
class FileSystem {
public:
    std::size_t numDisks() const;
};
extern FileSystem tfs;

class Directory {
public:
    Directory(params);
};
Directory::Directory(params){
    std::size_t disks = tfs.numDisks();
}
Directory tempDir(params);
