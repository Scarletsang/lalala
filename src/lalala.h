#ifndef LALALA
#define LALALA

#define LLL_TRUE	1
#define LLL_FALSE	0
#define LLL_INVALID_INDEX	((lll_u32) -1)

typedef unsigned int	lll_u32;
typedef int				lll_i32;
typedef char			lll_i8;
typedef unsigned char	lll_b8;
typedef unsigned char	lll_u8;

typedef struct
{
	char*	data;
	lll_u32	length;
}	lll_string;

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
