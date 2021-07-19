//----------------------------------------------------------------------
// NAME: Walker Gray
// FILE: boyermoore.h
// DATE: Summer 2021
// DESC: Implementation of Boyer-Moore w/ Galil rule
//       (generalized as Apostolico-Giancarlo)
//----------------------------------------------------------------------

#ifndef BOYERMOORE_WITH_GALIL_IMPLEMENTATION
#define BOYERMOORE_WITH_GALIL_IMPLEMENTATION
#include <string>
#include <list>
#include "naive_string_search.h"

// Size of the character set, or alphabet, in this case the ASCII characters
constexpr int CHARSET_LENGTH = 0x80;

/**
 * Boyer-Moore string-search algorithm implementation as a single function. For this
 * implementation, the Wikipedia article is being treated as if a proper specification,
 * EXCLUDING the "Implementations" article section
 * - https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm
 *
 * Information sources for the Apostolico-Giancarlo modifications
 * - https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#Variants
 * - https://en.wikipedia.org/wiki/Apostolico%E2%80%93Giancarlo_algorithm
 */
bool boyermoore(const std::string& pattern, const std::string& text, std::list<int>& matches)
{
	// text_length is 'n' and pattern_length is 'm' from Wikipedia article's definitions of
	// variables for the algorithim description
	// - https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#Definitions
	const int text_length = text.length();
	const int pattern_length = pattern.length();
    const int pattern_end_index = pattern_length - 1;
	// A string cannot contain a substring longer than the string itself
	if(text_length < pattern_length) return false;
	// Zero-width patterns trivially match any string
	if(pattern_length == 0) return true;
	if(text_length == 1)
	{
		if(pattern[0] == text[0])
		{
			matches.push_back(0);
			return true;
		} else {
			return false;
		}
	}
	// If pattern_length == 1 it would probably be most effective to just call a naive
	// search implementation since that is what will end up happening here in that case,
	// just with more overhead
	if(pattern_length == 1) return naive_string_search(pattern, text, matches);
	// At this point, pattern_length is guaranteed to be >= 2, and text_length is
	// guaranteed to be >= pattern_length. This is important for indexing safety


	/***  PREPROCESSING  ***/
	/*
	Bad Character rule
	https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#The_bad_character_rule
	*/
	int bad_character_table[CHARSET_LENGTH][pattern_length];
	for(int char_code = 0; char_code < CHARSET_LENGTH; ++char_code)
	{
		for(int pattern_index = 0; pattern_index < pattern_length; ++pattern_index)
		{
			// Initialize all values to the shift length needed to shift the pattern past
			// that character in the text. This is the default value, used for cases where
			// there is not another instance of the character in the rest of the pattern,
			// i.e. in substring pattern[0...pattern_index-1]. This includes characters
			// that never appear in the pattern
			bad_character_table[char_code][pattern_index] = pattern_index+1;
		}
	}
	// pattern_character_table is intended to help optimize bad_character_table generation
	int pattern_character_table[CHARSET_LENGTH];
	for(int char_code = 0; char_code < CHARSET_LENGTH; ++char_code)
	{
		// pattern_character_table holds indecies. Initial values must be -1
		pattern_character_table[char_code] = -1;
	}
	for(int pattern_index = 0; pattern_index < pattern_length; ++pattern_index)
	{
		// Initialize indecies in table for all characters in pattern
		bad_character_table[pattern[pattern_index]][pattern_index] = 0;
		if(pattern_character_table[pattern[pattern_index]] != -1) continue;
		// pattern_character_table holds the index of the first occurance of each character
		// in the pattern, or -1 for characters that do not appear
		pattern_character_table[pattern[pattern_index]] = pattern_index+1;
	}
	for(int char_code = 0; char_code < CHARSET_LENGTH; ++char_code)
	{
		// Characters not present in pattern are already known from pattern_character_table,
		// and can be skipped since bad_character_table is already initialized for
		// characters not present in pattern
		if(pattern_character_table[char_code] == -1) continue;
		// First instance of each character can be jumped to using pattern_character_table
		// Specifically, the character after the first instance is used
		for(int pattern_index = pattern_character_table[char_code]; pattern_index < pattern_length; ++pattern_index)
		{
			/*
			All instances of characters in pattern have been set to 0 in the BCT. For a
			pattern "babbbbaabbab", the table row before this loop would look like:
			- [1, 0, 3, 4, 5, 6, 0, 0, 9, 10, 0, 12]
			The bad_character_table is intended to hold, for any given character, the
			length from any index in the pattern to the next occurance of the character in
			the pattern before that index. Therefore, the row needs to look like this:
			- [1, 0, 1, 2, 3, 4, 0, 0, 1, 2, 0, 1]
			By already having all occurances of each character initialized to 0 in their
			rows, a memoization/dynamic programming approach is possible by skipping 0s
			and otherwise adding 1 to the value at the previous index. For this to work,
			iteration must start on either the index of or the index immediately after
			the first occurance of the character, or at index 1. Since values before the
			first occurance are already correct for having no more occurances before them
			and the first occurance is already 0, it makes the most sense to start at the
			index after the first occurance
			*/
			if(!bad_character_table[char_code][pattern_index]) continue;
			bad_character_table[char_code][pattern_index] = bad_character_table[char_code][pattern_index-1] + 1;
		}
	}

	/*
	Good Suffix rule
	https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#The_good_suffix_rule
	*/
	// suffix_match_table is 'L' and prefix_suffix_table is 'H' from Wikipedia description
	// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#Preprocessing_2
	int suffix_match_table[pattern_length] = {};
	int prefix_suffix_table[pattern_length] = {};
	int suffix_search_offset;
	suffix_match_table[pattern_length-1] = 0;
	suffix_match_table[pattern_length-2] = 0;
	// Initialize suffix_match_table
	for(int pattern_index = pattern_length-2; pattern_index > 0; --pattern_index)
	{
		/*
		The good suffix rule (suffix_match_table) requires finding where suffixes of
		various lengths can be found earlier in the pattern, if present. These comments
		will reference the following example pattern:
		"This is my mystring string. It contains mystring"
		Here, the maximum good suffix is " mystring", present at index 10. There is
		another valid good suffix, "string", present at index 20.
		During each iteration of the outer loop, the "first" index of the suffix is moved
		back by one to eaxmine a longer suffix. In the inner loop, suffix_search_index is
		the first index of a possible match for the suffix in the pattern. Here, the
		bad_character_table is being used to quickly find possible matches by skipping
		through the instances of the first character of the suffix in the table
		*/
		int suffix_search_index = pattern_index - bad_character_table[pattern[pattern_index]][pattern_index-1] - 1;
		for(; suffix_search_index > 0; suffix_search_index -= bad_character_table[pattern[pattern_index]][--suffix_search_index])
		{
			// If the character directly before the suffix and the potential match are
			// the same, it is not a match. If a shift was performed using this suffix
			// match, it would be guaranteed to fail again at the same location, namely
			// the index immediately preceeding the match
			// if(pattern[suffix_search_index-1] == pattern[pattern_index-1]) continue;
			int suffix_length = pattern_length - pattern_index;
			for(suffix_search_offset = 1; suffix_search_offset < suffix_length; ++suffix_search_offset)
			{
				// Compare from the start of the potential match until the end of the suffix length
				if(pattern[pattern_index + suffix_search_offset] != pattern[suffix_search_index + suffix_search_offset]) break;
			}
			if(suffix_search_offset == suffix_length)
			{
				// Shift length computation for valid good suffix matches. Stored such that
				// at index i, the stored shift distance is for the suffix
				// pattern[i+1...pattern_length], enabling the index at which a mismatch
				// occurs to be used directly to retrieve the correct good suffix_match
				// shift length during main search processing
				suffix_match_table[pattern_index-1] =  pattern_index - suffix_search_index;
				break;
			}
		}
	}
	// If only the last character in the pattern has been matched, the shift should be the
	// distance to the next occurance of that character, which can be found in the
	// bad_character_table, rather than defaulting to the value at the same index in
	// the prefix_suffix_table
	if(!suffix_match_table[pattern_length-2]) suffix_match_table[pattern_length-2] = bad_character_table[pattern[pattern_length-1]][pattern_length-2];
	// Initialize prefix_suffix_table
	prefix_suffix_table[pattern_length-1] = pattern_length - (pattern[pattern_length-1] == pattern[0]);
	for(int pattern_index = pattern_length-2; pattern_index > 0; --pattern_index )//prefix_suffix_table[pattern_index+1] = pattern_length - prefix_suffix_table[pattern_index+1], --pattern_index)
	{
		// prefix_suffix_table is similar to suffix_match_table except this is only
		// concerned with suffixes match a prefix of the pattern, uses suffix lengths
		// to determine shift lengths, and every index in the table will get populated
		if(pattern[pattern_index] != pattern[0])
		{
			prefix_suffix_table[pattern_index] = prefix_suffix_table[pattern_index+1];
			continue;
		}
		int suffix_length = pattern_length - pattern_index;
		int prefix_index = 1;
		for(; prefix_index < suffix_length; ++prefix_index)
		{
			if(pattern[pattern_index + prefix_index] != pattern[prefix_index]) break;
		}
		// The tables in this implementation are storing precalculated shift lengths rather
		// than the values needed to calculate the shift length, used by the specification.
		// If the prefix matches the suffix, H[i] is defined as the suffix_length, and the
		// shift at that position is calculated using pattern_length - suffix_length.
		// The suffix length is calculated using pattern_length - pattern_index, therefore
		// the shift from H[i] for a match = pattern_length - (pattern_length -
		// pattern_index) = (pattern_length - pattern_length) + pattern_index = pattern_index.
		if(prefix_index == suffix_length) prefix_suffix_table[pattern_index] = pattern_index;
		// If the suffix doesn't match the prefix, the table uses the shift for the largest
		// prefix suffix match with a higher index than itself. Copying the value at the next
		// highest index in the table accomplishes this
		else prefix_suffix_table[pattern_index] = prefix_suffix_table[pattern_index+1];
	}
	// The algorithm requires a strict suffix/prefix, so the full pattern does not count
	// as a suffix that matches a prefix (in both cases the full pattern). Instead, the
	// value from index 1 is used, similar to the rest of table generation when a prefix
	// suffix match is not found
	prefix_suffix_table[0] = prefix_suffix_table[1];
	// If the first character (the last character in the pattern) is a mismatch, always
	// use the shift in bad_character_table
	prefix_suffix_table[pattern_length-1] = 0;

	/*
	Apostolico-Giancarlo
	https://epubs.siam.org/doi/10.1137/0215007
	*/
	bool skip_validation_table[pattern_length][pattern_length];
	for(int pattern_index = 0; pattern_index < pattern_length; ++pattern_index)
	{
		for(int skip_length = 0; skip_length < pattern_length; ++skip_length)
		{
			if(skip_length <= pattern_index)
			{
				int offset = 0;
				do
				{
					if(pattern[pattern_index - offset] != pattern[pattern_end_index - offset]) break;
				} while(++offset < skip_length);
				skip_validation_table[pattern_index][skip_length] = offset != skip_length;
			} else {
				skip_validation_table[pattern_index][skip_length] = prefix_suffix_table[pattern_end_index - pattern_index] != pattern_end_index - pattern_index;
			}
		}
	}
	/***  END PREPROCESSING  ***/


	/***  SEARCH  ***/
	// pattern_alignment_index is k from Wikipedia definitions
	// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#Definitions
	int pattern_alignment_index = pattern_end_index;
	int pattern_index, pattern_alignment_start, bad_character_shift, good_suffix_shift, pattern_shift_length;
	/*
	Apostolico-Giancarlo table
	This table enables a generalization of the Galil rule, a significant optimization.
	The table is defined such that at a given pattern alignment index of k, the value of
	apostolico_giancarlo_skip[k] is equal to the number of characters successfully matched
	at that alignment. If text[k], text[k-1], and text[k-2] match the corresponding
	characters pattern[pattern_length - 1], pattern[pattern_length - 2], and
	pattern[pattern_length - 3], but the next character is a mismatch,
	apostolico_giancarlo_skip[k] should be 3. Values at k-1 and k-2 don't change
	*/
	int apostolico_giancarlo_skip[text_length] = {};
	while(pattern_alignment_index < text_length)
	{
		// pattern_alignment_start is analogous to index -1 from typical iteration contexts
		pattern_alignment_start = pattern_alignment_index - pattern_length + 1;
		for(pattern_index = pattern_end_index;  pattern_index >= 0;)
		{
			if(apostolico_giancarlo_skip[pattern_alignment_start + pattern_index])
			{
				if(skip_validation_table[pattern_index][apostolico_giancarlo_skip[pattern_alignment_start + pattern_index]])
				{
					apostolico_giancarlo_skip[pattern_alignment_index] = pattern_end_index - pattern_index;
					bad_character_shift = bad_character_table[text[pattern_alignment_index]][pattern_end_index - 1] + 1;
					good_suffix_shift = suffix_match_table[pattern_index];
					if(!good_suffix_shift) good_suffix_shift = prefix_suffix_table[pattern_index];
					pattern_shift_length = good_suffix_shift > bad_character_shift ? good_suffix_shift : bad_character_shift;
					pattern_alignment_index += pattern_shift_length;
					break;
				}

				pattern_index -= apostolico_giancarlo_skip[pattern_alignment_start + pattern_index];
				continue;
			}
			if(text[pattern_alignment_start + pattern_index] == pattern[pattern_index])
			{
				--pattern_index;
				continue;
			}

			/***  MISMATCH  ***/
			// Update Apostolico-Giancarlo table
			apostolico_giancarlo_skip[pattern_alignment_index] = pattern_end_index - pattern_index;
			// Calculate shift using precomputed tables
			bad_character_shift = bad_character_table[text[pattern_alignment_start + pattern_index]][pattern_index];
			good_suffix_shift = suffix_match_table[pattern_index];
			if(!good_suffix_shift) good_suffix_shift = prefix_suffix_table[pattern_index];
			pattern_shift_length = good_suffix_shift > bad_character_shift ? good_suffix_shift : bad_character_shift;
			pattern_alignment_index += pattern_shift_length;
			break;
		}
		if(pattern_index < 0)
		{
			/***  MATCH  ***/
			matches.push_back(pattern_alignment_start);
			pattern_shift_length = prefix_suffix_table[0];
			apostolico_giancarlo_skip[pattern_alignment_index] = pattern_length - pattern_shift_length;
			pattern_alignment_index += pattern_shift_length;
		}
	}
	/***  END SEARCH  ***/


	// Convert truthy/falsy integer to a proper bool via !! idiom
	return !!matches.size();
}

#endif
