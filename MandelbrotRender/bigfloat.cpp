#include "bigfloat.h"

BigFloat::BigFloat(): mantissa("0"), exponent(0), negative(false), precision(0)
{

}

BigFloat::BigFloat(std::string number): BigFloat()
{
	mantissa.clear();
	size_t ePos = number.find('e');
	std::string clearNum = number.substr(0, ePos);

	if (clearNum[0] == '-')
		negative = true;

	char symb;
	bool significantZeros = true;
	int64_t i;
	for (i = clearNum.length() - 1; i >= 0; i--)
	{
		symb = clearNum[i];
		if (std::isdigit(symb))
		{
			mantissa.insert(mantissa.begin(), symb);
		}
		else if (symb == '.')
		{	
			exponent -= static_cast<int64_t>(clearNum.length() - 1) - static_cast<int64_t>(i);
		}
	}

	cutZeros();

	if (ePos == std::string::npos)
		return;

	clearNum = number.substr(ePos + 1, number.length());
	exponent += std::atoll(clearNum.c_str());	
}

BigFloat::BigFloat(long double number) : BigFloat(std::to_string(number))
{

}

bool BigFloat::is_zero() const
{
	for (auto symb : mantissa)
	{
		if (symb != '0')
			return false;
	}
	return true;
}

std::string BigFloat::to_str() const
{
	std::string outStr = negative ? "-" : "";

	if (exponent == 0)
	{
		outStr += mantissa;
	}
	else if(exponent < 0)
	{
		if (mantissa.length() > abs(exponent))
		{
			for (int64_t i = 0; i < mantissa.length(); i++)
			{
				if (i == static_cast<int64_t>(mantissa.length())+exponent) outStr += ".";
				outStr += mantissa[i];
			}
		}
		else
		{
			outStr += "0." + std::string(abs(static_cast<int64_t>(mantissa.length()) + exponent), '0') + mantissa;
		}
	}
	else
	{
		outStr += mantissa + std::string(static_cast<int64_t>(exponent), '0');
	}
	return outStr;
}

std::string BigFloat::to_exp_str() const
{
	std::string outStr = negative ? "-" : "";
	int32_t diff = 6 - mantissa.length();

	if (diff < 0)
	{
		for (size_t i = 0; i < 6; i++)
		{
			outStr += mantissa[i];

			if (i == 0) outStr += ".";
		}
	}
	else
	{
		for (int32_t i = 0; i < mantissa.length(); i++)
		{
			outStr += mantissa[i];

			if (i == 0) outStr += ".";
		}
		outStr += std::string(diff, '0');
	}

	outStr += "E" + std::to_string(exponent + int64_t(mantissa.length() - 1));
	return outStr;
}

double BigFloat::to_double() const
{
	return std::atof(this->to_str().c_str());
}

BigFloat BigFloat::operator + (const BigFloat& addend) const
{
	if (is_zero())
		return addend;
	else if (addend.is_zero())
		return *this;

	BigFloat result;

	result.exponent = std::min(exponent, addend.exponent);

	int64_t expDiff = exponent - addend.exponent;
	std::string mantissaA = mantissa; 
	std::string mantissaB = addend.mantissa; 
	
	bool differentSigns = negative ^ addend.negative;

	if (expDiff > 0)
		mantissaA += std::string(expDiff, '0');
	else
		mantissaB += std::string(-expDiff, '0');

	if (mantissaA.length() < mantissaB.length()) {
		result.negative = addend.negative;
		std::swap(mantissaA, mantissaB);
	}
	else {
		result.negative = negative;
	}

	if (differentSigns)
	{
		if (mantissaA.length() == mantissaB.length())
		{
			for (int32_t i = 0; i < mantissaA.length(); i++)
			{
				if (mantissaA[i] < mantissaB[i])
				{
					result.negative = addend.negative;
					std::swap(mantissaA, mantissaB);
					break;
				}
				else if (mantissaA[i] > mantissaB[i])
				{
					result.negative = negative;
					break;
				}

				if (i == mantissaA.length() - 1)
					return BigFloat("0");
			}
		}
	
		int8_t buff, mem = 0;
		for (int64_t i = 0; i < mantissaA.length(); ++i)
		{
			if (i < mantissaB.length())
			{
				buff = (mantissaA[mantissaA.length() - 1 - i] - '0') - (mantissaB[mantissaB.length() - 1 - i] - '0') + mem;
			}
			else
			{
				buff = (mantissaA[mantissaA.length() - 1 - i] - '0') + mem;
			}
			if (buff < 0)
			{
				mem = -1;
				mantissaA[mantissaA.length() - 1 - i] = '0' + (10 + buff % 10) % 10;
			}
			else
			{
				mem = buff / 10;
				mantissaA[mantissaA.length() - 1 - i] = '0' + buff % 10;
			}
		}
	}
	else
	{
		int8_t buff, mem = 0;
		for (int64_t i = 0; i < mantissaA.length(); ++i)
		{
			if (i < mantissaB.length())
			{
				buff = (mantissaA[mantissaA.length() - 1 - i] - '0') + (mantissaB[mantissaB.length() - 1 - i] - '0') + mem;
			}
			else
			{
				buff = (mantissaA[mantissaA.length() - 1 - i] - '0') + mem;
			}
			mem = buff / 10;
			mantissaA[mantissaA.length() - 1 - i] = '0' + buff % 10;
		}
		if (mem)
		{
			mantissaA.insert(0, std::to_string(mem));
		}
	}

	result.mantissa = mantissaA;

	result.cutZeros();
	result.setPrecision(precision);

	return result;
}

BigFloat BigFloat::operator - (const BigFloat& sub) const
{
	return *this + (-sub);
}

BigFloat BigFloat::operator - () const
{
	BigFloat neg = *this;
	neg.negative = !neg.negative;
	return neg;
}

BigFloat& BigFloat::operator += (const BigFloat& addend)
{
	*this = *this + addend;
	return *this;
}

BigFloat& BigFloat::operator -= (const BigFloat& sub)
{
	*this = *this - sub;
	return *this;
}

void BigFloat::smolMultiplex(BigFloat& res, const BigFloat& mult) const
{
	std::string strRes = "0";

	std::string num1 = mantissa;
	std::string num2 = mult.mantissa;

	// num2 - число с наибольшим количеством знаков
	if (num1.length() > num2.length())
		std::swap(num1, num2);

	// Подфункция умножения числа num2 на цифру
	auto submultiply = [&](int8_t digit)
		{
			std::string res = num2;
			int8_t buff, mem = 0;
			for (int64_t i = res.length() - 1; i >= 0; --i)
			{
				buff = (res[i] - '0') * digit + mem;
				mem = buff / 10;
				res[i] = '0' + buff % 10;
			}
			if (mem)
			{
				res = static_cast<char>('0' + mem) + res;
			}
			return res;
		};

	// Подфункция прибавления строкового числа с результатом
	auto resultAdd = [&](std::string& added)
		{
			std::string& s1 = strRes;
			std::string& s2 = added;

			if (s1.length() < s2.length())
				std::swap(s1, s2);

			int8_t buff, mem = 0;
			for (int64_t i = 0; i < s1.length(); ++i)
			{
				if (i < s2.length())
				{
					buff = (s1[s1.length() - 1 - i] - '0') + (s2[s2.length() - 1 - i] - '0') + mem;
				}
				else
				{
					buff = (s1[s1.length() - 1 - i] - '0') + mem;
				}
				mem = buff / 10;
				s1[s1.length() - 1 - i] = '0' + buff % 10;
			}
			if (mem)
			{
				s1 = static_cast<char>('0' + mem) + s1;
			}
			strRes = s1;
		};

	// Запоминание посчитаных умножений числа num2 на одну из 10 цифр
	struct MultMap {
		bool isCalculate = false;
		std::string val;
	};
	MultMap multMap[10];

	multMap[1].val = num2;
	multMap[1].isCalculate = true;


	std::string strBuf;
	for (int64_t i = num1.length() - 1; i >= 0; i--)
	{
		int8_t digit = num1[i] - '0';
		if (digit == 0) continue;
		if (multMap[digit].isCalculate)
		{
			strBuf = multMap[digit].val + std::string(num1.length() - 1 - i, '0');
			resultAdd(strBuf);
		}
		else
		{
			multMap[digit].val = submultiply(digit);
			multMap[digit].isCalculate = true;
			strBuf = multMap[digit].val + std::string(num1.length() - 1 - i, '0');
			resultAdd(strBuf);
		}
	}

	res.mantissa = strRes;
	res.negative = this->negative ^ mult.negative;
	res.exponent = this->exponent + mult.exponent;

	res.cutZeros();
	res.setPrecision(precision);
}

void BigFloat::karatsubaMultiplex(BigFloat& result, const BigFloat& mult) const
{
	std::string a2 = mantissa;
	std::string b2 = mult.mantissa;

	int32_t diff = a2.length() - b2.length();

	if (diff < 0)
		a2.insert(0, std::string(-diff, '0'));
	else if(diff > 0)
		b2.insert(0, std::string(diff, '0'));

	if (a2.length() % 2 != 0)
	{
		a2.insert(0, "0");
		b2.insert(0, "0");
	}

	int32_t k = a2.length() / 2;
	std::string a1 = a2.substr(k, 2 * k);
	std::string b1 = b2.substr(k, 2 * k);
	a2.resize(k);
	b2.resize(k);

	auto adding = [&](std::string& res, std::string& added)
		{
			std::string& s1 = res;
			std::string& s2 = added;

			if (s1.length() < s2.length())
				std::swap(s1, s2);

			int8_t buff, mem = 0;
			for (int64_t i = 0; i < s1.length(); ++i)
			{
				if (i < s2.length())
				{
					buff = (s1[s1.length() - 1 - i] - '0') + (s2[s2.length() - 1 - i] - '0') + mem;
				}
				else
				{
					buff = (s1[s1.length() - 1 - i] - '0') + mem;
				}
				mem = buff / 10;
				s1[s1.length() - 1 - i] = '0' + buff % 10;
			}
			if (mem)
				s1.insert(0, std::to_string(mem));
		};

	auto subing = [&](std::string& res, std::string& sub)
		{
			int8_t buff, mem = 0;
			for (int64_t i = 0; i < res.length(); ++i)
			{
				if (i < sub.length())
				{
					buff = (res[res.length() - 1 - i] - '0') - (sub[sub.length() - 1 - i] - '0') + mem;
				}
				else
				{
					buff = (res[res.length() - 1 - i] - '0') + mem;
				}
				if (buff < 0)
				{
					mem = -1;
					res[res.length() - 1 - i] = '0' + (10+buff % 10) % 10;
				}
				else
				{
					mem = buff / 10;
					res[res.length() - 1 - i] = '0' + buff % 10;
				}
			}
		};

	auto submultiply = [&](std::string& res, const std::string& val, int8_t digit)
		{
			res = val;
			int8_t buff, mem = 0;
			for (int64_t i = val.length() - 1; i >= 0; --i)
			{
				buff = (val[i] - '0') * digit + mem;
				mem = buff / 10;
				res[i] = '0' + buff % 10;
			}
			if (mem)
				res.insert(0, std::to_string(mem));
		};

	auto multiply = [&](std::string& res, const std::string& val1, const std::string& val2)
		{
			struct MultMap {
				bool isCalculate = false;
				std::string val;
			};
			MultMap multMap[10];

			multMap[1].val = val1;
			multMap[1].isCalculate = true;

			res = "0";
			std::string strBuff;
			for (int64_t i = val2.length() - 1; i >= 0; i--)
			{
				int8_t digit = val2[i] - '0';
				if (digit == 0) continue;
				if (!multMap[digit].isCalculate)
					submultiply(multMap[digit].val, val1, digit);

				strBuff = multMap[digit].val + std::string(val2.length() - 1 - i, '0');
				adding(res, strBuff);
			}
		};

	std::string p1;
	multiply(p1, a1, b1);
	std::string p2;
	multiply(p2, a2, b2);

	std::string t;
	adding(a1, a2);
	adding(b1, b2);
	multiply(t, a1, b1);
	subing(t, p1);
	subing(t, p2);

	t += std::string(k, '0');
	p2 += std::string(2*k, '0');

	adding(p1, t);
	adding(p1, p2);

	result.mantissa = p1;
	result.negative = this->negative ^ mult.negative;
	result.exponent = this->exponent + mult.exponent;

	result.cutZeros();
	result.setPrecision(precision);
}

BigFloat BigFloat::operator * (const BigFloat& mult) const
{
	BigFloat result;

	if (std::max(mantissa.length(), mult.mantissa.length()) < 130) // 130
	{
		smolMultiplex(result, mult);
	}
	else
	{
		karatsubaMultiplex(result, mult);
	}
	return result;
}

BigFloat& BigFloat::operator *= (const BigFloat& mult)
{
	*this = *this * mult;
	return *this;
}

bool BigFloat::operator > (const BigFloat& num2) const
{
	if (!this->negative && num2.negative)
		return true;

	if (*this == num2)
		return false;

	BigFloat diff = *this - num2;
	if (diff.negative)
		return false;

	return true;
}

bool BigFloat::operator < (const BigFloat& num2) const
{
	if (this->negative && !num2.negative)
		return true;

	if (*this == num2)
		return false;

	BigFloat diff = *this - num2;
	if (diff.negative)
		return true;

	return false;
}

bool BigFloat::operator == (const BigFloat& num2) const
{
	return this->mantissa == num2.mantissa && this->exponent == num2.exponent && this->negative == num2.negative;
}

bool BigFloat::operator >= (const BigFloat& num2) const
{
	return (*this == num2) || (*this > num2);
}

bool BigFloat::operator <= (const BigFloat& num2) const
{
	return (*this == num2) || (*this < num2);
}

void BigFloat::setPrecision(int64_t p)
{
	if (p >= 0)
	{
		precision = p;
		limitPrecision();
	}
}

void BigFloat::cutZeros()
{
	int64_t zerosCount = 0;
	for (size_t i = 0; i < mantissa.length() - 1; ++i)
	{
		if (mantissa[i] == '0')
			zerosCount++;
		else
			break;
	}
	mantissa.erase(mantissa.cbegin(), mantissa.cbegin() + zerosCount);

	zerosCount = 0;
	for (size_t i = mantissa.length() - 1; i > 0; --i)
	{
		if (mantissa[i] == '0')
			zerosCount++;
		else
			break;
	}
	mantissa.erase(mantissa.length() - zerosCount, mantissa.length());
	exponent += zerosCount;

	if (is_zero())
	{
		mantissa = "0";
		exponent = 0;
	}
}

void BigFloat::limitPrecision()
{
	if (precision != 0 && exponent < -precision)
	{
		int64_t diff = -precision - exponent;
		if (diff <= mantissa.length()) 
		{
			mantissa.erase(mantissa.length() - diff, mantissa.length());
			exponent = -precision;
		}
		else
		{
			mantissa = "0";
			exponent = 0;
		}
	}
}