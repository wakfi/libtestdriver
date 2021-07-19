// naive_string_search.h

#ifndef NAIVE_STRING_SEARCH_H
#define NAIVE_STRING_SEARCH_H

#include <string>
#include <list>

bool naive_string_search(const std::string& pattern, const std::string& text, std::list<int>& matches)
{
	const int n = text.length();
    const int m = pattern.length();
	if(n < m) return false;
	if(m == 0) return true;

    int i, j;

    for(i = 0; i < n-m+1; ++i)
    {
        for(j = 0; j < m; ++j)
        {
            if(text[i+j] != pattern[j]) break;
        }
        if(j == m) matches.push_back(i);
    }

    return !!matches.size();
}

#endif
