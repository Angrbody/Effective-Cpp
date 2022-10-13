## Effective C++ 条款02：
## 尽量以`const,enum,inline`替换 `#define`
### 1.宏（#define）
 - 宏：`#define`命令是C语言中的一个宏定义命令，它用来将一个标识符定义为一个字符串，该标识符被称为宏名，被定义的字符串称为替换文本。
 
 - 示例代码中`#define plus(x) x+x`表示用X+X这个字符串代替`plus(x)`
```cpp
#include<iostream>
using namespace std;
#define plus(x) x+x
int main(){
    int res = plus(2*2);
    cout<<res<<endl;
    system("pause");
    return 0;
}
```
输出：
![在这里插入图片描述](https://img-blog.csdnimg.cn/7a3f949f78294cfeac58430d5135908c.png)
### 2.宏的特点之一：替换性
- 宏命令在预处理过程中只进行==纯粹的替换和展开==，没有计算功能，下面的例子很好的说明了这一点
 

```cpp
#include<iostream>
using namespace std;
#define square(x) x*x
int main(){
    int res = square(2+2);
    cout<<res<<endl;
    system("pause");
    return 0;
}
```
输出：
![在这里插入图片描述](https://img-blog.csdnimg.cn/9347bc7abcb946e796bc66212cfff601.png)
- 上述例子中宏定义了square(x)替换为x*x，表面上看是一个类似求平方的函数，但编译器实际处理的指令为：2+2×2+2 = 8，以此可见宏定义的“替换”性质。

- 正确的求平方宏定义：

```cpp
#include<iostream>
using namespace std;
#define plus(x) x+x
#define square(x) (x)*(x)
int main(){
    int res = square(2+2);
    cout<<res<<endl;
    system("pause");
    return 0;
}
```
输出：
![在这里插入图片描述](https://img-blog.csdnimg.cn/a2cd5cd4cfc84f16937ae6b8ba456124.png)
### 3.宏的特点之二：宏替换的记号名称无法被编译器识别
*#define 不被视为语言的一部分*
例：`#define ASPECT_RATIO 1.653`

 - 因为宏作用与预处理器工作期间，所以当编译器开始处理源码时记号名称ASPECT_RATIO根本没有进入记号表（symbol table），编译器只看到了被替换的1.653，而无法追踪到替换前的记号名称。
- 解决之道：利用常量定义替换上述的宏（#define）：

```cpp
const double AspectRatio 1.653;
```
- 解释：将AspectRatio定义为常量的做法使得编译器能够识别并且在记号表（symbol table）中建立映射，此外在某些情况下（比如本例中的浮点常数），使用常量比使用宏使用更小量的*码*（避免复制了多个目标码）
- 两种以常量`const`替换`#define`的特殊情况：
1. 定义常量指针：常量定义式通常被放在头文件内（以便被不同的源码含入），因此有必要将指针声明为const，例如若要在头文件内定义一个常量的char*-based字符串，必须要const两次：

```cpp
const char* const authorName = "Scott Meyers";
```
代码中第一个const说明指向对象的内容不可改变，第二个const说明该指针指向的对象是固定的。
 更好的做法（至于为什么见条款03）：使用string指针：
 

```cpp
const std::string authorName = "Scott Meyers";
```
2. class的专属常量：如果你需要为你的class设计一个只在class内部有效的常量，则必须注意声明和定义的区别。

```cpp
class GamePlayer {
private:
	static const int NumTurns = 5; //常量声明式
	int scores[NumTurns];		   //使用该常量
	...
};
...
const int GamePlayer::NumTurns;    //NumTurns的定义式，位于源码中
```
上述示例中，类的内部对常量NumTurns进行了声明，但在源码中才为其创建了实例，注意到源码定义式中没有设置初值，因为在类中已经设置过了。
如果编译器不允许in-class设置初值，同样可以这样写：

```cpp
class GamePlayer {
private:
	static const int NumTurns;     //常量声明式（未设置初值）
	//int scores[NumTurns];		   //使用该常量
	...
};
...
const int GamePlayer::NumTurns = 5; 	//NumTurns的定义式，位于源码中
```
注意：#define不仅不能够用来定义class的专属常量，也不能够提供任何的封装性（宏是面向全局的）
### 4.更像#define的enum hack：解决#define的困境

  注意到在上述class专属常量的定义中，如果编译器不允许in-class设初值时，则无法在class内使用该常量（NumTurns），而#define又无法进入class的内部，此时可以使用枚举类型enum来处理：
  

 - 枚举数据类型是一种由程序员定义的数据类型，其合法值是与它们关联的一组命名整数常量。
 例如：`enum student{xiaoming, lihong, zhaohua};`其中stuent是类型名称，三个名字是类型的所有合法值。
 - ==enum hack==：把需要使用的常量定义为一个枚举常量：
 

```cpp
class GamePlayer {
private:
	enum { NumTurns = 5};     	   //enum hack:令NumTurns成为5的一个记号名称
	int scores[NumTurns];		   //现在可以使用该常量
	...
};
```

 - 如此一来成功的通过非in-class设初值的方式对常量成员进行了初始化
 - 小节：如果不想让别人获得一个pointer或reference指向你的某个整数常量，可以使用enum来进行约束，并且enum只声明，和#define一样*不会导致非必要的内存分配。*
### 5.宏的函数误用
 - 示例：

```cpp
#define CALL_WITH_MAX(a,b) f((a) > (b) ? (a) : (b)）
```
上述#define将`CALL_WITH_MAX(a,b)` 替换为函数`f((a) > (b))? (a) : (b)）`注意到我们已经改正了2.中的错误将每一个参数都加上了小括号，但是仍然会出现以下诡异的事情：

```cpp
int a = 5, b = 0;
CALL_WITH_MAX(++a, b);		//a被累加两次
CALL_WITH_MAX(++a, b+10);	//a被累加一次
```
因为编译器实际执行的为：
++a ，a > b → ++a；
++a ，a < b → b；

 - 解决方案：使用inline：
 

```cpp
template<typename T>
inline void callWithMax(const T& a, const T& b){
	f(a > b ? a : b);
}
```
使用template产出一整群函数，每个函数接受两个同型对象，并以其中较大者调用函数f

 - inline的优势：
1. 无需注意2.中的括号问题
2. 参数不会被核算多次
3. 遵守作用域和访问规则
### 6.总结
使用`const、enum、inline`可以减少对预处理器的需求，但是仍然无法完全消除（#include是必要的），在非必要场景下，请多多使用`const、enum、inline`
