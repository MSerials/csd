#pragma once
class Global
{
public:
	Global();
	~Global();
};

namespace exmath
{
	template<typename _T>
	void swap(_T& Left, _T& Right)
	{
		_T tmp = Left;
		Left = Right;
		Right = tmp;
	}
}