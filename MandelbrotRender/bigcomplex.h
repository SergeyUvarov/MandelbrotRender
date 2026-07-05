#pragma once

#include "bigfloat.h"

class BigComplex
{
public:
	BigComplex();
	BigComplex(double, double);
	BigComplex(const std::string&, const std::string&);
	BigComplex(const BigFloat&, const BigFloat&);

	std::string to_str();

	BigComplex operator +(const BigComplex&) const;
	BigComplex operator -(const BigComplex&) const;
	BigComplex operator -() const;

	BigComplex& operator +=(const BigComplex&);
	BigComplex& operator -=(const BigComplex&);

	BigComplex operator *(const BigComplex&) const;
	BigComplex& operator *=(const BigComplex&);

	BigFloat getReal();
	BigFloat getImaginary();
	BigFloat norm2() const;
	
	void setPrecision(int64_t);

protected:
	BigFloat re;
	BigFloat im;
};