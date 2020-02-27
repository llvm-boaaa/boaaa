#include "command_line_seperator.h"

char** boaaa::parse(char* in, int* argc)
{
	return parse(NULL, in, argc);
}

int count_args(const char* in) {
	if (in == nullptr)
		return -1;		
	if (*in == '\0')
		return 0;
	int count = 0;
	const char* it = in;
	char c; 
	bool in_qm = false; //quotation mark
	bool set_next = true;
	bool ignore_space = false;
	//loop is easier implementation of loop belown in parse, explanation is there.
	for (c = *in; c != '\0'; c = *(++it)) {
		switch (c) {
		case '"':
			in_qm ^= true;
			ignore_space = false; //when go into qm set to false; when going out of qm setting also to false
			set_next = true;
			break;
		case ' ':
			if (in_qm)
			{
				if (ignore_space) {
					break;
				}
			}
			else {
				set_next = true;
				break;
			}
		default:
			if (!set_next) break;
			count++;
			set_next = false;
			ignore_space = in_qm;
		}
	}
	
	return count;
}

char** boaaa::parse(char* programm_name, char* in, int* argc)
{
	*argc = count_args(in);
	if (*argc < 0) return NULL;

	if (programm_name) {
		(*argc)++;
		if (*argc == 1)
			return &programm_name;
	}

	char** argv = (char**) malloc((*argc) * (sizeof(char*)));

	int n = 0;
	if (programm_name) {
		argv[n++] = programm_name;
	}

	char* it = in;
	bool set_next = true;
	bool ignore_space = false;
	bool in_qm = false;
	char c;
	for (c = *in; c != '\0'; c = *(++it)) {
		switch (c) {
		case '"':
			in_qm ^= true;
			ignore_space = false; //when go into qm set to false; when going out of qm setting also to false
			*it = '\0';
			set_next = true;
			break;
		case ' ':
			if (in_qm) 
			{
				if (ignore_space) {
					break;
				}
			} else {
				*it = '\0';
				set_next = true;
				break;
			}
			//if not true set true, in following cases
			// "aaa"bbb    or    "aaa" bbb   aaa " " bbb
			//  ^   ^             ^    ^     ^    ^  ^    : ^ means: set pointer to argv
			// |_  |_            |_  ||_     _  ||_||_    : | means: set true, _ means: set false
		default:
			if (set_next) {//set argv[n] to pointer of c and increments n
				argv[n++] = it;
				set_next = false;
				ignore_space = in_qm; //when in qm after set of pointer ignore, when out of qm not ignore spaces
			}
		}
	}

	return argv;
}
