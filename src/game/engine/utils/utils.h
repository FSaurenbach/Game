#ifndef UTILS_H
    #define UTILS_H
    #include <time.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <assert.h>
    #include <stdlib.h>
    #include <math.h>
    #include <stdarg.h>
    #include <stdint.h>
    #include <stddef.h>
    #include <string.h>

    typedef uint8_t  u08;
    typedef uint16_t u16;
    typedef uint32_t u32; 
    typedef uint64_t u64;

    typedef int8_t  i08;
    typedef int16_t i16; 
    typedef int32_t i32;
    typedef int64_t i64;

    typedef float f32;
    typedef double f64;

    #define i08_min -128
    #define i16_min -32768
    #define i32_min -2147483647 - 1
    #define i64_min -9223372036854775807LL - 1

    #define i08_max  127 
    #define i16_max  32767
    #define i32_max  2147483647
    #define i64_max  9223372036854775807LL

    #define u08_max  0xff
    #define u16_max  0xffff
    #define u32_max  0xffffffff
    #define u64_max  0xffffffffffffffffULL 

    typedef struct {
        char * content;
        const char * path;
        u32 len;
    } file_info_t;

    i08 file_exists(const char * path);
    file_info_t txt_file_query(const char * path);
    i08 txt_file_write(const char * content, const char * path, const size_t len); 

    typedef struct {
        const char * id;
        u64 hash;
        void * val;
    } ht_item_t;

    typedef struct {
        ht_item_t ** data;
        u32 len;
        u32 capacity;
    } ht_t; 

    ht_t ht_create(const u32 capacity);
    u64 ht_insert(ht_t * ht, const char * id, void * data); 
    void * ht_get(const ht_t * ht, const char * id);
    u64 ht_rm_data(ht_t * ht, const char * id);
    void ht_rm(ht_t * ht);

    typedef struct {
        void ** data;
        u32 capacity;
        u32 len;
    } list_t;

    list_t list_create(const u32 capacity);
    void list_insert(list_t * list, void * data);
    i32 list_get_idx_of(const list_t list, void * data);
    void list_rm_at(list_t * list, u32 idx);
    void list_rm_data(list_t * list, void * data);
    void list_rm(list_t * list);

    #define list_get_at(list, index) \
        list.data[index]

    #define list_get_data(list, data) \
        get_at_list(list, get_list_idx(&list, (void *)data))


    typedef enum {
        LOG_DBG,
        LOG_SUCCESS,
        LOG_WARN,
        LOG_ERR,
        LOG_LEVEL_LEN
    } LOG_LEVEL;

    void logger_log(LOG_LEVEL level, const char * fmt, ...);

    #define pi 3.141592653589793238462643383279
    #define deg_to_rad(deg) deg * (pi / 180)
    #define rad_to_deg(rad) rad * (180 / pi)
    #define sq(e) (e) * (e)

    f32 dist2D(f32 x1, f32 y1, f32 x2, f32 y2);
    f32 dir2D(f32 x1, f32 y1, f32 x2, f32 y2);
    f32 ilerp(f32 start, f32 stop, f32 amt);
    i64 randnum_gen(i64 lower, i64 upper);

    // Note projection functions 
    // project v1 onto v2

    typedef struct {
        f32 x;
        f32 y;
    } v2_t;

    #define mk_v2_zero() (v2_t)  { 0.0f, 0.0f }
    #define mk_v2_one () (v2_t)  { 1.0f, 1.0f }
    #define mk_v2(_x, _y) (v2_t) { (_x), (_y) }

    v2_t v2_add(v2_t v1, v2_t v2);
    v2_t v2_sub(v2_t v1, v2_t v2);
    v2_t v2_lerp(v2_t v1, v2_t v2, f32 amt);
    void v2_scale(v2_t * v, f32 scalar);
    void v2_normalize(v2_t * v);
    void v2_limit(v2_t * v, f32 scalar);
    void v2_rotate(v2_t * v, f32 theta);
    void v2_mag_set(v2_t * v, f32 scalar);
    void v2_negate(v2_t * v);

    f32 v2_mag(v2_t v);
    f32 v2_angle(v2_t v);
    f32 v2_dot_prod(v2_t v1, v2_t v2);
    f32 v2_dist_between(v2_t v1, v2_t v2);
    f32 v2_angle_between(v2_t v1, v2_t v2);
    v2_t v2_projection(v2_t v1, v2_t v2);

    typedef struct {
        char * name;
        f64 elapsed;
        clock_t start;
        clock_t end;
    } prof_t;

    void prof_start(prof_t * profiler, char * name);
    void prof_end(prof_t * profiler);


    // maybe custom string functions
    char * i32_to_str(i32 num);
    char * f64_to_str(f64 num);
    i32 str_to_i32(char * string);
#endif