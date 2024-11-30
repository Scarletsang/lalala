#include "lalala.h"
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

void	lll_assert_explicit(lll_b8 check, const char* message, int line_number, const char* function_name, const char* file_name)
{
	if (!check)
	{
		lll_fdprintf(STDERR_FILENO, "ASSERT TRIGGERED in %s at function %s, line %d:\nReason: %s\n", file_name, function_name, line_number, message);
		kill(getpid(), SIGABRT);
	}
}

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

lll_b8	lll_arena_init(lll_arena* arena, lll_u32 size)
{
	arena->memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	lll_assert(arena->memory, "mmap failed");
	arena->capacity = size;
	arena->used = 0;
	return (arena->memory != ((void*) -1));
}

void	lll_arena_split(lll_arena* arena, lll_u32 size, lll_arena* output)
{
	output->memory = lll_arena_alloc(arena, size, 1);
	output->used = 0;
	output->capacity = size;
}

void*	lll_arena_alloc(lll_arena* arena, lll_u32 size, lll_u32 alignment)
{
	lll_assert(((alignment & (alignment - 1)) == 0), "Alignment is not power of two");
	void*	result = arena->memory + arena->used;
	lll_u64 modulo = (lll_u64)result & (alignment - 1);
	if (modulo != 0)
	{
		result += (alignment - modulo);
		size += (alignment - modulo);
	}
	lll_assert((arena->used + size) <= (arena->capacity), "Arena overflow");
	arena->used += size;
	return result;
}

void	lll_arena_clear(lll_arena* arena)
{
	arena->used = 0;
}

lll_arena_snapshot	lll_arena_cheese(lll_arena* arena)
{
	return arena->used;
}

void	lll_arena_rollback(lll_arena* arena, lll_arena_snapshot snapshot)
{
	arena->used = snapshot;
}

void	lll_ht_init(lll_ht*	hashtable,
					lll_u32	entry_size,
					lll_u32	capacity,
					lll_arena* arena)
{
	lll_arena_split(arena, entry_size * capacity, &hashtable->entries);
	hashtable->entry_size = entry_size;
}

void*	lll_ht_set(lll_ht* hashtable,
				   lll_string key,
				   void* value)
{
	(void) hashtable;
	(void) key;
	(void) value;
	return NULL;
}

void*	lll_ht_get(lll_ht* hashtable,
				   lll_string key)
{
	(void) hashtable;
	(void) key;
	return NULL;
}

void*	lll_ht_remove(lll_ht* hashtable,
					  lll_string key)
{
	(void) hashtable;
	(void) key;
	return NULL;
}

