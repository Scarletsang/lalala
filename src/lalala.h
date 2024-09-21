#ifndef LALALA
#define LALALA

#define LLL_TRUE	1
#define LLL_FALSE	0
#define LLL_INVALID_INDEX	((lll_u32) -1)
#define LLL_INVALID_SIZE	((lll_u32) -1)

typedef unsigned int	lll_u32;
typedef int				lll_i32;
typedef char			lll_i8;
typedef unsigned char	lll_b8;
typedef unsigned char	lll_u8;
typedef long			lll_u64;

typedef char*	lll_va_list;
#define lll_va_start(arg) ((lll_va_list) &arg + (sizeof(arg) << 3))

static inline void* __lll_va_arg(lll_va_list* list, lll_u32 size)
{
	void*	result = *list;
	*list += (size << 3);
	return result;
}

#define	lll_va_arg(list, type) (*(type*) __lll_va_arg(&list, sizeof(type)))

typedef struct
{
	char*	data;
	lll_u32	length;
}	lll_string;

#define lll_cstring(c_string) ((lll_string) {c_string, sizeof(c_string) - 1})

typedef struct
{
	void*	data;
	lll_u32	length;
}	lll_slice;

typedef struct
{
	lll_u8*	memory;
	lll_u32	used;
	lll_u32	capacity;
}	lll_arena;

typedef lll_u32	lll_arena_snapshot;

lll_b8	lll_arena_init(lll_arena* arena, lll_u32 size);
void	lll_arena_split(lll_arena* arena, lll_u32 size, lll_arena* output);
void*	lll_arena_alloc(lll_arena* arena, lll_u32 size, lll_b8 is_alligned);

#define lll_arena_alloc_type(arena, type) ((type*) lll_arena_alloc(arena, sizeof(type), LLL_TRUE))

lll_arena_snapshot	lll_arena_cheese(lll_arena* arena);
void	lll_arena_rollback(lll_arena* arena, lll_arena_snapshot snapshot);

struct lll_ht_entry
{
	lll_b8	is_deleted;
	lll_u8	data[];
};

typedef struct lll_ht
{
	lll_arena	entries;
	lll_u32		entry_size;
}	lll_ht;

void	lll_ht_init(lll_ht*	hashtable,
					lll_u32	entry_size,
					lll_u32	capacity,
					lll_arena* arena);

void*	lll_ht_set(lll_ht* hashtable,
				   lll_string key,
				   void* value);

void*	lll_ht_get(lll_ht* hashtable,
				   lll_string key);

void*	lll_ht_remove(lll_ht* hashtable,
				   	  lll_string key);

#endif
