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

void	lll_memset(void* dest, lll_u32 size, char data)
{
	for (lll_u32 i = 0; i < size; i++)
	{
		*(char*) dest = data;
		dest++;
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

lll_b8	lll_ht_init(lll_ht*	hashtable, lll_u32 capacity, lll_u32 duplicated_capacity)
{
	lll_b8 result = lll_arena_init(&hashtable->entries_arena, sizeof(union lll_ht_entry) * (capacity + duplicated_capacity));
	hashtable->entries = (union lll_ht_entry*) hashtable->entries_arena.memory;
	hashtable->free_list = NULL;
	hashtable->capacity = capacity;
	return result;
}

void	lll_ht_set(lll_ht* hashtable, lll_u32 key, void* value)
{
	lll_assert(value != NULL, "hashtable value cannot be zero");
	union lll_ht_entry* entry = hashtable->entries + (key % hashtable->capacity);
	if (!entry->is_occupied)
	{
		entry->data = value;
		entry->next = NULL;
		entry->hash = key;
	}
	else
	{
		lll_b8	found_myself = LLL_FALSE;
		while (LLL_TRUE)
		{
			if (entry->hash == key)
			{
				found_myself = LLL_TRUE;
				break;
			}
			if (entry->next)
			{
				entry = entry->next;
			}
			else
			{
				break;
			}
		}
		if (found_myself)
		{
			entry->data = value;
		}
		else
		{
			if (hashtable->free_list)
			{
				entry->next = hashtable->free_list;
				hashtable->free_list = entry->next->next_deleted;
			}
			else
			{
				entry->next = lll_arena_alloc(&hashtable->entries_arena, sizeof(union lll_ht_entry), 8);
			}
			entry = entry->next;
			entry->data = value;
			entry->next = NULL;
			entry->hash = key;
		}
	}
}

void*	lll_ht_remove(lll_ht* hashtable, lll_u32 key)
{
	void*	result = NULL;
	union lll_ht_entry* entry = hashtable->entries + (key % hashtable->capacity);
	if (entry->is_occupied)
	{
		if (entry->hash == key)
		{
			result = entry->data;
			if (entry->next)
			{
				union lll_ht_entry* next = entry->next;
				*entry = *next;
				next->is_occupied = LLL_FALSE;
				next->next_deleted = hashtable->free_list;
				hashtable->free_list = next;
			}
			else
			{
				entry->is_occupied = LLL_FALSE;
				entry->next_deleted = NULL;
			}
		}
		else
		{
			union lll_ht_entry* entry_previous = entry;
			entry = entry->next;
			while (entry)
			{
				if (entry->hash == key)
				{
					result = entry->data;
					entry_previous->next = entry->next;
					entry->is_occupied = LLL_FALSE;
					entry->next_deleted = hashtable->free_list;
					hashtable->free_list = entry;
					break;
				}
				entry_previous = entry;
				entry = entry->next;
			}
		}
	}
	return result;
}

void**	lll_ht_get(lll_ht* hashtable, lll_u32 key)
{
	void**	result = NULL;
	union lll_ht_entry* entry = hashtable->entries + (key % hashtable->capacity);
	if (entry->is_occupied)
	{
		while (LLL_TRUE)
		{
			if (entry->hash == key)
			{
				result = &entry->data;
				break;
			}
			if (entry->next)
			{
				entry = entry->next;
			}
			else
			{
				break;
			}
		}
	}
	return result;
}

void		lll_ht_clear(lll_ht* hashtable)
{
	lll_memset(hashtable->entries, sizeof(union lll_ht_entry) * hashtable->capacity, 0);
	hashtable->free_list = NULL;
	lll_arena_clear(&hashtable->entries_arena);
}

lll_u32	lll_hash_string(lll_string string)
{
	lll_u32 seed = 5381;
	for (lll_u32 i = 0; i < string.length; i++)
	{
		seed = ((seed << 5) + seed) + *string.data;
		string.data++;
	}
	return seed;
}
