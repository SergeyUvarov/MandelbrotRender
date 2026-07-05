#include "bigcomplex.h"

BigComplex::BigComplex() : re(), im()
{

}

BigComplex::BigComplex(double r, double i) : re(r), im(i)
{

}

BigComplex::BigComplex(const std::string& r, const std::string& i) : re(r), im(i)
{

}

BigComplex::BigComplex(const BigFloat& r, const BigFloat& i) : re(r), im(i)
{

}

std::string BigComplex::to_str()
{
	return re.to_str() + (im >= BigFloat(0) ? "+" : "") + im.to_str() + "i";
}

BigComplex BigComplex::operator +(const BigComplex& z) const
{
	return BigComplex(re + z.re, im + z.im);
}

BigComplex BigComplex::operator -(const BigComplex& z) const
{
	return BigComplex(re - z.re, im - z.im);
}

BigComplex BigComplex::operator -() const
{
	return BigComplex(-re, -im);
}

BigComplex& BigComplex::operator +=(const BigComplex& z)
{
	re += z.re;
	im += z.im;
	return *this;
}

BigComplex& BigComplex::operator -=(const BigComplex& z)
{
	re -= z.re;
	im -= z.im;
	return *this;
}

BigComplex BigComplex::operator *(const BigComplex& z) const
{
	return BigComplex(re * z.re - im * z.im, re * z.im + z.re * im);
}

BigComplex& BigComplex::operator *=(const BigComplex& z)
{
	*this = *this * z;
	return *this;
}

BigFloat BigComplex::getReal()
{
	return re;
}

BigFloat BigComplex::getImaginary()
{
	return im;
}

BigFloat BigComplex::norm2() const
{
	return re*re + im*im;
}

void BigComplex::setPrecision(int64_t p)
{
	re.setPrecision(p);
	im.setPrecision(p);
}