    
#include "csv.h"
#include <stdio.h>
#include <stdlib.h>

csv* csv_create()
{
    csv* c = (csv*)malloc(sizeof(csv));
    csv_init(c);
    return c;
}

void csv_init(csv* c)
{
    c->path = (kstring_t*)calloc(1, sizeof(kstring_t));
    kv_init(c->data);
    c->row_count = 0;
    c->col_count = 0;
}

void csv_destroy(csv* c)
{
    csv_clear(c);
    free(c);
}

void csv_clear(csv* c)
{
    if (c->path)
    {
        if (c->path->s) free(c->path->s);
        free(c->path);
        c->path = 0;
    }

    for (int row = 0; row < kv_size(c->data); ++row)
    {
        csvrow* rowdata = kv_A(c->data, row);
        for (int col = 0; col < kv_size(*rowdata); ++col)
        {
            kstring_t* data = kv_A(*rowdata, col);
            if (data)
            {
                if (data->s) free(data->s);
                free(data);
            }
        }
        kv_destroy(*rowdata);
        free(rowdata);
    }
    kv_destroy(c->data);
    kv_init(c->data);
    c->row_count = 0;
    c->col_count = 0;
}

#define END_OF_FILE 0

int csv_addrow(csv* c, csvrow* rowdata, kstring_t* error)
{
    if (c->col_count == 0)
    {
        c->col_count = (int)kv_size(c->data);
        kv_push(csvrow*, c->data, rowdata);
        return 1;
    }

    if (kv_size(*rowdata) != c->col_count)
    {
        if (error) ksprintf(error, "row(%d) col(%d) != %d:%s", c->row_count + 1, kv_size(*rowdata), c->path->s);
        return 0;
    }

    kv_push(csvrow*, c->data, rowdata);
    return 1;
}

int csv_load(csv* c, const char* path, kstring_t* error)
{
    FILE* file = 0;
#ifdef _WIN32
    fopen_s(&file, path, "rb");
#else
    file = fopen(path, "rb");
#endif
    if (file == 0)
    {
        if (error) ksprintf(error, "file open failed:%s", path);
        return 0;
    }

    // ��ȡ��һ���ַ�
    char cur_char = END_OF_FILE;
    size_t size = fread(&cur_char, 1, 1, file);
    if (size == 0)
    {
        if (error) ksprintf(error, "file is empty:%s", path);
        fclose(file);
        return 0;
    }
    if (cur_char == 0)
    {
        if (error) ksprintf(error, "row(1) has char is 0:%s", path);
        fclose(file);
        return 0;
    }

    // ����·��
    kputs(path, c->path);

    // ��ȡ��һ���ַ�
    char next_char = 0;
    size = fread(&next_char, 1, 1, file);
    if (size == 0)
    {
        // �����ַ���
        kstring_t* data = (kstring_t*)calloc(1, sizeof(kstring_t));
        kputc(cur_char, data);
        
        // ����������
        csvrow* rowdata = (csvrow*)malloc(sizeof(csvrow));
        kv_init(*rowdata);
        kv_push(kstring_t*, *rowdata, data);

        // ���ӵ�������
        kv_push(csvrow*, c->data, rowdata);

        fclose(file);
        return 1;
    }

    if (next_char == 0)
    {
        if (error) ksprintf(error, "row(1) has char is 0:%s", path);
        fclose(file);
        return 0;
    }

    // ���
    int result = 1;

    // ��ǵ�ǰ�Ƿ���������
    int in_quote = 0;

    // ��ǰ������
    csvrow* row = (csvrow*)malloc(sizeof(csvrow));
    kv_init(*row);
    // ��ǰ������
    kstring_t* cell = (kstring_t*)calloc(1, sizeof(kstring_t));

    while (1) {
        // ����Ƿָ���
        if (cur_char == ',')
        {
            // ����������У���ô��Ҫ���ӵ���ǰ���ַ�
            if (in_quote)
            {
                // ���ӵ���
                kputc(cur_char, cell);

                // �������û�������ˣ�˵��û�йر����ţ�����
                if (next_char == END_OF_FILE)
                {
                    if (error) ksprintf(error, "row(%d) have no close quote:%s", c->row_count, path);
                    result = 0;
                    break;
                }
            }
            // ���û���������У���ô��Ҫ���и�
            else
            {
                // ���ӵ���
                kv_push(kstring_t*, *row, cell);
                kstring_t* cell = (kstring_t*)calloc(1, sizeof(kstring_t));

                // �������û��������
                if (next_char == END_OF_FILE)
                {
                    // ����һ������
                    kv_push(kstring_t*, *row, (kstring_t*)calloc(1, sizeof(kstring_t)));
                    // ���ӵ���
                    if (!csv_addrow(c, row, error))
                    {
                        result = 0;
                        break;
                    }
                    // ����һ���µ�
                    csvrow* row = (csvrow*)malloc(sizeof(csvrow));
                    kv_init(*row);
                }
            }
        }
        // ����ǻ���
        else if (cur_char == '\n')
        {
            // ����������У���ô��Ҫ���ӵ���ǰ���ַ�
            if (in_quote)
            {
                // ���ӵ���
                kputc(cur_char, cell);

                // �������û�������ˣ�˵��û�йر����ţ�����
                if (next_char == END_OF_FILE)
                {
                    if (error) ksprintf(error, "row(%d) have no close quote:%s", c->row_count, path);
                    result = 0;
                    break;
                }
            }
            // ���û���������У���ô��Ҫ���и�
            else
            {
                // ���ӵ���
                kv_push(kstring_t*, *row, cell);
                kstring_t* cell = (kstring_t*)calloc(1, sizeof(kstring_t));

                // ���ӵ���
                if (!csv_addrow(c, row, error))
                {
                    result = 0;
                    break;
                }
                // ����һ���µ�
                csvrow* row = (csvrow*)malloc(sizeof(csvrow));
                kv_init(*row);
            }
        }
        // ���������
        else if (cur_char == '"')
        {
            // �����������
            if (in_quote)
            {
                // �����һ��Ҳ�����ţ���ô��ǰֱ��ת��Ϊ����
                if (next_char == '"')
                {
                    // ���ӵ���
                    kputc('"', cell);
                    // ֱ�Ӷ�ȡ��һ���ַ�
                    next_char = END_OF_FILE;
                    fread(&next_char, 1, 1, file);
                    // �������û�������ˣ�˵��û�йر����ţ�����
                    if (next_char == END_OF_FILE)
                    {
                        if (error) ksprintf(error, "row(%d) have no close quote:%s", c->row_count, path);
                        result = 0;
                        break;
                    }
                }
                // �����һ���������ţ���ô�͹ر�����
                else
                {
                    in_quote = 0;

                    if (next_char == END_OF_FILE)
                    {
                        // ���ӵ���
                        kv_push(kstring_t*, *row, cell);
                        kstring_t* cell = (kstring_t*)calloc(1, sizeof(kstring_t));

                        // ���ӵ���
                        if (!csv_addrow(c, row, error))
                        {
                            result = 0;
                            break;
                        }
                        // ����һ���µ�
                        csvrow* row = (csvrow*)malloc(sizeof(csvrow));
                        kv_init(*row);
                    }
                }
            }
            // ���������У�ֱ�ӱ��Ϊ��������
            else
            {
                // �����ǰ���ǿյģ���ô�ͱ�����ſ�ʼ
                if (ks_len(cell) == 0)
                {
                    in_quote = 1;
                }
                // �����ǰ�Ѿ��������ˣ���ôֱ�ӵ�����ͨ�ַ�
                else
                {
                    kputc(cur_char, cell);
                }

                // �������û�������ˣ�˵��û�йر����ţ�����
                if (next_char == END_OF_FILE)
                {
                    if (error) ksprintf(error, "row(%d) have no close quote:%s", c->row_count, path);
                    result = 0;
                    break;
                }
            }
        }
        // �����ַ�ֱ������
        else
        {
            if (in_quote)
            {
                // �����������ַ�
                kputc(cur_char, cell);

                // �������û�������ˣ�˵��û�йر����ţ�����
                if (next_char == END_OF_FILE)
                {
                    if (error) ksprintf(error, "row(%d) have no close quote:%s", c->row_count, path);
                    result = 0;
                    break;
                }
            }
            else
            {
                // ���ӵ���
                if (!(cur_char == '\r' && next_char == '\n')) {
                    kputc(cur_char, cell);
                }

                // �������û��������
                if (next_char == END_OF_FILE) {
                    // ���ӵ���
                    kv_push(kstring_t*, *row, cell);
                    kstring_t* cell = (kstring_t*)calloc(1, sizeof(kstring_t));

                    // ���ӵ���
                    if (!csv_addrow(c, row, error))
                    {
                        result = 0;
                        break;
                    }
                    // ����һ���µ�
                    csvrow* row = (csvrow*)malloc(sizeof(csvrow));
                    kv_init(*row);
                }
            }

        }

        // ���û���¸��ַ��ˣ�˵��������
        if (next_char == END_OF_FILE) {
            break;
        }

        // ��ȡ��һ���ַ�
        cur_char = next_char;
        next_char = END_OF_FILE;
        fread(&next_char, 1, 1, file);
    }

    if (cell)
    {
        if (cell->s) free(cell->s);
        free(cell);
    }

    if (row)
    {
        kv_destroy(*row);
        free(row);
    }

    fclose(file);
    return result;
}

const char* csv_getpath(csv* c)
{
    return ks_str(c->path);
}

const char* csv_readcell(csv* c, int row, int col)
{
    if (row >= kv_size(c->data)) return 0;
    csvrow* rowdata = kv_A(c->data, row);
    if (rowdata == 0) return 0;
    if (col >= kv_size(*rowdata)) return 0;
    return ks_str(kv_A(*rowdata, col));
}

int csv_rowcount(csv* c)
{
    return c->row_count;
}

int csv_colcount(csv* c)
{
    return c->col_count;
}