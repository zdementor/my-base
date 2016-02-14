//|-------------------------------------------------------------------------
//| File:        string.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef stringHPP
#define stringHPP
//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

// define the wchar_t type if not already built in.
#ifdef _MSC_VER             // microsoft compiler
#ifndef _WCHAR_T_DEFINED    // microsoft compiler
//! A 16 bit wide character type.
//! 
//! Defines the wchar_t-type.
//! In VS6, its not possible to tell
//! the standard compiler to treat wchar_t as a built-in type, and
//! sometimes we just don't want to include the huge stdlib.h or wchar.h,
//!  so we'll use this.
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif // wchar is not defined  
#endif // microsoft compiler

template <class T> class string;

//----------------------------------------------------------------------------

template <class T>
class const_string
{
	friend string<c8>;
	friend string<wchar_t>;
public:

	const_string(const T* c, unsigned len)
		: array(c), used(len) {}

	const_string(const T* c)
		: array(c), used(core::strlen(c)+1) {}

	const T& operator [](s32 index) const
	{ return array[index]; }

	s32 size() const
    { return used-1; }

    const T* c_str() const
    { return array; }

	template <class B>
    bool operator == (B *other_arr) const
    {   
        return (*this) == core::const_string<B>(other_arr);
    }

	template <class B>
    bool operator == (const B *other_arr) const
    {   
        return (*this) == core::const_string<B>(other_arr);
    }

	template <class B>
    bool operator == (const B &other) const
    {   
		if (used != other.used)
			return false;
		for(s32 i = 0; array[i]; ++i)
            if (array[i] != other.array[i])
                return false;
        return true;
    }

	//! Equals not operator
	template <class B>
    bool operator != (const B &other) const
    {
		return !(*this == other);
    }

private:

	const T	*array;
	u32		used;
};

//----------------------------------------------------------------------------

typedef const_string<c8>		const_stringc;
typedef const_string<wchar_t>	const_stringw;

//----------------------------------------------------------------------------

//! string class with some useful features.
template <class T>
class string
{
	friend const_string<c8>;
	friend const_string<wchar_t>;
public:

    //------------------------------------------------------------

    string() : allocated(1), used(1), array(0)
    {  
		array = new T[1];
        array[0] = 0x0;
    }

    //------------------------------------------------------------

    string(const string<T>& other) : allocated(0), used(0), array(0)
    {   
		*this = other;
    }

    //------------------------------------------------------------

    string(const const_string<T>& other) : allocated(0), used(0), array(0)
    {   
		*this = other;
    }

	//------------------------------------------------------------

    template <class B>
    string(const B* c, s32 lenght) : allocated(0), used(0), array(0)
    {   
		if (!c)
            return;
        allocated = used = lenght+1;
        array = new T[used];

        for (s32 l = 0; l<lenght; ++l)
            array[l] = (T)c[l];

        array[lenght] = 0;
    }

    //------------------------------------------------------------

    //! Constructor for unicode and ascii strings
    template <class B>
    string(const B* c) : allocated(0), used(0), array(0)
    {  
		*this = c;
    }

    //------------------------------------------------------------

    ~string()
    {  
		delete [] array;
    }

	//------------------------------------------------------------

	string<T>& set(const string<T>& other)
    {
		*this = other;
		return *this;
	}

    template <class B>
    string<T>& set(const B* c)
    {
		*this = c;
		return *this;
	}

    string<T>& set(const T* c, u32 sz)
    {
		if ((s32)sz <= used)
			memcpy(array, c, sz * sizeof(T));
		return *this;
	}

    //------------------------------------------------------------

    //! Assignment operator
    string<T>& operator = (const string<T> &other)
    {   
		if (this == &other)
            return *this;
		delete [] array;
        allocated = used = other.size() + 1;
        array = new T[used];
		set(other.c_str(), used);
        return *this;
    }

    string<T>& operator = (const const_string<T> &other)
    {   
		delete [] array;
        allocated = used = other.size() + 1;
        array = new T[used];
		set(other.c_str(), used);
        return *this;
    }

    //------------------------------------------------------------

    //! Assignment operator for strings, ascii and unicode
    template <class B>
    string<T>& operator = (const B* c)
    {

		if (!c)
		{   
			if (!array)
			{   
				array = new T[1];
				allocated = 1;
			}
			used = 1;
			array[0] = 0x0;
			return *this;
		}

		if ((void*)c == (void*)array)
			return *this;

		s32 len = 0;
		const B* p = c;
		while(*p)
		{   
			++len;
			++p;
		}

		T *oldArray = NULL;

		if (len >= allocated)
		{
			// we'll take the old string for a while, because the new string could be
			// a part of the current string.
			oldArray = array;

			allocated = used = len + 1;
			array = new T[used];
		}
		else
			used = len + 1;

		// use such implementation instead of memcpy to be able to copy wchar_t->char and char->wchar_t
		for (s32 l = 0; l <= len; ++l)
			array[l] = (T)c[l];

		if (oldArray)
			delete [] oldArray;

		return *this;
    }

    //------------------------------------------------------------

    //! Add operator
    string<T>& operator+(const string<T>& other)
    {   
		--used;

        s32 len = (s32)other.size() + 1;

        if (used + len > allocated)
            reallocate((s32)used + len);

        const T* p = other.c_str();
        for (s32 i=0; i<len; ++i, ++p)
            array[used+i] = *p;

        used = used+len;

        return *this;
    }

    //------------------------------------------------------------

    //! Add operator for strings, ascii and unicode
    template <class B>
    string<T>& operator+(const B* c)
    {   
		--used;

        s32 len = 0;
        const B* p = c;
        while(*p)
        {
            ++len;
            ++p;
        }

        ++len;

        if (used + len > allocated)
            reallocate((s32)used + (s32)len);

        for (s32 l = 0; l<len; ++l)
            array[l+used] = (T)c[l];

        used = used + len;

        return *this;
    }

    //------------------------------------------------------------

    //! Direct access operator

	T& operator [](s32 index)
    {   
		return array[index];
    }

    const T& operator [](s32 index) const
    {   
		return array[index];
    }

    //------------------------------------------------------------
    //! Comparison operator

	template <class B>
    bool operator == (B *other_arr) const
    {   
        return (*this) == core::const_string<B>(other_arr);
    }

	template <class B>
    bool operator == (const B *other_arr) const
    {   
        return (*this) == core::const_string<B>(other_arr);
    }

	template <class B>
    bool operator == (const B &other) const
    {   
		if (used != other.used)
			return false;
		for(s32 i = 0; array[i]; ++i)
            if (array[i] != other.array[i])
                return false;
        return true;
    }

    //------------------------------------------------------------
    //! Is smaller operator

	template <class B>
    bool operator < (B *other_arr) const
    {   
        return (*this) < core::const_string<B>(other_arr);
    }

	template <class B>
    bool operator < (const B *other_arr) const
    {   
        return (*this) < core::const_string<B>(other_arr);
    }

	template <class B>
    bool operator < (const B &other) const
    {   
		for(s32 i = 0; array[i] && other.array[i]; ++i)
            if (array[i] != other.array[i])
                return (array[i] < other.array[i]);
		return used < other.used;
    }

    //------------------------------------------------------------

    //! Equals not operator
	template <class B>
    bool operator != (const B &other) const
    {
		return !(*this == other);
    }

    //------------------------------------------------------------

    //! Returns length of string
    //! \return Returns length of the string in characters.
    s32 size() const
    {   
		return used-1;
    }

    //------------------------------------------------------------

    //! Returns character string
    //! \return Returns pointer to C-style zero terminated string.
    const T* c_str() const
    {   
		return array;
    }

    //------------------------------------------------------------

    //! Makes the string lower case.
    void make_lower()
    {   
		const T A = (T)'A';
        const T Z = (T)'Z';
        const T diff = (T)'a' - A;

        for (s32 i=0; i<used; ++i)
        {   if (array[i]>=A && array[i]<=Z)
                array[i] += diff;
        }
    }

    //------------------------------------------------------------

    //! Makes the string upper case.
    void make_upper()
    {
        const T a = (T)'a';
        const T z = (T)'z';
        const T diff = (T)'A' - a;

        for (s32 i=0; i<used; ++i)
        {
            if (array[i]>=a && array[i]<=z)
                array[i] += diff;
        }
    }

    //------------------------------------------------------------

    //! Compares the string ignoring case.
    //! \param other: Other string to compare.
    //! \return Returns true if the string are equal ignoring case.
    bool equals_ignore_case(const string<T>& other) const
    {   
		for(s32 i1=0, i2=0; array[i1] && other[i2]; ++i1, ++i2)
		{
            if (toLower(array[i1]) != toLower(other[i2]))
                return false;
		}

        return used == other.used;
    }

	//------------------------------------------------------------

	bool set_end_of_line(u32 idx)
	{
		if (idx < (u32)used)
		{
			array[idx] = 0;
			used = idx + 1;
			return true;
		}
		return false;
	}

    //------------------------------------------------------------

    //! Appends a character to this string
    //! \param character: Character to append.
    void append(T character)
    {   
		if (used + 1 > allocated)
            reallocate((s32)used + 1);
        used += 1;
        array[used-2] = character;
        array[used-1] = 0;
    }

    //------------------------------------------------------------

    //! Appends a string to this string
    //! \param other: String to append.
    void append(const string<T>& other)
    {   
		--used;

        s32 len = other.size();

        if (used + len + 1 > allocated)
            reallocate((s32)used + (s32)len + 1);

        for (s32 l=0; l<len+1; ++l)
            array[l+used] = other[l];

        used = used + len + 1;
    }

    //------------------------------------------------------------

    //! Appends a string of the length l to this string.
    //! \param other: other String to append to this string.
    //! \param length: How much characters of the other string to add to this one.
    void append(const string<T>& other, s32 length)
    {   
		if (length<=0) return;

        s32 len = other.size();

        if (len < length)
        {   append(other);
            return;
        }

        len = length;
        --used;

        if (used + len > allocated)
            reallocate((s32)used + (s32)len);

        for (s32 l=0; l<len; ++l)
            array[l+used] = other[l];

        used = used + len + 1;
    }

    //------------------------------------------------------------

    void append(const T *other, s32 length)
    {   
		if (length<=0) return;

        s32 len = core::strlen(other);

        if (len < length)
        {
			append(other);
            return;
        }

        len = length;
        --used;

        if (used + len > allocated)
            reallocate((s32)used + (s32)len);

        for (s32 l=0; l<len; ++l)
            array[l+used] = other[l];

        used = used + len + 1;
    }

	//------------------------------------------------------------

    void append(const T *other)
    {   
		--used;

		s32 len = core::strlen(other);

        if (used + len + 1 > allocated)
            reallocate((s32)used + (s32)len + 1);

        for (s32 l=0; l<len+1; ++l)
            array[l+used] = other[l];

        used = used + len + 1;
    }

    //------------------------------------------------------------

    //! Reserves some memory.
    //! \param count: Amount of characters to reserve.
    void reserve(s32 count)
    {   
		if (count < allocated)
            return;

        reallocate(count);		

		for (s32 u=used; u<count; u++)
		{
			array[u] = 0;
		}

		used = count;
    }

    //------------------------------------------------------------

    //! finds first occurrence of character in string
    //! \param c: Character to search for.
    //! \return Returns position where the character has been found,
    //! or -1 if not found.
    s32 find_first(T c) const
    {   
		for (s32 i=0; i<used; ++i)
            if (array[i] == c)
                return i;

        return -1;
    }

	//------------------------------------------------------------

    //! replacing character in string
    //! \param old_char: Character which must be replased
	//! \param new_char: Character to replace
    void replace_char(T old_char, T new_char) 
    {   
		for (s32 i=0; i<used; ++i)
		{
            if (array[i] == old_char)
			{
                array[i] = new_char;
			}
		}
    }

    //------------------------------------------------------------

    //! finds next occurrence of character in string
    //! \param c: Character to search for.
    //! \param startPos: Position in string to start searching.
    //! \return Returns position where the character has been found,
    //! or -1 if not found.
    s32 find_next(T c, s32 startPos) const
    {   
		for (s32 i=startPos; i<used; ++i)
            if (array[i] == c)
                return i;
        return -1;
    }

    //------------------------------------------------------------

    //! finds last occurrence of character in string
    //! \param c: Character to search for.
    //! \return Returns position where the character has been found,
    //! or -1 if not found.
    s32 find_last(T c) const
    {   
		for (s32 i=used-1; i>=0; --i)
            if (array[i] == c)
                return i;
        return -1;
    }

    //------------------------------------------------------------

    //! Returns a substring
    //! \param begin: Start of substring.
    //! \param length: Length of substring.
    string<T> sub_string(s32 begin, s32 length)
    {   
		if (length <= 0)
            return string<T>("");

        string<T> o;
        o.reserve(length+1);

        for (s32 i=0; i<length; ++i)
            o.array[i] = array[i+begin];

        o.array[length] = 0;
        o.used = o.allocated;

        return o;
    }

    //------------------------------------------------------------

    string < T > & sprintf(const c8 *format, ... )
    {   		
		static c8 buffer[16384];

		va_list ap;
        va_start( ap, format );
        s32 resval = _vsnprintf( buffer, 16384, format, ap );
        va_end( ap );

		(*this)=buffer;
	
		return *this;
    }

	//------------------------------------------------------------

    string < T > & sprintf(const wchar_t *format, ... )
    {   		
		static wchar_t buffer[16384];

		va_list ap;
        va_start( ap, format );
        s32 resval = _vsnwprintf( buffer, 16384, format, ap );
        va_end( ap );

		(*this)=buffer;
	
		return *this;
    }

    //------------------------------------------------------------

    void operator += (T c)
    {   
		append(c);
    }

    //------------------------------------------------------------

    void operator += (const string<T>& other)
    {   
		append(other);
    }

private:
    //------------------------------------------------------------
    //! Returns a character converted to lower case
    T toLower(const T& t) const
    {
        if (t>=(T)'A' && t<=(T)'Z')
            return t + ((T)'a' - (T)'A');
        else
            return t;
    }
    //------------------------------------------------------------
    //! Reallocate the array, make it bigger or smaler
    void reallocate(s32 new_size)
    {
        T* old_array = array;

        array = new T[new_size];
        allocated = new_size;

        s32 amount = used < new_size ? used : new_size;
        for (s32 i=0; i<amount; ++i)
            array[i] = old_array[i];

        if (allocated < used)
            used = allocated;

		delete [] old_array;
    }	
	//------------------------------------------------------------
	//--- member variables
	T* array;
	s32		allocated;
	s32		used;
};

//----------------------------------------------------------------------------

typedef string<c8>		stringc;
typedef string<wchar_t>	stringw;

//----------------------------------------------------------------------------

template <class T>
u32 strlen(const T *str)
{
	if (!str) return 0;
	const T *ptr = str;
	while (*ptr) ptr++;
	return ptr - str;
}

//----------------------------------------------------------------------------

// finds last occurance of char in the string
template <class T>
s32 findLast(T* str, c8 ch)
{
	s32 len = (s32)core::strlen(str);
    s32 n = len - 1;
    while (n >= 0)
    {
		if (str[n] == ch)
            break;			
        n--;
    }
    return n;
}

//----------------------------------------------------------------------------

// finds last occurance of the '\\' and '/' in the string
template <class T>
s32 findLastDelimiter(T* str)
{
	s32 len = (s32)core::strlen(str);
    s32 n = len - 1;
    while (n >= 0)
    {
        if (str[n] == '\\' || str[n] == '/')
            break;        
		n--;
    }
    return n;
}

//----------------------------------------------------------------------------

// calculating relative path between two directories
// \param from_dir = "H:/My_Engine/media/models/decorations/trees/"
// \param to_dir   = "H:/My_Engine/media/textures/decorations/"
// \return           "../../../textures/decorations/"
template <class T>
const T* getRelativePathBetweenDirectories(
	const const_string<T> &from_dir, const const_string<T> &to_dir)
{
	static string<T> FromDir;
	static string<T> ToDir;
	static string<T> ResPath;

	FromDir = from_dir.c_str();
	ToDir   = to_dir.c_str();
	ResPath = "";

	FromDir.make_lower();
	ToDir.make_lower();

	if (ToDir.size()==0)
		return ToDir.c_str();

	if (FromDir[FromDir.size()-1] != '\\' &&
		FromDir[FromDir.size()-1] != '/')
		FromDir.append("/");

	if (ToDir[ToDir.size()-1] != '\\' &&
		ToDir[ToDir.size()-1] != '/')
		ToDir.append("/");

	s32 i=0, j=0;
	s32 fdsize = FromDir.size();
	s32 tdsize = ToDir.size();
	for (i = 0; i < fdsize; i++)
	{
		if (i < tdsize && (
				FromDir[i] == ToDir[i] ||
				(FromDir[i] == '\\' && ToDir[i] == '/') ||
				(FromDir[i] == '/' && ToDir[i] == '\\')))
		{
			if (i == (fdsize - 1) && i < (tdsize - 1))
				ResPath = ToDir.sub_string(i + 1, tdsize - i);
			continue;
		}

		if (FromDir[i] != '\\' && FromDir[i] != '/')
			for (j = i; j >= 0; j--, i--)
				if (FromDir[j] == '\\' || FromDir[j] == '/')
				{
					i++;
					break;
				}

		CHECK_MIN_RANGE(i, 0);

		int slash_cnt = 0;
		for (j = i; j<fdsize; j++)
			if (FromDir[j]=='\\' || FromDir[j]=='/')
				slash_cnt++;

		if (i > 0)
			for (s32 s = 0; s < slash_cnt; s++)
				ResPath+="../";	

		if (i < tdsize)
			ResPath += ToDir.sub_string(i, tdsize - i);

		break;
	}
	return ResPath.c_str();
}

//----------------------------------------------------------------------------

template <class T>
const T* getRelativePathBetweenDirectories(
	const string<T> &from_dir, const string<T> &to_dir)
{
	return getRelativePathBetweenDirectories(
		const_string<T>(from_dir.c_str(), from_dir.size() + 1),
		const_string<T>(to_dir.c_str(), to_dir.size() + 1));
}

//----------------------------------------------------------------------------

template <class T>
const T* getRelativePathBetweenDirectories(const T *from_dir, const T *to_dir)
{
	return getRelativePathBetweenDirectories(
		const_string<T>(from_dir), const_string<T>(to_dir));
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFilePath(const const_string<T> &sourceStr)
{
	s32 lastDelimiterInStringPos = findLastDelimiter(sourceStr.c_str());
    s32 lenSrc = sourceStr.size();

	static string<T> pathStr;
	pathStr = "";
	pathStr.reserve(lenSrc+1);
	
	s32 i=0;
	for (i=0; i<=lenSrc; i++)
		pathStr[i] = 0;

	for (i=0; i<=lastDelimiterInStringPos; i++)
		pathStr[i] = sourceStr[i];

	return pathStr.c_str();
}


//----------------------------------------------------------------------------

template <class T>
const T* extractFilePath(const string<T> &sourceStr)
{
	return extractFilePath(
		const_string<T>(sourceStr.c_str(), sourceStr.size() + 1));
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFilePath(const T *sourceStr)
{
	return extractFilePath(const_string<T>(sourceStr));
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFileName(const const_string<T> &sourceStr, bool noExt = false)
{
	s32 lastDelimiterInStringPos = findLastDelimiter(sourceStr.c_str());
    s32 len = sourceStr.size();

    static array<T> name;
	name.set_used(len - lastDelimiterInStringPos);
	name[name.size() - 1] = 0;

	if (!noExt)
		for (s32 j = 0, i = lastDelimiterInStringPos+1; i <= len - 1; i++, j++)
			name[j] = sourceStr[i];
	else
	{
		for (s32 j = 0, i = lastDelimiterInStringPos+1; i <= len - 1; i++, j++)
		{
			if (sourceStr[i] == '.')
			{
				name[j] = 0;
				break;
			}
			name[j] = sourceStr[i];
		}
	}

	return name.const_pointer();
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFileName(const string<T> &sourceStr, bool noExt = false)
{
	return extractFileName(
		const_string<T>(sourceStr.c_str(), sourceStr.size() + 1), noExt);
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFileName(const T *sourceStr, bool noExt = false)
{
	return extractFileName(const_string<T>(sourceStr), noExt);
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFileExt(const const_string<T> &sourceStr)
{   
	s32 lastPointInStringPos = findLast(sourceStr.c_str(), '.');
    s32 lenSrc = sourceStr.size();
	s32 lenExt = lenSrc - lastPointInStringPos;

	static string<T> extStr;
	extStr = "";

	if (lenExt>0)
	{
		extStr.reserve(lenExt);

		s32 i=0;
		for (i=0; i<lenExt; i++)
			extStr[i] = 0;

		if (lastPointInStringPos>=-1 && lastPointInStringPos<lenSrc)  
			for (s32 j=0, i=lastPointInStringPos+1; i<=lenSrc-1; i++, j++)
				extStr[j] = sourceStr[i];
	}
	return extStr.c_str();
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFileExt(const string<T> &sourceStr)
{
	return extractFileExt(
		const_string<T>(sourceStr.c_str(), sourceStr.size() + 1));
}

//----------------------------------------------------------------------------

template <class T>
const T* extractFileExt(const T *sourceStr)
{
	return extractFileExt(const_string<T>(sourceStr));
}

//----------------------------------------------------------------------------

template <class T>
void changeFileExt(core::string<T>& Str, c8 *newExt)
{   
	s32 lastPointInStringPos = findLast(Str.c_str(), '.');
	s32 sizeOfNewExt = (s32)core::strlen(newExt);
	s32 strSize = Str.size();

	if (lastPointInStringPos >= 0)
	{
		Str.reserve(sizeOfNewExt+lastPointInStringPos+1);
		s32 i=0;
		for (i=0; i<sizeOfNewExt; i++)
			Str[lastPointInStringPos+1+i]=newExt[i];
	}
}

//----------------------------------------------------------------------------

__MY_CORE_LIB_API__ const c8*	Utf8ToAnsi(const c8* utf8);
__MY_CORE_LIB_API__ const c8*	AnsiToUtf8(const c8* ansi);
__MY_CORE_LIB_API__ void		CP866ToCP1251(c8 *s);
__MY_CORE_LIB_API__ const c8*	ToHexStr(u32 v);
__MY_CORE_LIB_API__ const c8*	ToHexStr64(u64 v);

//----------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//----------------------------------------------------------------------------

#endif