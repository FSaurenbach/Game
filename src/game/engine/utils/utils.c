#include "./utils.h"

i08 file_exists(const char * path) {
    assert(path != NULL);
    FILE * f = fopen(path, "rb");
    if (f) {
        return 1;
    }
    return 0;
}

// Maximum readable size is 2 GB
file_info_t txt_file_query(const char * path) {
    assert(path != NULL);
    FILE * f = fopen(path, "rb");
    assert(f != NULL);

    file_info_t file;
    file.path = path;

    assert(fseek(f, 0, SEEK_END) == 0);

    file.len = ftell(f);
    assert(file.len > -1);

    file.content = calloc(file.len + 1, sizeof(char));
    assert(file.content != NULL);
    
    assert(fseek(f, 0, SEEK_SET) == 0);
    
    assert(fread(file.content, sizeof(char), file.len, f) == file.len);
    file.content[file.len + 1] = '\0';
    
    assert(fclose(f) == 0);
    return file;
}

// It's not possible to write binary data, or more than 2GB, using write_file.
i08 txt_file_write(const char * content, const char * path, const size_t len) { 
    assert(content != NULL && path != NULL && len > 0);
    FILE * f = fopen(path, "wb");
    assert(f != NULL);
    assert(fwrite(content, sizeof(char), len, f) == len);
    assert(fclose(f) == 0);
    return 1;
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
u64 hash_id(const char * id) {
    assert(id != NULL);
    u64 hash = FNV_OFFSET;
    for (const char * p = id; *p; p++) {
        hash ^= (u64)(u08)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Capacity MUST BE power of 2
void increase_ht_capacity(ht_t * ht, u32 capacity) {
    assert(ht != NULL && capacity % 2 == 0);
    ht_t new_ht;
    new_ht.data = calloc(capacity, sizeof(ht_item_t));
    assert(new_ht.data != NULL);
    new_ht.len = 0;
    new_ht.capacity = capacity;
    for (u64 i = 0; i < ht -> capacity; i++) {
        if (ht -> data[i] != NULL) {
            ht_insert(&new_ht, ht -> data[i] -> id, ht -> data[i] -> val);
        }
    }

    ht = &new_ht;
}

// - Add collision handling -> linear probing 
// - More functionality:
//  - Add          O(1)
//  - Remove       O(1) 
//  - Get          O(1)
//  - Expand       O(n)
// - Good hash function

// Capacity MUST BE power of 2
ht_t ht_create(const u32 capacity) {
    assert(capacity % 2 == 0);
    ht_t table;
    table.data = calloc(capacity, sizeof(ht_item_t));
    assert(table.data != NULL); 
    table.len = 0;
    table.capacity = capacity;
    return table;
}

u64 ht_insert(ht_t * ht, const char * id, void * data) {
    assert(ht != NULL && id != NULL && data != NULL);
    ht_item_t * item = calloc(1, sizeof(ht_item_t));
    assert(item != NULL);
    item -> hash = hash_id(id);
    item -> id = id;
    item -> val = data;
    u64 index = item -> hash & (u64)(ht -> capacity - 1);

    ht_item_t * current = ht -> data[index];
    while (current != NULL) {
        index++;
        current = ht -> data[index];
    }

    if (index > ht -> capacity) {
        increase_ht_capacity(ht, ht -> capacity * 2);
        ht -> data[index] = item;
    }

    if (ht -> data[index] == NULL) {
        ht -> data[index] = item;
    } 

    ht -> len++;   
    return index;
} 

void * ht_get(const ht_t * ht, const char * id) {
    assert(ht != NULL && id != NULL);
    u64 index = hash_id(id) & (u64)(ht -> capacity - 1); 
    ht_item_t * data = ht -> data[index];
     
    if (data == NULL) {
        return NULL;
    }

    if (data -> val == NULL) {
        return NULL;
    }

    while (strcmp(data -> id, id) != 0) {
        index++; 
        data = ht -> data[index]; 
    }

    if (data -> val != NULL && strcmp(data -> id, id) == 0) {
        return data -> val; 
    }

    return NULL;
}


u64 ht_rm_data(ht_t * ht, const char * id) {
    assert(ht != NULL && id != NULL);
    u64 index = hash_id(id) & (u64)(ht -> capacity - 1); 
    ht_item_t * data = ht -> data[index];
    
    if (data -> val == NULL) {
        return 0;
    }

    while (strcmp(data -> id, id) != 0) {
        index++; 
        data = ht -> data[index]; 
    }

    if (data -> val != NULL && strcmp(data -> id, id) == 0) {
        data = NULL;
        ht -> data[index] = NULL;
    }
    return index;
}


void ht_rm(ht_t * ht) {
    assert(ht != NULL);
    for (u32 i = 0; i < ht -> capacity; i++) {
        free(ht -> data[i]);
    }
    free(ht -> data);   
}

list_t list_create(const u32 capacity) {
    list_t list;
    list.data = calloc(capacity, sizeof(void*)); 
    assert(list.data != NULL);
    list.capacity = capacity;
    list.len = 0;
    return list;
}

void list_insert(list_t * list, void * data) {
    assert(list != NULL && data != NULL);
    if (list -> len + 1 > list -> capacity) {   
        list -> capacity += 10;
        void * new_data = realloc(list -> data, list -> capacity);
        assert(new_data != NULL);
        list -> data = new_data;
    }
    list -> data[list -> len] = data;
    list -> len++; 

}

i32 list_get_len_of(const list_t list, void * data) {
    assert(data != NULL);
    for (i32 i = 0; i < list.capacity - 1; i++) {
        if (list.data[i] == data) {
            return i;
        }
    }

    return 0x0;
}

void list_rm_at(list_t * list, u32 len) {
    assert(list != NULL);
    while (len + 1 < list -> capacity) {
        list -> data[len] = list -> data[len + 1]; 
    }      
}

void list_rm_data(list_t * list, void * data) {
    assert(list != NULL && data != NULL);
    u32 len = list_get_len_of(*list, data);
    list_rm_at(list, len);
}

void list_rm(list_t * list) {
    assert(list != NULL);
    for (i32 i = 0; i < list -> capacity - 1; i++) {
        free(list -> data[i]);
    } 

    free(list -> data);
}

#ifdef __EMSCRIPTEN
    #define NO_ANSI
#endif

char * type[LOG_LEVEL_LEN] = {
    "DEBUG",
    "SUCCESS",
    "WARN",
    "ERR"
}; 

char * colors[LOG_LEVEL_LEN] = {
    "\x1b[0m",
    "\x1b[32m",
    "\x1b[1;33m",
    "\x1b[31m"
};

time_t current_time;
struct tm * m_time; 

void get_time() {
    time(&current_time);
    m_time = localtime(&current_time);
}

void logger_log(LOG_LEVEL level, const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
        get_time();
        #ifndef NO_ANSI 
            printf("%s", colors[level]);
        #endif
        
        printf("[%d/%d/%d -> %d:%d:%d][%s] ", m_time -> tm_mday,
                                        m_time -> tm_mon,
                                        m_time -> tm_year + 1900,
                                        m_time -> tm_hour, 
                                        m_time -> tm_min, 
                                        m_time -> tm_sec,
                                        type[level]); 
        vfprintf(stdout, fmt, args);
        printf("\n%s", colors[LOG_DBG]);
    va_end(args);
}

f32 dist2D(f32 x1, f32 y1, f32 x2, f32 y2) {
    return sqrt(sq(x2 - x1) + sq(y2 - y1));
}

f32 dir2D(f32 x1, f32 y1, f32 x2, f32 y2) {
    return atan2(y2 - y1, x2 - x1);  
}

f32 ilerp(f32 start, f32 stop, f32 amt) {
    return start + (stop - start) * amt; 
}

i64 randnum_gen(i64 lower, i64 upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

v2_t v2_add(v2_t v1, v2_t v2) {
    return mk_v2(v1.x + v2.x, v1.y + v2.y);
}

v2_t v2_sub(v2_t v1, v2_t v2) {
    return mk_v2(v2.x - v1.x, v2.y - v1.y);
}

v2_t v2_lerp(v2_t v1, v2_t v2, f32 amt) {
    v2_t vec;
    vec.x = ilerp(v1.x, v2.x, amt);
    vec.y = ilerp(v1.y, v2.y, amt);
    return vec;
}


void v2_scale(v2_t * v, f32 scalar) {
    assert(v != NULL);
    v -> x *= scalar;
    v -> y *= scalar;
}

void v2_normalize(v2_t * v) {
    assert(v != NULL);
    f32 mag = v2_mag(*v);

    if (mag != 1.0f) {
        v -> x = v -> x / mag;
        v -> y = v -> y / mag;  
    }
}


void v2_limit(v2_t * v, f32 scalar) {
    assert(v != NULL);
    f32 len = v -> x * v -> x + v -> y * v -> y;
    f32 len_t = scalar * scalar;
    if (len > len_t) {
        v2_normalize(v);
        v2_scale(v, scalar);
    }         
}

void v2_rotate(v2_t * v, f32 theta) {
    assert(v != NULL);
    f32 temp = v -> x;
    v -> x = temp * cosf(theta) - v -> y * sinf(theta);
    v -> y = temp * sinf(theta) + v -> y * cosf(theta);
}

void v2_mag_set(v2_t * v, f32 scalar) {
    assert(v != NULL);
    v2_normalize(v);
    v2_scale(v, scalar);
}

void v2_negate(v2_t * v) {
    assert(v != NULL);
    v -> x = -v -> x;
    v -> y = -v -> y;   
}

f32 v2_mag(v2_t v) {
    return (sqrt(sq(v.x) + sq(v.y))); 
}

f32 v2_angle(v2_t v) {
    return atan2(v.y, v.x);
}

f32 v2_dot_prod(v2_t v1, v2_t v2) {
    return v1.x * v2.x + v1.y * v2.y; 
}

f32 v2_dist_between(v2_t v1, v2_t v2) {
    f32 dx = v1.x - v2.x;
    f32 dy = v1.y - v2.y;
    return sqrt(sq(dx) + sq(dy));
}

f32 v2_angle_between(v2_t v1, v2_t v2) {
    if (v1.x == 0 && v1.y == 0) return 0.0f;
    if (v2.x == 0 && v2.y == 0) return 0.0f;

    f32 dot = v2_dot_prod(v1, v2);
    f32 v1mag = v2_mag(v1);
    f32 v2mag = v2_mag(v2);
    f32 amt = dot / (v1mag * v2mag);

    if (amt <= -1) {
        return pi;
    } else if (amt >= 1) {
        return 0;
    }
    return acos(amt);
}

v2_t v2_projection(v2_t v1, v2_t v2) {
    f32 dot = v2_dot_prod(v1, v2);
    f32 len = v2_dot_prod(v2, v2);
    if (len == 0.0f) return v2;

    v2_t return_val = v2;
    v2_scale(&return_val, dot / len);
    return return_val;
}

void prof_start(prof_t * profiler, char * name) {
    assert(profiler != NULL && name != NULL);
    profiler -> name = name;
    profiler -> elapsed = 0.0f;
    profiler -> start = clock();
}

void prof_end(prof_t * profiler) {
    assert(profiler != NULL);
    profiler -> end = clock();
    profiler -> elapsed = (f64)(profiler -> end - profiler -> start) / CLOCKS_PER_SEC;
}

char * i32_to_str(i32 num) {
    char * string = calloc(64, sizeof(char)); 
    assert(string != NULL);
    snprintf(string, 64, "%d", num); 
    return string;
}

char * f64_to_str(f64 num) {
    char * string = malloc(64); 
    assert(string != NULL);
    snprintf(string, 64, "%f", num); 
    return string;   
}

// From: https://github.com/TheAlgorithms/C
i32 str_to_i32(char * string) {
    int i;
    int sign;
    long value;
    long prev;

    i = 0;
    sign = 1;
    value = 0;

    while (((string[i] <= 13 && string[i] >= 9) || string[i] == 32) && string[i] != '\0')
        i++;

    if (string[i] == '-') {
        sign = -1;
	    i++;
	}

    else if (string[i] == '+') {
        sign = 1;
	    i++;
	}

    while (string[i] >= 48 && string[i] <= 57 && string[i] != '\0') {
        prev = value;
        value = value * 10 + sign * (string[i] - '0');

        if (sign == 1 && prev > value)
            return (-1);
        else if (sign == -1 && prev < value)
            return (0);
        i++;
    }
    return (value); 
}


