#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <sstream>
#include <vector>
using namespace std;


//namespace ABE
//{
	namespace Util
	{
		// wchar to ansi
		static const string WToA(LPCWSTR pwszSrc)
		{
			int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
			if (nLen<= 0) return std::string("");
			char* pszDst = new char[nLen];
			if (NULL == pszDst) return std::string("");
			WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
			pszDst[nLen -1] = 0;
			std::string strTemp(pszDst);
			delete [] pszDst;
			return strTemp;
		}

		static vector<string> Split(const string &str, const string &delim)
		{
			if( str.empty() )
				return vector<std::string>();

			vector< std::string > result;
			size_t last = 0;
			size_t index = str.find_first_of(delim, last);
			while (index != std::string::npos)
			{
				result.push_back(str.substr(last, index - last));
				last = index + 1;
				index = str.find_first_of(delim, last);
			}
			if (index-last > 0)
			{
				result.push_back(str.substr(last, index - last));
			}

			return result;
		}

		static std::string Trim(const string &str)
		{
			string tmp = str;
			if (tmp.empty())   
				return tmp;

			tmp.erase(0, tmp.find_first_not_of(" "));
			tmp.erase(tmp.find_last_not_of(" ") + 1);

			return tmp;
		}

		static std::string ToString(int num)
		{
			stringstream ss;
			ss << num;
			string str;
			ss >> str;
			return str;
		}

		static int ToNumber(const string &numString)
		{
			// int to string (don't using VC itoa())
			stringstream ss;
			ss << numString;
			int num;
			ss >> num;
			return num;
		}
	}
//}
#endif