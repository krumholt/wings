#ifndef _wings_strings_c_
#define _wings_strings_c_

#include "memory.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

struct string
{
    char *base;
    s32   size;
};

struct token
{
    struct string string;
    b32   is_incomplete_string;
    b32   is_float;
    b32   is_integer;
    b32   is_newline;
    b32   is_empty;
};

enum string_to_f32_error
{
    string_to_f32_no_error,
    string_to_f32_illegal_character,
    string_to_f32_empty_string,
};

enum string_to_s32_error
{
    string_to_s32_no_error,
    string_to_s32_illegal_character,
    string_to_s32_empty_string,
};


int
is_digit(char c)
{
    return c >= '0' && c <= '9';
}

int
is_whitespace(char c)
{
    return((c >= 9 && c <= 13) || c == ' ');
}

char *
end_of_cstring(char *s)
{
    char *tmp = s;
    while(*tmp) ++tmp;
    return(tmp);
}

s32
split_cstring(char *s, char c, struct string *left, struct string *right)
{
    s32 count = 0;
    s32 left_size = 0;
    left->base = s;
    while(*s)
    {
        count += 1;
        if (*s == c)
        {
            left_size = count;
        }
        s += 1;
    }
    if (left_size == 0)
        left_size = count;

    left->size  = left_size - 1;
    right->base = left->base + left_size;
    right->size = count - left_size;
    if (right->size == -1) right->size = 0;
    return left_size;
}

b32
compare_string_cstring(struct string a, char *b)
{
    for (int index = 0; index < a.size && *b; ++index, ++b)
    {
        if (a.base[index] != *b)
            return 1;
    }
    return 0;
}

enum string_to_f32_error
string_to_f32(char *string, s32 string_size, f32 *result)
{
    if (string_size == 0)
        return string_to_f32_empty_string;
    *result = 0.0f;
    s32 index = 0;
    f32 sign = 1.0f;
    if (string[0] == '+')
    {
        index += 1;
    }
    else if (string[0] == '-')
    {
        sign = -1.0f;
        index += 1;
    }
    for (; index < string_size; ++index)
    {
        char c = string[index];
        if (c == '.')
        {
            index += 1; // eat the dot
            break;
        }
        if (!is_digit(c))
            return string_to_f32_illegal_character;
        *result *= 10.0f;
        *result += (c - '0');
    }
    f32 after_comma_result = 0.0f;
    f32 base = 0.1f;
    for (; index < string_size; ++index)
    {
        char c = string[index];
        if (!is_digit(c))
            return string_to_f32_illegal_character;
        after_comma_result += (c - '0') * base;
        base *= 0.1f;
    }
    *result += after_comma_result;
    *result *= sign;
    return string_to_f32_no_error;
}


enum string_to_s32_error
string_to_s32(char *string, s32 string_size, s32 *result)
{
    if (string_size == 0)
        return string_to_s32_empty_string;
    s32 sign = 1;
    s32 index = 0;
    if (string[0] == '+')
    {
        index++;
    }
    else if (string[0] == '-')
    {
        index++;
        sign = -1;
    }
    *result = 0;
    for (; index < string_size; ++index)
    {
        char c = string[index];
        if (!is_digit(c)) return string_to_s32_illegal_character;
        *result *= 10;
        *result += c - '0';
    }

    *result = *result * sign;
    return string_to_s32_no_error;
}

/*
void
replace(struct string target, char old_char, char new_char)
{
    for (s32 index = 0; index < target.size; ++index)
    {
        if (target.memory[index] == old_char)
            target.memory[index] = new_char;
    }
}


*/




/*
// @Todo: this whole method does not work!
// it does strange allocations and generally hopes for the best
// it should not allocate at first and then allocate as needed
// from the block. only failing if the block is full
struct string
_string_print(struct string format, va_list list, int zero_terminated, struct memory *block)
{
    memory_save_state_t save = save_memory_state(block);
    uint64 max_result_size = memory_free(*block); // allocate everything left because we don't know how much we need
    char *result = allocate_array(block, max_result_size, char);
    uint64 result_size = 0;
    for (int i = 0; i < format.size; ++i)
    {
        while(i < format.size && format.memory[i] != '%')
        {
            result[result_size++] = format.memory[i];
            if (result_size == max_result_size)
            {
                break;
            }
            ++i;
        }
        if (i == format.size) // there was no % in the whole string
        {
            break;
        }
        ++i;
        if (i == format.size)
            log_and_exit("[error] illegal format string");
        switch(format.memory[i])
        {
        case '-': {
            ++i;
        } break;
        case '+': {
            ++i;
        } break;
        case ' ': {
            ++i;
        } break;
        case '#': {
            ++i;
        } break;
        case '0': {
            ++i;
        } break;
        default: {
            // no flag
        }
        }
        if (i == format.size)
            log_and_exit("[error] illegal format string");
        int output_length = 10;
        if (format.memory[i] >= '0' && format.memory[i] <= '9')
        { // number of characters to be printed @Todo: think about supporting *
            int start = i;
            while(format.memory[i] >= '0' && format.memory[i] <= '9')
            {
                ++i;
            }
            struct string s = {format.memory+start, i-start};
            string_to_s32(s, &output_length);
        }
        int precision = 10;
        if(format.memory[i] == '.')
        {
            ++i;
            int start = i;
            while(format.memory[i] >= '0' && format.memory[i] <= '9')
            {
                ++i;
            }
            struct string s = {format.memory+start, i-start};
            string_to_s32(s, &precision);
        }
            
        switch(format.memory[i])
        {
        case 'f':{
            double f = va_arg(list, double);
            int chars_written = snprintf(result+result_size, max_result_size - result_size, "%*.*f", output_length, precision, f);
            result_size += chars_written;
            if (result_size == max_result_size)
                break;
        } break;
        case 'g':{
            double f = va_arg(list, double);
            int chars_written = snprintf(result+result_size, max_result_size - result_size, "%g", f);
            result_size += chars_written;
            if (result_size == max_result_size)
                break;
        } break;
        case 'd': {
            int d = va_arg(list, int);
            int chars_written = snprintf(result+result_size, max_result_size - result_size, "%d", d);
            result_size += chars_written;
            if (result_size == max_result_size)
                break;
        } break;
        case 's': {
            char *s = va_arg(list, char *);
            int chars_written = snprintf(result+result_size, max_result_size - result_size, "%s", s);
            result_size += chars_written;
            if (result_size == max_result_size)
                break;
        } break;
        case 'c': {
            int c = va_arg(list, int);
            int chars_written = snprintf(result+result_size, max_result_size - result_size, "%c", c);
            result_size += chars_written;
            if (result_size == max_result_size)
                break;
        } break;
        case 'w': {
            struct string w = va_arg(list, struct string);
            for (int w_index = 0; w_index < w.size; ++w_index)
            {
                result[result_size++] = w.memory[w_index];
                if (result_size == max_result_size)
                    break;
            }
        } break;
        default: {
            log_and_exit("[error] illegal format specifier");
        }
        }
    }
    if (zero_terminated)
    {
        result[result_size++] = 0;
    }
    reset_to_save_state_dont_zero(save);
    allocate_array(block, result_size, char); // allocate only what was acutally used
    struct string s = {result, (s32)result_size - (zero_terminated?1:0)};
    return(s);
}

struct string
string_print(struct memory *block, struct string format, ...)
{
    va_list list;
    va_start(list, format);
    struct string result = _string_print(format, list, 0, block);
    va_end(list);
    return(result);
}

struct string
string_print(struct memory *block, char *format, ...)
{
    va_list list;
    va_start(list, format);
    struct string format_as_string = string(format, block);
    struct string result = _string_print(format_as_string, list, 0, block);
    va_end(list);
    return(result);
}

struct string
string_print_z(struct memory *block, char *format, ...)
{
    va_list list;
    va_start(list, format);
    struct string format_as_string = string(format, block);
    struct string result = _string_print(format_as_string, list, 1, block);
    va_end(list);
    return(result);
}

struct string
string_print_z(struct memory *block, struct string format, ...)
{
    va_list list;
    va_start(list, format);
    struct string result = _string_print(format, list, 1, block);
    va_end(list);
    return(result);
}
*/

/*
int
compare_strings(struct string a, struct string b)
{
    if (a.size != b.size) return 1;
    for (int index = 0; index < a.size; ++index)
    {
        if (a.memory[index] != b.memory[index])
            return 1;
    }
    return 0;
}



int
compare_cstring_string(char *b, struct string a)
{
    return(compare_string_cstring(a, b));
}

bool32
compare_cstring_cstring(char *a, char  *b)
{
    while(*a && *b && *a++ == *b++);
    return *a != *b;
}
*/

char
is_one_of(char c, char *c_string)
{
    if (!c_string) return 0;
    while(*c_string)
    {
        if (*c_string == c)
            return c;
        ++c_string;
    }
    return 0;
}

b32
cstring_ends_with(char *haystack, s32 haystack_size, char *needle, s32 needle_size)
{
    if (needle_size > haystack_size || needle_size == 0) return 0;
    while(needle_size)
    {
        --needle_size;
        --haystack_size;
        if(haystack[haystack_size] != needle[needle_size]) return 0;
    }
    return 1;
}

/*

int
begins_with_string_string(struct string a, struct string b)
{
    if (a.size < b.size) return 0;
    for (int index = 0; index < b.size; ++index)
    {
        if (a.memory[index] != b.memory[index])
            return 0;
    }
    return 1;
}

int
empty(struct string s)
{
    return s.size == 0;
}
*/

b32
begins_with_cstring(char *a, u64 size_a, char *b, u64 size_b)
{
    for(u64 index = 0; index < size_a && index < size_b; index += 1)
    {
        if (a[index] != b[index])
            return 0;
    }
    return 1;
}




struct string
next_word(struct string *context)
{
    struct string word = *context;
    word.size = 0;
    for (int at = 0; at < context->size; ++at)
    {
        if (is_whitespace(context->base[at]))
        {
            context->size -= (word.size + 1);
            context->base += (word.size + 1);
            while(context->size > 0 && is_whitespace(context->base[0]))
            {
                context->size -= 1;
                context->base += 1;
            }
            return(word);
        }
        word.size++;
    }
    context->base += context->size;
    context->size   = 0;
    return(word);
}


s32
is_token_separator(char c)
{
    return 0
        || c == ','
        || c == '/'
        || c == '|'
        || c == ' ';
}


struct string
next_line(struct string *context)
{
    int line_size = 0;
    struct string line = {0};
    line.base = context->base;
    while(context->size)
    {
        if (context->size >= 2
            && ((context->base[0] == '\n' && context->base[1] == '\r') ||
                (context->base[0] == '\r' && context->base[1] == '\n')))
        {
            line.size   = line_size;
            context->size -= 2;
            context->base += 2;
            return(line);
        }
        else if (context->size >= 1 && (context->base[0] == '\n' || context->base[0] == '\r'))
        {
            line.size   = line_size;
            context->size -= 1;
            context->base += 1;
            return(line);
        }
        context->size -= 1;
        context->base += 1;
        line_size += 1;
    }
    line.size   = line_size;
    context->base += line_size;
    context->size = 0;
    return(line);
}

typedef int (*seperator_fn)(char);

struct token
next_token_with_separator(struct string *context, seperator_fn is_token_separator)
{
    struct token token = {0};
    if (context->size == 0) {
        token.is_empty = 1;
        return(token);
    }
    while (is_token_separator(context->base[0]))
    {
        context->size -= 1;
        context->base += 1;
        if (context->size == 0)
        {
            token.string.size = 0;
            return token;
        }
    }
    token.string = *context;
    token.string.size = 0;
    if (context->size >= 2
        && ((context->base[0] == '\n' && context->base[1] == '\r') ||
            (context->base[0] == '\r' && context->base[1] == '\n')))
    {
        token.is_newline  = 1;
        token.string.size = 2;
        context->size -= 2;
        context->base += 2;
        return(token);
    }
    else if (context->size >= 1 && (context->base[0] == '\n' || context->base[0] == '\r'))
    {
        token.is_newline  = 1;
        token.string.size = 1;
        context->size -= 1;
        context->base += 1;
        return(token);
    }
    else if (
        context->size >= 1 && (
            context->base[0] == ':' ||
            context->base[0] == '=' ||
            context->base[0] == ';'))
             
    {
        token.string.size = 1;
        context->size -= 1;
        context->base += 1;
        return(token);
    }
    else if (context->size && context->base[0] == '\"')
             
    {
        token.string.base += 1;
        context->size     -= 1;
        context->base     += 1;
        while(context->size && context->base[0] != '\"')
        {
            context->size     -= 1;
            context->base     += 1;
            token.string.size += 1;
        }
        if (context->size)
        {
            context->size -= 1;
            context->base += 1;
        }
        else
        {
            token.is_incomplete_string = 1;
        }
        return(token);
    }
    bool32 only_numbers = 1;
    bool32 contains_dot = 0;
    for (int at = 0; at < context->size; ++at)
    {
        if (is_token_separator(context->base[at]) ||
            context->base[at] == '\n' ||
            context->base[at] == '\r')
        {
            if (only_numbers)
            {
                if (contains_dot == 1)
                    token.is_float = 1;
                else if (contains_dot == 0)
                    token.is_integer = 1;
            }
            context->size   -= (token.string.size);
            context->base += (token.string.size);
            return(token);
        }
        if (context->base[at] == '.')
        {
            contains_dot += 1;
        }
        else if (!is_digit(context->base[at]))
        {
            only_numbers = 0;
        }
        token.string.size++;
    }
    if (only_numbers)
    {
        if (contains_dot == 1)
            token.is_float = 1;
        else if (contains_dot == 0)
            token.is_integer = 1;
    }
    context->base += context->size;
    context->size   = 0;
    return(token);
}

struct token
next_token(struct string *context)
{
    return(next_token_with_separator(context, is_token_separator));
}

void
ws_string_copy(char *target, s32 target_size, char *source, s32 source_size)
{
    while(*target && *source && target_size && source_size)
    {
        *target = *source;
        target += 1;
        source += 1;
        target_size -= 1;
        source_size -= 1;
    }
}


struct token
skip_newlines(struct string *context, s32 *lines_skiped)
{
    struct token token = next_token(context); 
    while(token.is_newline)
    {
        if (lines_skiped)
            *lines_skiped += 1;
        token = next_token(context);
    }
    return(token);
}

#endif
