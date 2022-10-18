@[TOC](目录)
# 条款04：确定对象被使用前已先被初始化
# - Make sure that objects are initialized before they're used.
如下列代码所示，读取未初始化的值会导致不明确的行为：

```cpp
//textBlock.h
class TextBlock{
public:	
	void printXYZ(){
		std::cout<<x<<" "<<y<<" "<<z<<std::endl;
	}
private:
	std::string text;
	int x;
	int y;
	int z;
};
```

```cpp
//main.cpp
#include"textBlock.h"
using namespace std;
int main(){
    TextBlock T;
    T.printXYZ();
    system("pause");
return 0;
}
```
未初始化的三个in-class成员变量
![在这里插入图片描述](https://img-blog.csdnimg.cn/114249b65638427493775c24099e9578.png)

**不及时完成初始化会对程序带来不可预知的麻烦，因此应该永远在使用对象之前先进行初始化**
##  1.对无任何成员的内置类型进行初始化

```cpp
#include<iostream>
...
int x = 0;		//对int进行手工初始化
const char* text = "A - C-style string";	//对指针进行手工初始化
double d;
std::cin >> d;	//以读取input stream的方式完成初始化
```
## 2.对非内置类型进行初始化

 - 对于用户定义的非内置类型，通常使用**构造函数**进行初始化
 - **规则：确保每一个构造函数都将对象的每一个成员初始化**
 - 请使用构造函数的**初值列**：区分赋值(assignment)与初始化(initialization)的区别
 

```cpp
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

```
上图中对ABEntry类的构造函数进行了类外实现，此构造函数里进行的操作皆为赋值操作，且发生在隐式的default构造函数之前，也就是说这些操作曾经已经被执行一次，因而使得代码的效率降低。
应使用成员函数初值列替换复制动作：

```cpp
ABEntry::ABEntry(const string& name, const string& address, const list<PhoneNumber>& phones)
  ：theName(name),         //利用初值列进行初始化
    theAddress(address),
    thePhones (phones),
    numTimesConsulted(0)
{}
```
构造一个成员变量的默认值：
```cpp
ABEntry::ABEntry()		//无参数构造函数（可以重载）
  ：theName(),         
    theAddress(),
    thePhones (),
    numTimesConsulted(0)
{}
```
 - 使用**初值列**的注意事项：
 - 确保在初值列中列出所有成员变量，如果是内置类型（如上例中的int numTimesConsulted）则必须为其赋初值。
 - 当class拥有多个构造函数时，多份成员初值列可能会存在大量重复，此时可以合理的选择隐掉初值列中那些“赋值和初始化效率一样的成员变量”，使用某个函数（通常是private）统一对这些变量进行赋值操作。
 - 成员初值列中的赋值顺序最好和声明次序保持一致（以避免逻辑错误）
## 3.“不同编译单元内定义之non-local static对象”的初始化次序
### 3.1 名词解释：local static对象与编译单元
 - static对象即从构造到整个程序结束为止一直存在的静态对象，其包括*global对象、定义与namespace作用域内的对象、以及classes内、函数内、file作用域内被声明为static的对象*。
 - 函数内的static对象成为local static对象（于函数是local），其他static对象称为non-local static对象
 - 编译单元指产出单一目标文件的那些源码，由单一源码文件(cpp)+其含入的头文件(.h)组成

### 3.2 两个（以上）源码中non-local static对象无法确定初始化次序

```cpp
//FileSystem.h
class FileSystem {		//来自你的程序库
public:
    std::size_t numDisks() const;	//众多成员函数之一，返回size_t类型
};
extern FileSystem tfs;	//为客户使用而预留的对象。tfs = the file system

//Directory.h
class Directory {		//客户自定义处理文件系统的目录类
public:
    Directory(params);
};
Directory::Directory(params){
    std::size_t disks = tfs.numDisks();	//使用tfs对象
}

//Directory.cpp
Directory tempDir(params);	//为临时文件而做出来的目录

```

 - 注意上例中两个构造函数的使用顺序：除非tfs在tempDir之前先被初始化，否则tempDir的构造函数会用到尚未初始化的tfs。但tfs和tempDir是定义在不同编译单元内的non-local static对象，无法保证tfs先于tempDir初始化。
 ### 3.3 使用Singleton模式实现封装 - 将non-local 转为 local
 - 原理：C++保证函数内部的local static对象会在“该函数被调用期间”、“首次遇上该对象之定义式”时被初始化
 - 做法：以“函数调用”（返回一个reference指向local static对象）替换“直接访问non-local static对象”
 - 为什么值得做：
 1.提供封装以保证任何调用顺序都有其逻辑正确性
 2.如果不调用则节省了构造和析构成本
- 改进后的class设计
```cpp
//FileSystem.h
class FileSystem {		//来自你的程序库
public:
    std::size_t numDisks() const;	//众多成员函数之一，返回size_t类型
};
FileSystem& tfs(){		//为客户使用而预留的对象。tfs = the file system
	static FileSystem fs;
	return fs;
}

//Directory.h
class Directory {		//客户自定义处理文件系统的目录类
public:
    Directory(params);
};
Directory::Directory(params){
    std::size_t disks = tfs().numDisks();	//使用tfs对象
}
Directory& tempDir(){	//为临时文件而做出来的目录
	static Director td;
	return td;
}
```
 - 注意事项：必须自己设计合理的class，避免两个对象的初始化逻辑冲突
## 4.总结
 1. 为内置型对象进行手工初始化
 2. 构造函数最好使用成员初值列代替赋值操作
 3. 在跨编译单元初始化次序问题中以local static对象替换non-local static对象

