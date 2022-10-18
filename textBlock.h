#include<string>
#include<iostream>
class TextBlock{
public:	
    TextBlock(std::string _text)
        : text(_text){}
    TextBlock(){}
	const char& operator[] (std::size_t position) const //operator[] for const对象
	{ return text[position]; }
	char& operator[] (std::size_t position) //operator[] for non-const 对象
	{ return text[position]; } 

	void printXYZ(){
		std::cout<<x<<" "<<y<<" "<<z<<std::endl;
	}
private:
	std::string text;
	int x;
	int y;
	int z;
};