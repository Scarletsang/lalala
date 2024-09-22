#include "lalala.h"
#include <unistd.h>

#define READ_BUFF_SIZE	1024

typedef lll_string	lll_token;

static char		read_buff[READ_BUFF_SIZE];
static lll_string	user_input = {read_buff, 0};

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
	return (matched == reference.length);
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

struct lll_sprintf_state
{
	lll_b8	is_left_justify : 1; // -
	lll_b8	is_force_positive_sign : 1; // +
	lll_b8	is_force_positive_space : 1; // ' '
	lll_b8	is_padding_with_zero : 1; // 0
	lll_b8	is_binary : 1; // b
	lll_b8	is_binary_big_endian : 1; // B
	lll_b8	is_binary_hex : 1; // h
	lll_b8	has_width : 1;
	lll_b8	has_precision : 1;
	lll_i32	width; // Note: minimum characters to print
	lll_u32	precision; // Note: for string, maximum characters to print; for number, minimum of digits to print
};

static lll_u8	lll_sprintf_binary(lll_u64 data, lll_u8 data_size, struct lll_sprintf_state state, char* output)
{
	lll_u8	output_size = 0;
	if (state.is_binary_big_endian)
	{
		if (state.is_binary_hex)
		{
			output_size = data_size * 2 + (data_size - 1);
			char* write_ptr = output + output_size - 2;
			for (int i = 0; i < output_size; i += 3)
			{
				if ((data & 0xf) >= 10)
				{
					write_ptr[1] = 'a' + (data & 0xf) - 10;
				}
				else
				{
					write_ptr[1] = '0' + (data & 0xf);
				}
				if (((data >> 4) & 0xf) >= 10)
				{
					write_ptr[0] = 'a' + ((data >> 4) & 0xf) - 10;
				}
				else
				{
					write_ptr[0] = '0' + ((data >> 4) & 0xf);
				}
				write_ptr[2] = ' ';
				data >>= 8;
				write_ptr -= 3;
			}
		}
		else
		{
			output_size = data_size * 8 + (data_size - 1);
			char* write_ptr = output + output_size - 1;
			for (int i = 0; i < (data_size * 8); i++)
			{
				*write_ptr = '0' + (data & 1);
				data >>= 1;
				write_ptr--;
				if (i % 8 == 7)
				{
					*write_ptr = ' ';
					write_ptr--;
				}
			}
		}
	}
	else if (state.is_binary)
	{
		if (state.is_binary_hex)
		{
			output_size = data_size * 2 + (data_size - 1);
			char* write_ptr = output;
			for (int i = 0; i < data_size; i++)
			{
				if ((data & 0xf) >= 10)
				{
					write_ptr[1] = 'a' + (data & 0xf) - 10;
				}
				else
				{
					write_ptr[1] = '0' + (data & 0xf);
				}
				if (((data >> 4) & 0xf) >= 10)
				{
					write_ptr[0] = 'a' + ((data >> 4) & 0xf) - 10;
				}
				else
				{
					write_ptr[0] = '0' + ((data >> 4) & 0xf);
				}
				write_ptr[2] = ' ';
				data >>= 8;
				write_ptr += 3;
			}
		}
		else
		{
			output_size = data_size * 8 + (data_size - 1);
			char* write_ptr = output + 8;
			*write_ptr = ' ';
			write_ptr--;
			for (int i = 0; i < data_size; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					*write_ptr = '0' + (data & 1);
					data >>= 1;
					write_ptr--;
				}
				write_ptr += 18;
				*write_ptr = ' ';
				write_ptr--;
			}
		}
	}
	return output_size;
}

static void	lll_sprintf_output(char** buffer_memory, lll_string* buffer, char* data, lll_u32 data_size)
{
	lll_u32	writable_length = buffer->length - (*buffer_memory - buffer->data);
	if (writable_length > data_size)
	{
		lll_memcpy(*buffer_memory, data, data_size);
		*buffer_memory += data_size;
	}
	else
	{
		lll_memcpy(*buffer_memory, data, writable_length);
		*buffer_memory += writable_length;
	}
}

lll_u32	lll_sprintf(lll_string buffer, const char* format, ...)
{
	char*	buffer_memory = buffer.data;
	lll_va_list	args = lll_va_start(format);
#define lll_sprintf_has_space(n) ((buffer_memory - buffer.data + n) < buffer.length)
	while (*format && lll_sprintf_has_space(0))
	{
		if (*format == '%')
		{
			format++;
			struct lll_sprintf_state state = {0};
			// Note: sprintf flags
			for (lll_i32 i = 0; (i < 4) && *format; i++)
			{
				switch (*format)
				{
				case '-': state.is_left_justify         = LLL_TRUE; break;
				case '+': state.is_force_positive_sign  = LLL_TRUE; break;
				case ' ': state.is_force_positive_space = LLL_TRUE; break;
				case '0': state.is_padding_with_zero    = LLL_TRUE; break;
				case 'b': state.is_binary               = LLL_TRUE; break;
				case 'B': state.is_binary_big_endian    = LLL_TRUE; break;
				case 'h': state.is_binary_hex           = LLL_TRUE; break;
				default: goto no_flags;
				}
				format++;
			}
no_flags:
			// Note: sprintf width
			if (*format == '*')
			{
				state.width = lll_va_arg(args, lll_u32);
				format++;
			}
			else
			{
				lll_u32	width = 0;
				const char*	start = format;
				while ((*format >= '0') && (*format <= '9'))
				{
					width = width * 10 + (*format - '0');
					format++;
				}
				if ((format - start) > 0)
				{
					state.has_width = LLL_TRUE;
					state.width = width;
				}
			}
			// Note: sprintf precision
			if (*format == '.')
			{
				format++;
				state.has_precision = LLL_TRUE;
				if (*format == '*')
				{
					state.precision = lll_va_arg(args, lll_u32);
					format++;
				}
				else
				{
					lll_u32	precision = 0;
					while ((*format >= '0') && (*format <= '9'))
					{
						precision = precision * 10 + (*format - '0');
						format++;
					}
					state.precision = precision;
				}
			}
			static char	substitution_buffer[64 + 7];
			lll_u8		substitution_buffer_size = 0;
			switch (*format)
			{
			case 'd':
			case 'i':
			{
				lll_i32	number = lll_va_arg(args, lll_i32);
				substitution_buffer_size = lll_sprintf_binary(number, sizeof(number), state, substitution_buffer);
				if (substitution_buffer_size == 0)
				{
					if (number == 0)
					{
						*substitution_buffer = '0';
						substitution_buffer_size = 1;
					}
					else
					{
						lll_i32	temp = number;
						while (temp != 0)
						{
							temp /= 10;
							substitution_buffer_size++;
						}
						temp = number;
						char*	start = substitution_buffer + substitution_buffer_size - 1;
						for (int i = substitution_buffer_size - 1; i >= 0; i--)
						{
							if (number > 0)
							{
								*start = '0' + (temp % 10);
							}
							else
							{
								*start = '0' + -(temp % 10);
							}
							temp /= 10;
							start--;
						}
						if ((number < 0) && lll_sprintf_has_space(0))
						{
							*buffer_memory = '-';
							buffer_memory++;
						}
					}
				}
				lll_sprintf_output(&buffer_memory, &buffer, substitution_buffer, substitution_buffer_size);
				format++;
			} break;
			case 'x':
			case 'X':
			case 'u':
			{
				lll_u32	number = lll_va_arg(args, lll_u32);
				substitution_buffer_size = lll_sprintf_binary(number, sizeof(number), state, substitution_buffer);
				if (substitution_buffer_size == 0)
				{
					if (number == 0)
					{
						*substitution_buffer = '0';
						substitution_buffer_size = 1;
					}
					else
					{
						lll_u32	temp = number;
						while (temp != 0)
						{
							temp /= 10;
							substitution_buffer_size++;
						}
						temp = number;
						char*	start = substitution_buffer + substitution_buffer_size - 1;
						for (int i = substitution_buffer_size - 1; i >= 0; i--)
						{
							*start = '0' + (temp % 10);
							temp /= 10;
							start--;
						}
					}
				}
				lll_sprintf_output(&buffer_memory, &buffer, substitution_buffer, substitution_buffer_size);
				format++;
			} break;
			case 'p':
			{
				lll_ptr	pointer = lll_va_arg(args, lll_ptr);
				if (pointer == 0)
				{
					*substitution_buffer = '0';
					substitution_buffer_size = 1;
				}
				else
				{
					lll_ptr	temp = pointer;
					while (temp != 0)
					{
						temp /= 16;
						substitution_buffer_size++;
					}
					temp = pointer;
					char*	start = substitution_buffer + substitution_buffer_size - 1;
					for (int i = substitution_buffer_size - 1; i >= 0; i--)
					{
						if (temp % 16 >= 10)
						{
							*start = 'a' + (temp % 16);
						}
						else
						{
							*start = '0' + (temp % 16);
						}
						temp /= 16;
						start--;
					}
				}
				lll_sprintf_output(&buffer_memory, &buffer, "0x", 2);
				lll_sprintf_output(&buffer_memory, &buffer, substitution_buffer, substitution_buffer_size);
				format++;
			} break;
			case 's':
			{
				char*	string = lll_va_arg(args, char*);
				lll_sprintf_output(&buffer_memory, &buffer, string, lll_strlen(string));
				format++;
			} break;
			case 'c':
			{
				char	character = lll_va_arg(args, char);
				substitution_buffer_size = lll_sprintf_binary(character, sizeof(character), state, substitution_buffer);
				if (substitution_buffer_size == 0)
				{
					if (lll_sprintf_has_space(0))
					{
						*buffer_memory = character;
						buffer_memory++;
					}
				}
				else
				{
					lll_sprintf_output(&buffer_memory, &buffer, substitution_buffer, substitution_buffer_size);
				}
				format++;
			} break;
			case '%':
			{
				if (lll_sprintf_has_space(0))
				{
					*buffer_memory = '%';
					buffer_memory++;
				}
				format++;
			} break;
			case 0: // Note: syntax error but no need to produce an error
			default:
				break;
			}
		}
		else
		{
			*buffer_memory = *format;
			buffer_memory++;
			format++;
		}
	}
#undef lll_sprintf_has_space
	return (buffer_memory - buffer.data);
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
	static char	buff[1024];
	lll_string	string = {buff, 1024};
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
				(void) !write(STDOUT_FILENO, "help\n", sizeof("help\n") - 1);
			}
			else if (lll_string_is_equal(token, lll_cstring("get")))
			{
				lll_u32 length = lll_sprintf(string, "get\n");
				(void) !write(STDOUT_FILENO, buff, length);
			}
			else if (lll_string_is_equal(token, lll_cstring("set")))
			{
				(void) !write(STDOUT_FILENO, "set\n", sizeof("set\n") - 1);
			}
			else if (lll_string_is_equal(token, lll_cstring("remove")))
			{
				(void) !write(STDOUT_FILENO, "remove\n", sizeof("remove\n") - 1);
			}
			else if (lll_string_is_equal(token, lll_cstring("quit")))
			{
				quit = LLL_TRUE;
			}
			else
			{
				(void) !write(STDOUT_FILENO, "Error: invalid command\n", sizeof("Error: invalid command\n") - 1);
			}
		}
		user_input.data = read_buff;
	}
	return 0;
}
