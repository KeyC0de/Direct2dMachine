#pragma once

#include <cmath>
#include <cstdint>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include "assertions_console.h"


namespace math
{

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;


template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
bool approximatelyEqual( T x, T y )
{
	return std::fabs( x - y ) <= std::numeric_limits<T>::epsilon();
}

// fast multiplication by 10
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T mult10( T num )
{
	return ( num << 3 ) + ( num << 1 );
}

// fast division by 10
//https://stackoverflow.com/a/5558614/4743275
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T div10( T dividend )
{
	long long int invDivisor = 0x1999999A;
	return static_cast<T>( ( invDivisor * dividend ) >> 32 );
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T div1000( T dividend )
{
	return div10( div10( div10( dividend ) ) );
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T absValueOf( T val )
{
	return ( val >= 0 ) ? val : -val;
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
void quadratic( T a, T b, T c )
{
	T diakr = b * b - 4 * a * c;

	if ( diakr < 0 )
	{
		std::cout << "Imaginary roots.\n";
	}
	else if ( diakr == 0 )
	{
		std::cout << "There is only one root: "
			<< -b / (2.0 * a)
			<< "\n";
	}
	else
		std::cout << "1st root: " 
					<< ( -b + squareRoot( diakr ) ) / ( 2 * a )
					<< "\t 2nd root: " 
					<< ( -b - squareRoot( diakr ) ) / ( 2 * a )
					<< "\n";
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr T square( const T& x )
{
	return x * x;
}

template<typename T>
std::string toHexString( T i )
{
	std::ostringstream oss;
	oss << "0x"
		<< std::setfill ('0')
		<< std::setw( sizeof( T ) * 2 )
		<< std::hex
		<< i;
	return oss.str();
}

//===================================================
//	\function	wrapAngle
//	\brief wrap angle (in radians) in the range -pi to pi
//	\date 2020/07/15 5:10
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr T wrapAngle( T angle )
{
	const T val = fmod( angle, (T)2.0 * (T)PI_D );
	return ( val > (T)PI_D )
		? ( val - (T)2.0 * (T)PI_D )		// if > 2pi subtract 2pi from it
		: val;
}

//===================================================
//	\function	toRadians
//	\brief convert angle in degrees to radians
//	\date 2020/07/15 5:13
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr T toRadians( T deg )
{
	return ( deg * PI ) / (T)180.0;
}

//===================================================
//	\function	interpolate
//	\brief linear interpolation from src to dst value using alpha provided
//			interpolates any two Types that support the operations +, -, *
//	\date 2020/08/02 17:56
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr T interpolate( const T& src,
	const T& dst,
	float alpha )
{
	return src + alpha * ( dst - src );
}

template<typename T>
constexpr T gauss( T x,
	T sigma ) noexcept
{
	const auto ss = square( sigma );
	return ((T)1.0 / sqrt( (T)2.0 * (T)PI_D * ss )) * exp( -square( x ) / ((T)2.0 * ss) );
}

// calculate x^n, n > 1
// doesn't take into account negative powers for now
template<typename T, typename J, typename = std::enable_if_t<std::is_arithmetic_v<T>>,
	typename = std::enable_if_t<std::is_arithmetic_v<J>>>
constexpr T powerOf( T x,
	J n )
{
	if ( n == 0 )
	{
		return x = 1;
	}
	if ( n < 0 )
	{
		return -1;
	}
	T xo = x;
	while ( --n > 0 )
	{
		x *= xo;
	}
	return x;
}


// produce random floating point numbers
float frand() noexcept;
// m = divident, n = divisor, r = remainder
// O(log_n(m))
constexpr int gcd( int m, int n ) noexcept;
constexpr int factorialOf( int n ) noexcept;
// trial division method, returns 1 for prime, 0 for not prime
int isPrime( int number ) noexcept;
// Function that calculates the square root of a number
constexpr float squareRoot( float x ) noexcept;
constexpr bool isPowerOfTwo( const std::size_t value ) noexcept;
float cosine( float x ) noexcept;
float sine( float x ) noexcept;

//  check whether the address is aligned to `alignment` boundary
bool isAligned( const volatile void *p, std::size_t alignment ) noexcept;
bool isAligned( std::uintptr_t pi, std::size_t alignment ) noexcept;

constexpr int is4ByteAligned( intptr_t *addr );

constexpr int linearSearch( int* A, int n, int x );
constexpr int binarySearch( int* A, int low, int high, int x );

int toDecimal( int hex );
int toHex( int dec );

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
int signum( T val )
{
	return val > T(0) ?
		1 :
		-1;
}

// bitwise ops
// Prints given datatype to binary
void printDec2Bin( size_t const size, void const* const ptr );
bool oppositeSigns(int x, int y);
// print binary conversion of supplied decimal number
void dec2bin( int num );
// finds MSB index in decimal number (counting from 0)
int msbIndexOfDec( int num );
//  count number of Ones in a given number
int countOnes( int num );
// checks whether a number is a power of 2
int isPowerOf2( int num );
// Swap the bits at position i and j of given number iff they are different
int swapBits( int num, int i, int j );
int setNthBit( int num, unsigned n );
int clearNthBit( int num, unsigned n );
int toggleNthBit( int num, unsigned n );
// check nth bit's state 0 or 1
int returnNthBit( int num, unsigned n );
// change nth bit to bitVal
int changeNthBit( int num, unsigned n, bool bitVal );


template<int b>
class ByteSet
{
	enum
	{
		bit7 = ( b & 0x80 ) ? 1 : 0,
		bit6 = ( b & 0x40 ) ? 1 : 0,
		bit5 = ( b & 0x20 ) ? 1 : 0,
		bit4 = ( b & 0x10 ) ? 1 : 0,
		bit3 = ( b & 0x08 ) ? 1 : 0,
		bit2 = ( b & 0x04 ) ? 1 : 0,
		bit1 = ( b & 0x02 ) ? 1 : 0,
		bit0 = ( b & 0x01 ) ? 1 : 0
	};
public:
	enum
	{
		nbits = bit0 + bit1 + bit2 + bit3 + bit4 + bit5 + bit6 + bit7
	};
};


}// namespace math