#define LALALA_IMPLEMENTATION
#include "lalala.h"
#include <unistd.h>

#define READ_BUFF_SIZE	1024

typedef lll_string	lll_token;

static char		read_buff[READ_BUFF_SIZE];
static lll_string	user_input = {read_buff, 0};

void	lll_sprintf_test()
{
	char		buffer[1024];
	lll_string	string = {buffer, 1024};
	lll_u32		length;
	for (int i = 0; i < 2; i++)
	{
		if (i == 1)
		{
			string.length = 10;
		}
		// Note: format d
		length = lll_sprintf(string, "front[%d]after", 1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%d]after", -1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%bd]after", -1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%xd]after", -1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%xbd]after", -1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf((lll_string){buffer, 3}, "%d]after", 1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%-+10.5d]after", 1234);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		// Note: format u
		length = lll_sprintf(string, "front[%u]after", 2234567);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%u]after", 2234567);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%bu]after", 2234567);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%xu]after", 2234567);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%bxu]after", 2234567);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%10.4u]after", 12);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%-10.4u]after", 12345);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%10.u]after", 12345);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%3.9u]after", 12345);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		// Note: format p
		length = lll_sprintf(string, "front[%p]after", &length);
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		// Note: format s
		length = lll_sprintf(string, "front[%s]after", "what the actual fuck");
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%.*s]after", 4, "what the actual fuck");
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%10s]after", "what");
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%-10s]after", "what");
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%-10.5s]after", "123456789");
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		length = lll_sprintf(string, "front[%-*.*s]after", 10, 5, "123456789");
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
		// Note: format c
		length = lll_sprintf(string, "front[%c]after", 'a');
		write(STDOUT_FILENO, buffer, length);
		write(STDOUT_FILENO, "\n", 1);
	}
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
			output->length = i;
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
#if DEBUG
	lll_sprintf_test();
#endif
	lll_b8		quit = LLL_FALSE;
	lll_b8		syntax_error = LLL_FALSE;
	lll_token	token;
	lll_arena	arena;
	lll_ht		hashtable;

	if (!lll_arena_init(&arena, 2 * LLL_PAGE_SIZE))
	{
		(void) !write(STDOUT_FILENO, "Failed to allocate memory", sizeof("Failed to allocate memory") - 1);
		return 1;
	}
	lll_ht_init(&hashtable, 256, 256);
	while (!quit)
	{
		syntax_error = LLL_FALSE;
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
		if (lll_tokenize(&user_input, &token))
		{
			if (lll_string_is_equal(token, lll_cstring("help")))
			{
				lll_printf("Avaliable hashtable commands:\nget    <key>\nset    <key> <string>\nremove <key>\nclear\n");
			}
			else if (lll_string_is_equal(token, lll_cstring("clear")))
			{
				lll_ht_clear(&hashtable);
				lll_arena_clear(&arena);
				lll_printf("cleared hashtable\n");
			}
			else if (lll_string_is_equal(token, lll_cstring("get")))
			{
				if (lll_tokenize(&user_input, &token))
				{
					lll_string** value = (lll_string**) lll_ht_get(&hashtable, lll_hash_string(token));
					if (value)
					{
						lll_printf("Found by key '%.*s': '%.*s'\n", token.length, token.data, (*value)->length, (*value)->data);
					}
					else
					{
						lll_printf("Cannot find entry by key '%.*s'\n", token.length, token.data);
					}
				}
				else
				{
					syntax_error = LLL_TRUE;
				}
			}
			else if (lll_string_is_equal(token, lll_cstring("set")))
			{
				lll_token	parameter_2;
				if (lll_tokenize(&user_input, &token) && lll_tokenize(&user_input, &parameter_2))
				{
					lll_string*	data = lll_arena_alloc(&arena, sizeof(lll_string) + parameter_2.length, 8);
					data->data = (char*) (data + 1);
					data->length = parameter_2.length;
					lll_memcpy(data->data, parameter_2.data, parameter_2.length);
					lll_ht_set(&hashtable, lll_hash_string(token), data);
					lll_printf("Set entry using key '%.*s': '%.*s'\n", token.length, token.data, data->length, data->data);
				}
				else
				{
					syntax_error = LLL_TRUE;
				}
			}
			else if (lll_string_is_equal(token, lll_cstring("remove")))
			{
				if (lll_tokenize(&user_input, &token))
				{
					lll_string* value = lll_ht_remove(&hashtable, lll_hash_string(token));
					if (value)
					{
						lll_printf("Removed entry using key '%.*s': '%.*s'\n", token.length, token.data, value->length, value->data);
					}
					else
					{
						lll_printf("Cannot find entry by key '%.*s'\n", token.length, token.data);
					}
				}
				else
				{
					syntax_error = LLL_TRUE;
				}
			}
			else if (lll_string_is_equal(token, lll_cstring("quit")))
			{
				quit = LLL_TRUE;
			}
			else
			{
				lll_printf("Error: invalid command\n");
				lll_printf("Avaliable hashtable commands:\nget    <key>\nset    <key> <string>\nremove <key>\nclear\n");
			}
			if (syntax_error)
			{
				lll_printf("Error: Syntax error\n");
			}
		}
		user_input.data = read_buff;
	}
	return 0;
}
