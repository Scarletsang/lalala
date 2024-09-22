#include "lalala.h"
#include <signal.h>
#include <unistd.h>

#define READ_BUFF_SIZE	1024

typedef lll_string	lll_token;

static char		read_buff[READ_BUFF_SIZE];
static lll_string	user_input = {read_buff, 0};

void	lll_assert_explicit(lll_b8 check, const char* message, int line_number, const char* function_name, const char* file_name)
{
	if (!check)
	{
		lll_fdprintf(STDERR_FILENO, "ASSERT TRIGGERED in %s at function %s, line %d:\nReason: %s\n", file_name, function_name, line_number, message);
		kill(getpid(), SIGABRT);
	}
}

#ifdef DEBUG
#define lll_assert(check, message) lll_assert_explicit(check, message, __LINE__, __FUNCTION__, __FILE__) 
#else
#define lll_assert(check, message) ((void) 0)
#endif

lll_u32	lll_strlen(char* string)
{
	lll_u32	length = 0;
	while (*string)
	{
		string++;
		length++;
	}
	return length;
}

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

lll_b8	lll_string_is_equal(lll_string target, lll_string reference)
{
	lll_u32	matched = 0;
	while ((matched < target.length) &&
		   (matched < reference.length) &&
		   (*target.data == *reference.data))
	{
		target.data++;
		reference.data++;
		matched++;
	}
	return (matched == reference.length) && (matched == target.length);
}

void	lll_memcpy(void* dest, void* src, lll_u32 size)
{
	for (lll_u32 i = 0; i < size; i++)
	{
		*(lll_u8*) dest = *(lll_u8*) src;
		dest++;
		src++;
	}
}

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
	lll_sprintf_test();
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
		if (lll_tokenize(&user_input, &token))
		{
			if (lll_string_is_equal(token, lll_cstring("help")))
			{
				lll_printf("help\n");
			}
			else if (lll_string_is_equal(token, lll_cstring("get")))
			{
				lll_printf("get\n");
			}
			else if (lll_string_is_equal(token, lll_cstring("set")))
			{
				lll_printf("set\n");
			}
			else if (lll_string_is_equal(token, lll_cstring("remove")))
			{
				lll_printf("remove\n");
			}
			else if (lll_string_is_equal(token, lll_cstring("quit")))
			{
				quit = LLL_TRUE;
			}
			else
			{
				lll_printf("Error: invalid command\n");
			}
		}
		user_input.data = read_buff;
	}
	return 0;
}
