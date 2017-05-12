// word_associations.cpp -- by Thatcher Ulrich http://tulrich.com 2005

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Program to discover word associations within a corpus of text.


#include "base/container.h"
#include "base/tu_random.h"
#include <stdio.h>
#include <stdlib.h>


void print_usage()
{
	printf("word_associations -- discover word associations in a corpus of text\n");
	printf("\n");
	printf("Send your text in on stdin.  The program outputs a list of word-like\n");
	printf("tokens, followed by a table of the most common co-occuring words for\n");
	printf("each word in the token table.\n");
}


// Known tokens & their stats.
struct token_info
{
	tu_string m_text;
	int m_occurrences;
	int m_lowercase_occurrences;
	int m_rank;

	token_info() : m_occurrences(0), m_lowercase_occurrences(false), m_rank(-1)
	{}
};
array<token_info> s_tokens;
string_hash<int> s_token_index;

struct associated_word
{
	int m_id;
	float m_weight;
	float m_conditional_weight;  // like a probability, but not properly normalized.

	associated_word() : m_id(-1), m_weight(0), m_conditional_weight(0)
	{
	}

	// For qsort
	static int compare_weight(const void* a, const void* b)
	{
		float wa = static_cast<const associated_word*>(a)->m_conditional_weight;
		float wb = static_cast<const associated_word*>(b)->m_conditional_weight;

		// Sort larger to smaller.
		if (wa > wb) return -1;
		else if (wa < wb) return 1;
		else return 0;
	}
};

const int MAX_STATS = 500;

array< array<associated_word> > s_stats;



int get_token_id(const tu_string& word)
// Look up or add the given word, and return its id number.
{
	int id = -1;
	if (s_token_index.get(word, &id))
	{
		// Already in the table.
		s_tokens[id].m_occurrences++;
		return id;
	}

	// Must add the word to the table.
	id = s_tokens.size();
	token_info info;
	info.m_text = word;
	info.m_occurrences++;
	s_tokens.push_back(info);
	s_stats.resize(s_tokens.size());
	s_token_index.add(word, id);

	// xxxx debugging
	if (id < 20 || (id % 1000) == 0)
	{
		fprintf(stderr, "Token %d is '%s'\n", id, word.c_str());
	}

	return id;
}


// Keep a FIFO history of the last N tokens, for finding
// co-occurrences as they come in.
const int WINDOW_BITS = 4;
const int WINDOW_SIZE = (1 << WINDOW_BITS);
const int WINDOW_MASK = WINDOW_SIZE - 1;
int s_window[WINDOW_SIZE];

// s_current_token points at the latest addition to the window.
int s_current_token = 0;


void clear_window()
// Clear the window FIFO.
{
	for (int i = 0; i < WINDOW_SIZE; i++)
	{
		s_window[i] = -1;
	}
}


bool is_word_separator(int c)
// Return true if the given character is a valid word-separator.
// Basically, it's a separator if it's whitespace or '-'
{
	if (c == ' '
	    || c == '\t'
	    || c == '\n'
	    || c == '\r'
	    || c == '/'
	    || c == '-')
	{
		return true;
	}

	return false;
}


bool is_trailing_punctuation(int c)
// Return true if the character is punctuation that often appears at
// the end of a word (i.e. periods, parens, etc).
{
	if (c == '.'
	    || c == ':'
	    || c == ';'
	    || c == ')'
	    || c == '\''
	    || c == '\"'
	    || c == '!'
	    || c == '?'
	    || c == ','
	    || c == '/')
	{
		return true;
	}

	return false;
}


static string_hash<bool> s_common_words;
static bool s_inited_common_words = false;
static const char* COMMON_WORDS[] = {
	"that",
	"with",
	"this",
	"from",
	"there",
	"have",
	"were",
	"they",
	"which",
	"like",
	"then",
	"their",
	"some",
	"when",
	"upon",
	"what",
	"them",
	"would",
	"been",
	"these",
	"though",
	"here",
	"than",
	"those",
	"your",
	"thou",
	NULL
};


bool validate_word(char* wordbuf)
// Return true if the word is valid, and munges the word to fix simple
// things (like trailing punctuation).  Things that make a word
// invalid are:
// 
// * less than 4 characters long
//
// * contains numbers or bad punctuation
//
// * is a very common English word
//
// * rarely/never appears lowercased (Therefore probably a proper noun)
{
	assert(wordbuf);

	if (s_inited_common_words == false)
	{
		s_inited_common_words = true;
		
		for (int i = 0; ; i++)
		{
			if (COMMON_WORDS[i] == NULL) break;
			s_common_words.add(tu_string(COMMON_WORDS[i]), true);
		}
	}

	int len = strlen(wordbuf);

	// Chop off trailing punctuation.
	while (len > 0 && is_trailing_punctuation(wordbuf[len - 1]))
	{
		wordbuf[len - 1] = 0;
		len--;
	}

	// Chop off trailing "'s", "Peter's" --> "Peter"
	if (len > 1 && wordbuf[len - 2] == '\'' && wordbuf[len - 1] == 's')
	{
		wordbuf[len - 2] = 0;
		len -= 2;
	}

	if (len < 4)
	{
		// Too short.
		return false;
	}

	assert(wordbuf[len] == 0);

	// Scan to make sure chars are all alphabetic.  Also lowercase
	// the whole thing.
	for (int i = 0; i < len; i++)
	{
		int c = tolower(wordbuf[i]);
		if (c < 'a' || c > 'z')
		{
			// Non alphabetic character; this is not a
			// valid word.
			return false;
		}
		wordbuf[i] = c;
	}

	// Ignore very common English words.
	if (s_common_words.get(tu_string(wordbuf), NULL))
	{
		return false;
	}

	return true;
}


bool get_next_token(FILE* in)
// Read the next word-like token from the input stream.  Add the token
// to the token table and the input window as appropriate.  Possibly
// clears the window if we hit some kind of obvious document boundary.
{
	// (possibly add to token table, clear window if we hit a boundary, etc)

	bool skipping = false;
	
	static const int MAX_WORD = 10;
	char buf[MAX_WORD + 1];
	int next_char = 0;

	for (;;)
	{
		int c = fgetc(in);
		if (c == EOF)
		{
			// TODO: Finish current word?
			return false;
		}

		if (is_word_separator(c))
		{
			// Finish current word.
			
			if (skipping)
			{
				skipping = false;
				continue;
			}

			// Terminate the word string.
			assert(next_char <= MAX_WORD);
			buf[next_char] = 0;

			bool is_lower = islower(buf[0]);
			if (validate_word(buf))
			{
				// Valid: add to the window.
				int token_id = get_token_id(tu_string(buf));
				s_current_token = (s_current_token + 1) & WINDOW_MASK;
				s_window[s_current_token] = token_id;

				if (is_lower)
				{
					// Count the number of times we see it lowercased.
					s_tokens[token_id].m_lowercase_occurrences++;
				}

				return true;
			}
			else
			{
				// Not a valid word; keep scanning.
				next_char = 0;
				continue;
			}
		}
		else if (skipping)
		{
			// We're ignoring the current word.
			continue;
		}
		else
		{
			// Process the character.
			
			if (next_char >= MAX_WORD)
			{
				// This word is too long; ignore it.
				next_char = 0;
				skipping = true;
			}
			else
			{
				buf[next_char] = c;
				next_char++;
			}
		}
	}
}


void add_weight(int w0, int w1, float weight)
// Add weight to the co-occurrence score of w0[w1].
{
	array<associated_word>& a = s_stats[w0];

	// Is w1 already in the stats for w0?
	for (int i = 0, n = a.size(); i < n; i++)
	{
		if (a[i].m_id == w1)
		{
			// Add our weight.
			a[i].m_weight += weight;
			return;
		}
	}

	if (a.size() < MAX_STATS)
	{
		associated_word aw;
		aw.m_id = w1;
		aw.m_weight = weight;

		a.push_back(aw);
	}
	else
	{
		// Random replacement of a low-weighted entry.
		for (int i = 0; i < 10; i++)
		{
			int ai = tu_random::next_random() % MAX_STATS;
			float p = tu_random::get_unit_float();
			if (p >= a[ai].m_weight)
			{
				// Replace this unlucky entry.
				a[ai].m_id = w1;
				a[ai].m_weight = weight;
				return;
			}
		}
		// else we just drop this weight.
	}
}


void process_token()
// Update statistics on the current token.
{
	int curr = s_window[s_current_token];
	assert(curr != -1);

	for (int i = 1; i < WINDOW_SIZE; i++)
	{
		int window_i = (s_current_token + WINDOW_SIZE - i) & WINDOW_MASK;
		int wi = s_window[window_i];
		if (wi <= 0) continue;
		if (wi == curr) continue;

		const float bias = 0.2f;  // closer to 0 should give more weight to closer words
		compiler_assert(bias > 0);
		float weight = bias / (i - (1.0f - bias));

		add_weight(wi, curr, weight);
		//xxx		add_weight(curr, wi, weight);
	}
}

 
void process_input(FILE* in)
// Scan stdin until it runs out.  For each word-like token in the
// input stream, keep statistics on other tokens that appear near it.
{
	for (;;) {
		if (get_next_token(in) == false)
		{
			break;
		}

		process_token();
	}
}


struct sorted_entry
{
	int m_index;  // original index in s_tokens
	int m_occurrences;

	static int compare(const void* a, const void* b)
	{
		int oa = static_cast<const sorted_entry*>(a)->m_occurrences;
		int ob = static_cast<const sorted_entry*>(b)->m_occurrences;

		if (oa > ob) return -1;
		if (oa < ob) return 1;
		return 0;
	}
};
static array<sorted_entry> s_remap;


void postprocess()
// Sort by frequency, compute the conditional weights of co-occurring
// words, etc.
{
	// Sort tokens by frequency.
	s_remap.resize(s_tokens.size());
	for (int i = 0; i < s_remap.size(); i++)
	{
		s_remap[i].m_index = i;
		s_remap[i].m_occurrences = s_tokens[i].m_occurrences;
	}
	if (s_remap.size())
	{
		qsort(&s_remap[0], s_remap.size(), sizeof(s_remap[0]), sorted_entry::compare);
	}

	// Fill in the rank field.
	for (int i = 0; i < s_remap.size(); i++)
	{
		s_tokens[s_remap[i].m_index].m_rank = i;
	}

	// Compute conditional weight of the related words;
	// essentially, divide by probability, a la Bayes' Theorem.
	for (int a = 0; a < s_tokens.size(); a++)
	{
		array<associated_word>& awords = s_stats[a];
		for (int i = 0; i < awords.size(); i++)
		{
			// Find the conditional weight.
			const float bias = 100.0f;  // bias the occurrences factor, against infrequent words
			compiler_assert(bias >= 0);
			awords[i].m_conditional_weight =
				awords[i].m_weight / (bias + s_tokens[awords[i].m_id].m_occurrences);
		}

		if (awords.size() > 0)
		{
			qsort(&awords[0], awords.size(), sizeof(awords[0]), associated_word::compare_weight);
		}
	}
}


void print_tables()
// Print the token table, and the co-occurrences table.
{
	// Print the co-occurrences of the N most frequent tokens.
	int token_count = 3000;
	if (s_tokens.size() < token_count)
	{
		token_count = s_tokens.size();
	}

	// Start with the token table itself.
	for (int i = 0; i < token_count; i++)
	{
		int index = s_remap[i].m_index;

		printf("\t\"%s\", \n", s_tokens[index].m_text.c_str());
	}
	printf("\n");

	printf("{\n");
	// Now print a table of co-occurrences
	for (int i = 0; i < token_count; i++)
	{
		int index = s_remap[i].m_index;
		
		printf("\t{ ");
		
		array<associated_word>& a = s_stats[index];
		if (a.size() > 0)
		{
			int show_n = 20;
			int shown = 0;
			for (int j = 0; j < a.size() && shown < show_n; j++)
			{
				int a_id = a[j].m_id;
				if (s_tokens[a_id].m_rank >= token_count)
				{
					// Only print co-occurrences within our universe of N
					// tokens.
					continue;
				}
				if (s_tokens[a_id].m_lowercase_occurrences * 2 < s_tokens[a_id].m_occurrences)
				{
					// Skip words that are probably proper nouns.
					continue;
				}
				printf("%d, ", s_tokens[a_id].m_rank);
				shown++;
			}
		}
		printf("},\n");
	}
	printf("}\n");
}


void print_chain()
// Print a randomized chain of associated words.
{
	int N = s_tokens.size();
	if (N < 10) return;
	
	int current = tu_random::next_random() % N;
	printf("%s\n", s_tokens[s_remap[current].m_index].m_text.c_str());

	for (int i = 0; i < 20; i++)
	{
		array<associated_word>& a = s_stats[s_remap[current].m_index];
		if (a.size() > 0)
		{
			// Pick the next word.
			float f = tu_random::get_unit_float();
			f = powf(f, 3.0f);  // Skew towards 0
			int ai = iclamp(int(floorf(f * a.size())), 0, a.size() - 1);
			ai = imin(ai, 4);
			current = a[ai].m_id;

			printf("%s\n", s_tokens[s_remap[current].m_index].m_text.c_str());
		}
	}
}


void print_sample_association_chains()
// Print some chains of associated words.
{
	for (int i = 0; i < 20; i++)
	{
		printf("\n");
		print_chain();
	}
}


int	main(int argc, char *argv[])
{
	clear_window();

	process_input(stdin);
	postprocess();
	print_tables();

	print_sample_association_chains();

	return 0;
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
