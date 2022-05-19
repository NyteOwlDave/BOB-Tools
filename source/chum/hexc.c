
/*

    hexc.c

    HEXC file format

*/

#include <ctype.h>
#include "inc/stdafx.h"
#include "inc/hexc.h"


#define MAX_LINE 1024

#define TOKEN_ERROR     (-1)
#define TOKEN_EOF       (0)
#define TOKEN_COMMENT   (1)
#define TOKEN_KEYWORD   (2)


static char m_line[MAX_LINE+1];
static char m_token[MAX_LINE+1];


typedef int token_t;


// Tokenizer
// Read next line
static BOOL line_read(FILE* fp)
{
    Log("Line Read");
    BOOL ok = (fgets(m_line, MAX_LINE, fp) ? TRUE : FALSE);
    if (ok)
    {
        // Convert whitespace to all BLANK spaces
        LPSTR pos = m_line;
        while (*pos)
        {
            if (isspace(*pos))
            {
                if (*pos == '\n')
                {
                    *pos = 0;
                }
                else
                {
                    *pos = 32;
                }
            }
            pos++;
        }
        Log(m_line);
    }
    else
    {
        Log("Failed");
    }
    return ok;
}

// Tokenizer
// Read token
static token_t token_read(FILE* fp, LPCSTR* ppos)
{
    static LPCSTR szMaxToken = "Exceeded maximum token length";
    int out;
    LPCSTR pos = *ppos;
    
    Log("Token Read");

    // Start with empty token
    m_token[0] = 0;

    // Skip whitespace
    if (isspace(*pos))
    {
        Log("Skip whitespace");
        while (isspace(*pos)) 
        {
            pos++;
        }
    }
    
    // Refresh line if buffer empty
    if (*pos == 0)
    {
        Log("Detected EOL");
        if (!line_read(fp))
        {
            if (feof(fp))
            {
                Log("EOF");
                return TOKEN_EOF;
            }
            Log("Error");
            return TOKEN_ERROR;
        }
        *ppos = m_line;
        return token_read(fp, ppos);
    }

    // Capture comment
    if (*pos == '#')
    {
        Log("Capture comment");
        out = 0;
        while (*pos)
        {
            if (out > MAX_LINE)
            {
                Log(szMaxToken);
                out = MAX_LINE;
                break;
            }
            m_token[out++] = *pos;
            pos++;
        }
        m_token[out] = 0;
        Log("Comment");
        Log(m_token);
        *ppos = pos;
        return TOKEN_COMMENT;
    }

    // Capture keyword
    Log("Capture keyword");
    out = 0;
    while ((*pos) && !isspace(*pos))
    {
        if (out > MAX_LINE)
        {
            Log(szMaxToken);
            out = MAX_LINE;
            break;
        }
        m_token[out++] = *pos;
        pos++;
    }
    m_token[out] = 0;
    Log("Keyword");
    Log(m_token);
    *ppos = pos;
    return TOKEN_KEYWORD;
}


// Read in palette
static void read_colors(FILE* fp, PRGBPALETTE pal)
{
    int n=0, i;
    RGBColor color;
    LPCSTR ptr = m_line;
    while (n<256)
    {
        token_t tok = token_read(fp, &ptr);
        switch (tok)
        {
        case TOKEN_KEYWORD:
            sscanf(m_token, "0x%X", &i);
            color.a = 0;
            color.r = (i >> 16) & 0xFF;
            color.g = (i >> 8) & 0xFF;
            color.b = (i) & 0xFF;
            pal->color[n++] = color;
            continue;
        case TOKEN_COMMENT:
            n++;
            continue;
        case TOKEN_EOF:
            return;
        case TOKEN_ERROR:
            LastCall("Reading HEXC colors");
        default:
            errno = ELOGIC;
            LastCall("Reading HEXC colors");
        }
    }
}

// Write out palette
static void write_colors(FILE* fp, PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        RGBColor color = pal->color[n];
        int result=0;
        BYTE* p = (BYTE*)&result;
        p[0] = color.b;
        p[1] = color.g;
        p[2] = color.r;
        fprintf(fp, "0x%08X", result);
    }
}



// Read HEXC file
void LoadHexcFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileRead(szPathName, "Opening i/p HEXC file");
    read_colors(fp, pal);
    Log("Read HEXC colors");
    CloseFile(fp);
}

// Write HEXC file
void SaveHexcFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p HEXC file");
    write_colors(fp, pal);
    Log("Wrote HEXC colors");
    CloseFile(fp);
}

