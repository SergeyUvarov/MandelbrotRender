#pragma once

#include <string>
#include <cstring>
#include <cmath>
#include <iostream>

class BigFloat
{
public:
	BigFloat();
	BigFloat(long double);
	BigFloat(std::string);

	bool is_zero() const;

	std::string to_str() const;
	std::string to_exp_str() const;
	double to_double() const;

	BigFloat operator + (const BigFloat&) const;
	BigFloat operator - (const BigFloat&) const;

	BigFloat operator - () const;

	BigFloat& operator += (const BigFloat&);
	BigFloat& operator -= (const BigFloat&);

	BigFloat operator * (const BigFloat&) const;

	BigFloat& operator *= (const BigFloat&);

	bool operator > (const BigFloat&) const;
	bool operator < (const BigFloat&) const;
	bool operator == (const BigFloat&) const;
	bool operator >= (const BigFloat&) const;
	bool operator <= (const BigFloat&) const;

	void setPrecision(int64_t);


protected:

	void smolMultiplex(BigFloat& res, const BigFloat& mult) const;
	void karatsubaMultiplex(BigFloat& res, const BigFloat& mult) const;


	void cutZeros();
	void limitPrecision();

	std::string mantissa;
	int64_t exponent;
	int64_t precision;
	bool negative;
};