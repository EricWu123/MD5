#include "md5.h"

/*分组长度，每个组512位，即64个字节*/
#define GROUP_LEN 64

typedef struct
{
    __uint32_t A;
    __uint32_t B;
    __uint32_t C;
    __uint32_t D;
} group_data, *p_group_data;

static __uint32_t F(__uint32_t X, __uint32_t Y, __uint32_t Z)
{
    return ((X & Y) | ((~X) & Z));
}

static __uint32_t G(__uint32_t X, __uint32_t Y, __uint32_t Z)
{
    return ((X & Z) | (Y & (~Z)));
}

static __uint32_t H(__uint32_t X, __uint32_t Y, __uint32_t Z)
{
    return (X ^ Y ^ Z);
}

static __uint32_t I(__uint32_t X, __uint32_t Y, __uint32_t Z)
{
    return (Y ^ (X | (~Z)));
}

static __uint32_t FF(__uint32_t a, __uint32_t b, __uint32_t c,
                     __uint32_t d, __uint32_t mj, __uint32_t s,
                     __uint32_t ti)
{
    __uint32_t tmp = a + F(b, c, d) + mj + ti;
    return b + ((tmp << s) | (tmp >> (32 - s)));
}

static __uint32_t GG(__uint32_t a, __uint32_t b, __uint32_t c,
                     __uint32_t d, __uint32_t mj, __uint32_t s,
                     __uint32_t ti)
{
    __uint32_t tmp = a + G(b, c, d) + mj + ti;
    return b + ((tmp << s) | (tmp >> (32 - s)));
}

static __uint32_t HH(__uint32_t a, __uint32_t b, __uint32_t c,
                     __uint32_t d, __uint32_t mj, __uint32_t s,
                     __uint32_t ti)
{
    __uint32_t tmp = a + H(b, c, d) + mj + ti;
    return b + ((tmp << s) | (tmp >> (32 - s)));
}

static __uint32_t II(__uint32_t a, __uint32_t b, __uint32_t c,
                     __uint32_t d, __uint32_t mj, __uint32_t s,
                     __uint32_t ti)
{
    __uint32_t tmp = a + I(b, c, d) + mj + ti;
    return b + ((tmp << s) | (tmp >> (32 - s)));
}

static void long2byte(__uint64_t x, __u_char *output)
{
    output[0] = x & 0xff;
    output[1] = (x >> 8) & 0xff;
    output[2] = (x >> 16) & 0xff;
    output[3] = (x >> 24) & 0xff;
    output[4] = (x >> 32) & 0xff;
    output[5] = (x >> 40) & 0xff;
    output[6] = (x >> 48) & 0xff;
    output[7] = (x >> 56) & 0xff;

    return;
}

/**
 * 对字符串进行填充
 * str: 要填充的字符串
 * len: 字符串长度
 * total_len: 整个文件长度
 * output: 填充后的字符串
 * outlen: 填充后的字符串长度
*/
static void padding(__u_char *str, size_t len, size_t total_len, __u_char *output, size_t *outlen)
{
    /*计算余数*/
    size_t remainder = (len * 8) % 512;
    size_t part_len = (remainder >= 448 ? 512 + 448 - remainder : 448 - remainder) / 8;
    memcpy(output, str, len);
    /*填充1000..00*/
    __u_char c = 0x80;
    memcpy(output + len, &c, 1);
    memset(output + len + 1, 0, part_len - 1);
    /*最后64位填充字符串的长度*/
    __uint64_t _len = total_len * 8;
    __u_char len_byte[sizeof(__uint64_t)] = {0};
    long2byte(_len, len_byte);
    memcpy(output + len + part_len, len_byte, sizeof(len_byte));
    *outlen = len + part_len + sizeof(len_byte);
    return;
}

static void int2byte(__uint32_t x, __u_char *output)
{
    output[0] = x & 0xff;
    output[1] = (x >> 8) & 0xff;
    output[2] = (x >> 16) & 0xff;
    output[3] = (x >> 24) & 0xff;
    return;
}

static __uint32_t byte2int(__u_char *input)
{
    __uint32_t output = 0;
    memcpy(&output, input, 4);
    return output;
}

static void group(__u_char *str, __uint32_t *_a, __uint32_t *_b, __uint32_t *_c, __uint32_t *_d)
{
    __uint32_t a = *_a;
    __uint32_t b = *_b;
    __uint32_t c = *_c;
    __uint32_t d = *_d;
    __uint32_t mj[16] = {0};
    for (size_t i = 0; i < GROUP_LEN; i += 4)
    {
        mj[i / 4] = byte2int(&str[i]);
    }
    a = FF(a, b, c, d, mj[0], 7, 0xd76aa478);
    d = FF(d, a, b, c, mj[1], 12, 0xe8c7b756);
    c = FF(c, d, a, b, mj[2], 17, 0x242070db);
    b = FF(b, c, d, a, mj[3], 22, 0xc1bdceee);
    a = FF(a, b, c, d, mj[4], 7, 0xf57c0faf);
    d = FF(d, a, b, c, mj[5], 12, 0x4787c62a);
    c = FF(c, d, a, b, mj[6], 17, 0xa8304613);
    b = FF(b, c, d, a, mj[7], 22, 0xfd469501);
    a = FF(a, b, c, d, mj[8], 7, 0x698098d8);
    d = FF(d, a, b, c, mj[9], 12, 0x8b44f7af);
    c = FF(c, d, a, b, mj[10], 17, 0xffff5bb1);
    b = FF(b, c, d, a, mj[11], 22, 0x895cd7be);
    a = FF(a, b, c, d, mj[12], 7, 0x6b901122);
    d = FF(d, a, b, c, mj[13], 12, 0xfd987193);
    c = FF(c, d, a, b, mj[14], 17, 0xa679438e);
    b = FF(b, c, d, a, mj[15], 22, 0x49b40821);

    a = GG(a, b, c, d, mj[1], 5, 0xf61e2562);
    d = GG(d, a, b, c, mj[6], 9, 0xc040b340);
    c = GG(c, d, a, b, mj[11], 14, 0x265e5a51);
    b = GG(b, c, d, a, mj[0], 20, 0xe9b6c7aa);
    a = GG(a, b, c, d, mj[5], 5, 0xd62f105d);
    d = GG(d, a, b, c, mj[10], 9, 0x2441453);
    c = GG(c, d, a, b, mj[15], 14, 0xd8a1e681);
    b = GG(b, c, d, a, mj[4], 20, 0xe7d3fbc8);
    a = GG(a, b, c, d, mj[9], 5, 0x21e1cde6);
    d = GG(d, a, b, c, mj[14], 9, 0xc33707d6);
    c = GG(c, d, a, b, mj[3], 14, 0xf4d50d87);
    b = GG(b, c, d, a, mj[8], 20, 0x455a14ed);
    a = GG(a, b, c, d, mj[13], 5, 0xa9e3e905);
    d = GG(d, a, b, c, mj[2], 9, 0xfcefa3f8);
    c = GG(c, d, a, b, mj[7], 14, 0x676f02d9);
    b = GG(b, c, d, a, mj[12], 20, 0x8d2a4c8a);

    // 第三轮运算HH
    a = HH(a, b, c, d, mj[5], 4, 0xfffa3942);
    d = HH(d, a, b, c, mj[8], 11, 0x8771f681);
    c = HH(c, d, a, b, mj[11], 16, 0x6d9d6122);
    b = HH(b, c, d, a, mj[14], 23, 0xfde5380c);
    a = HH(a, b, c, d, mj[1], 4, 0xa4beea44);
    d = HH(d, a, b, c, mj[4], 11, 0x4bdecfa9);
    c = HH(c, d, a, b, mj[7], 16, 0xf6bb4b60);
    b = HH(b, c, d, a, mj[10], 23, 0xbebfbc70);
    a = HH(a, b, c, d, mj[13], 4, 0x289b7ec6);
    d = HH(d, a, b, c, mj[0], 11, 0xeaa127fa);
    c = HH(c, d, a, b, mj[3], 16, 0xd4ef3085);
    b = HH(b, c, d, a, mj[6], 23, 0x4881d05);
    a = HH(a, b, c, d, mj[9], 4, 0xd9d4d039);
    d = HH(d, a, b, c, mj[12], 11, 0xe6db99e5);
    c = HH(c, d, a, b, mj[15], 16, 0x1fa27cf8);
    b = HH(b, c, d, a, mj[2], 23, 0xc4ac5665);

    // 第四轮运算II
    a = II(a, b, c, d, mj[0], 6, 0xf4292244);
    d = II(d, a, b, c, mj[7], 10, 0x432aff97);
    c = II(c, d, a, b, mj[14], 15, 0xab9423a7);
    b = II(b, c, d, a, mj[5], 21, 0xfc93a039);
    a = II(a, b, c, d, mj[12], 6, 0x655b59c3);
    d = II(d, a, b, c, mj[3], 10, 0x8f0ccc92);
    c = II(c, d, a, b, mj[10], 15, 0xffeff47d);
    b = II(b, c, d, a, mj[1], 21, 0x85845dd1);
    a = II(a, b, c, d, mj[8], 6, 0x6fa87e4f);
    d = II(d, a, b, c, mj[15], 10, 0xfe2ce6e0);
    c = II(c, d, a, b, mj[6], 15, 0xa3014314);
    b = II(b, c, d, a, mj[13], 21, 0x4e0811a1);
    a = II(a, b, c, d, mj[4], 6, 0xf7537e82);
    d = II(d, a, b, c, mj[11], 10, 0xbd3af235);
    c = II(c, d, a, b, mj[2], 15, 0x2ad7d2bb);
    b = II(b, c, d, a, mj[9], 21, 0xeb86d391);

    *_a = a;
    *_b = b;
    *_c = c;
    *_d = d;
    return;
}

static void convert2result(p_group_data data, char *output)
{
    __u_char result[16] = {0};
    int2byte(data->A, result);
    int2byte(data->B, result + 4);
    int2byte(data->C, result + 8);
    int2byte(data->D, result + 12);

    for (size_t i = 0; i < 16; i++)
    {
        sprintf(&(output[2 * i]), "%02x", result[i]);
    }
    output[32] = '\0';
    return;
}

static void update_group_data(p_group_data data, __u_char *str, size_t len)
{
    __uint32_t A = data->A;
    __uint32_t B = data->B;
    __uint32_t C = data->C;
    __uint32_t D = data->D;

    for (size_t i = 0; i < len; i += GROUP_LEN)
    {
        __uint32_t a = A;
        __uint32_t b = B;
        __uint32_t c = C;
        __uint32_t d = D;
        group(str + i, &a, &b, &c, &d);
        A = a + A;
        B = b + B;
        C = c + C;
        D = d + D;
    }
    data->A = A;
    data->B = B;
    data->C = C;
    data->D = D;
    return;
}

/**
 * 计算字符串的md5值
 * str: 需要计算md5值的字符串
 * len: 字符串长度
 * output: 计算后的md5值16进制字符串，长度为32个字节。
 * out_max_len: output可容纳的字符串长度
 * 返回值：成功返回0，失败返回-1
 */
int md5(const char *str, size_t len, char *output, size_t out_max_len)
{
    if (str == NULL || output == NULL || out_max_len <= 32)
    {
        return -1;
    }
    /*当字符串的长度对512的余数为448时，字符串扩充后最大增大72个长度*/
    __u_char *str_padding = (__u_char *)calloc(len + 72, sizeof(__u_char));
    if (str_padding == NULL)
    {
        return -1;
    }
    size_t len_padding = 0;
    padding((__u_char *)str, len, len, str_padding, &len_padding);
    group_data data = {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476,
    };
    update_group_data(&data, str_padding, len_padding);
    convert2result(&data, output);
    free(str_padding);
    return 0;
}

/**
 * 计算文件的md5值
 * str: 需要计算md5值的文件的路径
 * output: 计算后的md5值16进制字符串，长度为32个字节。
 * out_max_len: output可容纳的字符串长度
 * 返回值：成功返回0，失败返回-1
 */
int md5_file(const char *file_path, char *output, size_t out_max_len)
{
    if (file_path == NULL || output == NULL || out_max_len <= 32)
    {
        return -1;
    }
    FILE *fp = stdin;
    if (strlen(file_path) > 0)
    {
        fp = fopen(file_path, "rb");
    }
    if (fp == NULL)
    {
        return -1;
    }
    group_data data = {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476,
    };

    __u_char buff[GROUP_LEN] = {0};
    size_t file_len = 0;
    size_t read_len = 0;
    while (!feof(fp))
    {
        memset(buff, 0, sizeof(buff));
        read_len = fread(buff, 1, GROUP_LEN, fp);
        if (ferror(fp))
        {
            fclose(fp);
            return -1;
        }
        file_len += read_len;
        if (read_len < GROUP_LEN)
        {
            __u_char buff_padding[read_len + 72];
            memset(buff_padding, 0, sizeof(buff_padding));
            size_t len_buff = 0;
            padding(buff, read_len, file_len, buff_padding, &len_buff);
            update_group_data(&data, buff_padding, len_buff);
            break;
        }

        update_group_data(&data, buff, GROUP_LEN);
    }
    convert2result(&data, output);
    fclose(fp);
    return 0;
}
