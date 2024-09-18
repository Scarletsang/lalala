#include "lalala.h"
#include <unistd.h>

#define READ_BUFF_SIZE	1024

typedef lll_string	lll_token;

static char		read_buff[READ_BUFF_SIZE];
static lll_string	user_input = {read_buff, 0};

lll_u32	lll_strchr(lll_string string, char c)
{
	for (lll_u32 i = 0; i < string.length; i++)
	{
		if (*string.data == c)
		{
			return i;
		}
		string.data++;
	}
	return LLL_INVALID_INDEX;
}

lll_b8	lll_tokenize(lll_string* input, lll_token* output)
{
	lll_string string = *input;
	for (lll_u32 i = 0; (i < string.length) && (*string.data == ' '); i++)
	{
		string.data++;
	}
	string.length -= (string.data - input->data);
	if (string.length == 0)
	{
		return LLL_FALSE;
	}
	char*	start = string.data;
	char	lookup_target = ' ';
	if ((*start == '\'') || (*start == '"'))
	{
		lookup_target = *start;
		start++;
		string.data++;
		string.length--;
	}
	lll_u32	i = 0;
	for (; i < string.length; i++)
	{
		if (*string.data == lookup_target)
		{
			output->data = start;
			if ((lookup_target == '\'') || (lookup_target == '"'))
			{
				output->length = i;
			}
			else
			{
				output->length = i + 1;
			}
			string.data++;
			break;
		}
		string.data++;
	}
	if (i == string.length)
	{
		if (lookup_target == ' ')
		{
			output->data = start;
			output->length = i;
		}
		else
		{
			return LLL_FALSE;
		}
	}
	string.length = (input->data + input->length) - string.data;
	*input = string;
	return LLL_TRUE;
}

int main(void)
{
	lll_b8		quit = LLL_FALSE;
	lll_token	token;
	while (!quit)
	{
		(void) !write(STDOUT_FILENO, "lalala terminal>", sizeof("lalala terminal>") - 1);
		lll_i32	size = (lll_i32) read(STDIN_FILENO, read_buff, READ_BUFF_SIZE);
		if (size <= 0)
		{
			quit = LLL_TRUE;
			continue;
		}
		lll_u32	index = lll_strchr((lll_string){read_buff, size}, '\n');
		if (index == LLL_INVALID_INDEX)
		{
			(void) !write(STDOUT_FILENO, "Error: input too long", sizeof("Error: input too long") - 1);
			user_input.length = 0;
			continue;
		}
		user_input.length = index;
		while (lll_tokenize(&user_input, &token))
		{
			(void) !write(STDOUT_FILENO, token.data, token.length);
			(void) !write(STDOUT_FILENO, "\n", 1);
		}
		user_input.data = read_buff;
	}
	return 0;
}
