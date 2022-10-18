# 条款03:尽可能使用const - Use const whenever possible
@[TOC] 
## 一、多才多艺的const
- 使用const代表我们需要一个**不应该被任何人改动的**对象,在你定义了一个const对象后,编译器会帮助你强制实施这项约束,告诉其他程序或程序员某值应该保持不变.
- const的基本用法:
### 1. 在classes外部修饰global或namespace作用域中的常量;

```cpp
//const.h
namespace myconst{
    const int id = 1;
    const double height = 60.5;
}
class A{
...
};
```

```cpp
//const.cpp
#include<iostream>
#include"const.h"
using namespace myconst;
using namespace std;
const int global = 1;
int main(){
    std::cout<<"id : "<<id<<std::endl;
    std::cout<<"height : "<<height<<std::endl;
    std::cout<<"global : "<<global<<std::endl;
    system("pause");
    return 0;
}
```
输出结果:
![在这里插入图片描述](https://img-blog.csdnimg.cn/6b5311292aca4caaa79505abd9a9cf69.png)


### 2. 修饰文件,函数或区块作用域(block scope)中被声明为static的对象
const 对象在作用域结束后会自动释放, 而static创建的对象在静态区, 不会被自动释放 const static = static const.
### 3. 修饰classes内部的static和non-static成员变量
```cpp
class Node{
public:
	const static int myself;	//等同于static const
private:
	const int left = 0;	//错误操作,const类型的对象不可在类中直接赋初值,需要构造函数
	const int left;		//正确操作
};
```
- 需要注意的是const static/static const 在大部分编译器环境中(除了老VC)可以在类内直接初始化,而const / static不可以, 需要通过构造函数或类外赋值.
### 4. 面向指针时:为指针本身和指针所指的内容进行修饰

```cpp
	char greeting[] = "hello";  //字符数组greeting
    char* p = greeting;         //非常量指针，非常量数据
    const char* p = greeting;   //非常量指针，常量数据
    char* const p = greeting;   //常量指针，非常量数据
    const char* const p = greeting; //常量指针，常量数据
```
当对 const 对象做出write操作时:
![在这里插入图片描述](https://img-blog.csdnimg.cn/34735118a94f45ccb819801e4f925073.png)
- 区分原则: 若 `const` 出现在 * 的==左侧==, 则 `const` 修饰的==对象==是常量, 反之则指针本身是常量.

注意: 若被指物是常量, `const Widget * pw` 与 `Widget const * pw` 两种写法是等价的, 因为虽然绝对位置不同,但两个 `const` 都位于 * 左侧.

### 5. const iterator, 常量迭代器
因STL迭代器需要指向不同类型的数据, 故其类似于一个T* 指针, const用法:

```cpp
	vector<int> vec1;
    vector<int> vec2;
    vec1.resize(2,0);
    vec2.resize(2,0);
    const vector<int>::iterator iter = vec1.begin();    //const 迭代器, 相当于 T* const
    *iter = 10; 
    ++iter;     //error！

    vector<int>::const_iterator cter = vec2.begin();    //const data, 相当于 const T*
    ++cter;
    *cter = 10; //error！
```
输出:
![在这里插入图片描述](https://img-blog.csdnimg.cn/ba8f57ee5e1f4a6eb5cad2ce6dc5f022.png)

### 6.面对函数声明时的应用
- 在一个函数声明式内, const 可以和函数返回值, 各参数, 函数自身(如果是成员函数)产生关联
- 示例:

```cpp
class Rational (...);
const Rational operator* (const Rational& lhs, const Rational& rhs);
```
第一个const表示该重载的操作符将会返回一个不可改变的对象,以避免如下的操作:

```cpp
Rational a, b, c;
(a * b) = c;	//在 a * b 的结果上调用 operator=
if(a * b = c)	//将 == 错写为 =
```
- 良好的用户自定义类型的特征是它们可以避免无端地与内置类型不兼容,即const可以预防很多没有意义的操作.

## 二、将const实施于成员函数
- 将一个class内部的成员函数设置为const类型说明该函数可作用于const对象，具体有如下两个目的：
1. const成员函数使得class接口更加清晰，将成员函数分为是否可以改动对象内容两类；
2. 使得 “操作const对象” 成为可能，这是编写高效代码的关键( *pass by reference-to-const* )
- C++的重载特性：两个成员函数如果只是常量性不同，可以被重载。
- 示例：

```cpp
//textBlock.h
class TextBlock{
public:	
	...
	const char& operator[] (std::size_t position) const //operator[] for const对象
	{ return text[position]; }
	char& operator[] (std::size_t position) //operator[] for non-const 对象
	{ return text[position]; } 

private:
	std::string text;
};
```

```cpp
//textBlock.cpp
	TextBlock tb("hello");	//non-const 对象
    const TextBlock ctb("world");	//const 对象
    std::cout<<tb[0];	//调用non-const operator[]
    std::cout<<ctb[0];	//调用const operator[]
    tb[0] = 'x';	//为non-const对象赋值
    ctb[0] = 'x';	//为const对象赋值 -> error！不可改动const对象
```
首先对operator[]左侧的const进行说明：此const代表函数的返回值为不可改变的char&，由此可见const 函数返回值不可作为被修改的左值
然后是operator[]右侧的const，这个const表示成员函数本身是const类型，关于const成员函数的定义有如下两个流派：
### 1.bitwise constness（physical constness）
从字面上来看，此定义中最小的const单位为bit（位），即成员函数不会改变对象内的任何一个bit，这也是C++对常量性（constness）的定义，因此==const成员函数不可改变对象内的任何non-static成员变量==。

然而，仅仅是不改变non-static成员变量并不意味着使用这种const成员函数是完全安全的，因为其可能会将data in class 暴露在类之外：
	

```cpp
class CTextBlock {
public:
	...
	char& operator[] (std::size_t position) const	//不恰当的bitwise声明
	{ return pText[position]; }
private:
	char* pText;
};
```
上例中CTextBlock类拥有一个private属性的字符指针pText，其指向存储在类内的一个字符串。public成员函数operator[] 虽然使用了pText，却没有改变指针本身，也没有改变指针所指的数据，看起来满足bitwise，并且可以通过编译，但致命的是其返回了一个指向数据内部的字符引用char&，导致如下结果：

```cpp
const CTextBlock cctb("hello");	//声明一个常量class对象
char* pc = &cctb[0];			//调用public operator[]获得一个指针，其指向cctb的数据
*pc = 'j';						//cctb现在有了“jello”的数据
```
从结果来看不恰当的bitwise声明间接改变了const对象中的non-const private数据，即欺骗了编译器而将类的内部数据暴露在外，这是及其危险的做法。
### 2.logical constness
这一派拥护者主张一个const成员函数可以修改它所处理对象内的某些bits，但只有在客户端侦测不出的情况才得如此：

```cpp
class CTextBlock {
public:
	...
	std::size_t length() const;
private:
	char* pText;
	std::size_t textLength;	//最近一次计算的文本区块长度
	bool lengthIsValid;		//判断目前的长度是否有效
};
std::size_t CTextBlock::length() const {
	if(!lengthIsValid){
		textLength = std::strlen(pText);	//错误！ 不可以修改non-const对象
		lengthIsValid = true;				//错误！ 同上
	}
	return textLength;
}

```
解决方法：使用mutable修饰成员变量，使其释放掉non-static的bitwise constness约束
```cpp
class CTextBlock {
public:
	...
	std::size_t length() const;
private:
	char* pText;
	mutable std::size_t textLength;	//最近一次计算的文本区块长度
	mutable bool lengthIsValid;		//判断目前的长度是否有效
};
std::size_t CTextBlock::length() const {
	if(!lengthIsValid){
		textLength = std::strlen(pText);	//可以执行
		lengthIsValid = true;				//可以执行
	}
	return textLength;
}

```
## 三、在const和non-const成员函数中避免重复
假设在CTextBlock和TextBlock的例子中为每个成员函数都加上执行边界检验、日志访问信息、数据完善性检验等多种复杂功能，则const和non-const operator[] 两个函数将会成为非常庞大的怪物，因此需要考虑代码的复用。

 - 做法：通过使用常量性转型来使用一个函数调用另外一个函数（casting away constness）
 

```cpp
class TextBlock{
public:	
	...
	const char& operator[] (std::size_t position) const{	//operator[] for const对象 
		...	//其他复杂操作
		...
		return text[position];
	}
	char& operator[] (std::size_t position){ 				//operator[] for non-const 对象
		return
			const_cast<char&>(
				static_cast<const TextBlock&>(*this)[position]
			);
	}
...
};
```

 - 上述代码意在通过non-const成员函数来调用其const兄弟，其中进行了两次转型（casting）操作：
1. 第一个static_cast将non-const成员函数的对象从TextBlock&转型为const TextBlock&，目的是让编译器自动调用const类型的operator[]
2. const类型的operator返回来的是const char&，所以需要第二次转型（const_char<char&>）将const char&转回为char&，以供non-const operator[] 的返回输出。
- 调用安全性声明：不要试图反向操作，即避免通过const成员函数调用non-const成员函数。
## 四、总结
- const可以运用于***指针及其所指对象、迭代器及reference指涉对象、函数参数和返回类型、local变量、成员函数*** 身上，const威力巨大，请尽可能使用它。
- **三个注意事项**：
 1. 将某些东西声明为const可帮助编译器侦测出错误用法。
 2. 编译器强制实施bitwise constness，但编写程序时应该使用“概念上的常量性”。
 3. 当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const版本可避免代码重复。
