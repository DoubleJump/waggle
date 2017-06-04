#ifdef DEBUG
	#define fail(...) printf(__VA_ARGS__); printf("Error in file: %s line: %d \n ", __FILE__, __LINE__ );
	#define assert(expr, ...) if(!(expr)) { fail(__VA_ARGS__); } 
	#define NO_DEFAULT_CASE default: { fail("Invalid case in switch statement\n"); }
#else
	#define fail(...)
	#define assert(expr, ...)
	#define NO_DEFAULT_CASE default: break;
#endif

#define KILOBYTES(value) ((value)*1024LL)
#define MEGABYTES(value) (KILOBYTES(value)*1024LL)
#define GIGABYTES(value) (MEGABYTES(value)*1024LL)
#define TERABYTES(value) (GIGABYTES(value)*1024LL)
#define ARRAY_COUNT(Array) (sizeof(Array) / sizeof((Array)[0]))

#define FLAG_SET(mask, flag) ((flag & mask) == flag)
#define FOR(index, start, end) for(memsize index = start; index < end; ++index)