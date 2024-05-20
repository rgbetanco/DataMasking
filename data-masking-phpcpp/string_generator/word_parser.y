%define api.pure full
%lex-param {void * scanner}
%parse-param {void * scanner}
%parse-param {char * output_buf}

%code requires {

#ifdef __cplusplus
  extern "C" {
#endif
typedef void* yyscan_t;

extern void yyerror(yyscan_t scanner, char *output_buf, const char* msg);
extern void execParser(const char *s, char *output_buf);
// extern void logger(const char *toLog);

#ifdef __cplusplus
  }
#endif

}

%{
	#include <time.h>
	#include <sodium.h>
	#include "y.tab.h"
	#include "lex.yy.h"
	#include "../debug.h"
	#include "chinese_generator/chinese_generator.h"
    
    #define CAPITAL_LETTER 0
    #define LOWERCASE_LETTER 1
    #define DIGIT 2

	void concat(char *dst, const char *s1, const char *s2);
	unsigned int rand_interval(unsigned int min, unsigned int max);

	//PROCESSES
	char *processDefInSQR(char *s, unsigned int a);
	char *trimFirstLastChar(const char *s);
	unsigned int processDefInBRA(char *s);
	char *processOR(char *a, char *b);
	char *processDef(char *s, unsigned int a);
	void localParser(char *s, char *output_buf);
	void processCommand(char *dst, const char *s1, const unsigned int cnt);
	void genRandomString(int length, char* ouput, int min, int max);
	//CONSTANTS
	const unsigned int maximum_length = 10;
	extern const size_t len_dst;
%}

%union {int number; char character; char* text;}
%start line
%token <text> SQR CHR BRA UTF8 CMD DOT
%type <text> expr subexpr chars char sqr dot

%%

line	: expr					{localParser($1, output_buf);}
    	;

//return def
char    : CHR
        | CHR '*'				{$$ = processDef($1, rand_interval(0, maximum_length));}
        | CHR '+'				{$$ = processDef($1, rand_interval(1, maximum_length));}
        | CHR '?'				{$$ = processDef($1, rand_interval(0, 1));}
        | CHR BRA				{$$ = processDef($1, processDefInBRA($2));}
		| CMD CHR				{
									char dst[len_dst];
									processCommand(dst, $2, 1);
									$$ = dst;
								}
		| CMD CHR '*'			{
									char dst[len_dst];
									processCommand(dst, $2, rand_interval(0, maximum_length));
									$$ = dst;
								}
		| CMD CHR '+'			{
									char dst[len_dst];
									processCommand(dst, $2, rand_interval(1, maximum_length));
									$$ = dst;
								}
		| CMD CHR '?'			{
									char dst[len_dst];
									processCommand(dst, $2, rand_interval(0, 1));
									$$ = dst;
								}
		| CMD CHR BRA			{
									char dst[len_dst];
									processCommand(dst, $2, processDefInBRA($2));
									$$ = dst;
								}
        | CMD UTF8              {
                                    char dst[len_dst];
                                    processCommand(dst, $2, 1);
                                    $$ = dst;
                                }
        | CMD UTF8 '*'          {
                                    char dst[len_dst];
                                    processCommand(dst, $2, rand_interval(0, maximum_length));
                                    $$ = dst;
                                }
        | CMD UTF8 '+'          {
                                    char dst[len_dst];
                                    processCommand(dst, $2, rand_interval(1, maximum_length));
                                    $$ = dst;
                                }
        | CMD UTF8 '?'          {
                                    char dst[len_dst];
                                    processCommand(dst, $2, rand_interval(0, 1));
                                    $$ = dst;
                                }
        | CMD '*'               {   char dst[len_dst];
                                    processCommand(dst, "*", 1);
                                    $$ = dst;
                                }
        | CMD '*' '*'           {
                                    char dst[len_dst];
                                    processCommand(dst, "*", rand_interval(0, maximum_length));
                                    $$ = dst;
                                }
        | CMD '*' '+'           {
                                    char dst[len_dst];
                                    processCommand(dst, "*", rand_interval(1, maximum_length));
                                    $$ = dst;
                                }
        | CMD '*' '?'           {
                                    char dst[len_dst];
                                    processCommand(dst, "*", rand_interval(0, 1));
                                    $$ = dst;
                                }
        ;

sqr     : SQR					{$$ = processDefInSQR($1, 1);}
        | SQR '*'				{$$ = processDefInSQR($1, rand_interval(0, maximum_length));}
        | SQR '+'				{$$ = processDefInSQR($1, rand_interval(1, maximum_length));}
        | SQR '?'				{$$ = processDefInSQR($1, rand_interval(0, 1));}
        | SQR BRA				{$$ = processDefInSQR($1, processDefInBRA($2));}
        ;

dot     : DOT                   {
                                    char output[len_dst];
                                    genRandomString(1, output, CAPITAL_LETTER, DIGIT);
                                    $$ = output;
                                }
        | DOT '*'               {
                                    char output[len_dst];
                                    genRandomString(rand_interval(0, maximum_length), output, CAPITAL_LETTER, DIGIT);
                                    $$ = output;
                                }
        | DOT '+'               {
                                    char output[len_dst];
                                    genRandomString(rand_interval(1, maximum_length), output, CAPITAL_LETTER, DIGIT);
                                    $$ = output;
                                }
        | DOT '?'               {
                                    char output[len_dst];
                                    genRandomString(rand_interval(0, 1), output, CAPITAL_LETTER, DIGIT);
                                    $$ = output;
                                }
        | DOT BRA               {
                                    char output[len_dst];
                                    genRandomString(processDefInBRA($2), output, CAPITAL_LETTER, DIGIT);
                                    $$ = output;
                                }
        ;

chars   : char					{$$ = $1;}
        | UTF8					{$$ = $1;}
        | sqr					{$$ = $1;}
        | dot                   {$$ = $1;}
        | chars char			{char dst[len_dst]; concat(dst, $1, $2); strcpy($$, dst);}
        | chars sqr				{char dst[len_dst]; concat(dst, $1, $2); strcpy($$, dst);}
        ;

subexpr : chars					{$$ = $1;}
        | '(' subexpr ')'		{$$ = $2;}
        | '(' subexpr ')' '*'	{$$ = processDef($2, rand_interval(0, maximum_length));}
        | '(' subexpr ')' '+'	{$$ = processDef($2, rand_interval(1, maximum_length));}
        | '(' subexpr ')' '?'	{$$ = processDef($2, rand_interval(0, 1));}
        | '(' subexpr ')' BRA	{$$ = processDef($2, processDefInBRA($4));}
        | subexpr '|' subexpr	{$$ = processOR($1, $3);}
        | subexpr subexpr		{char dst[len_dst]; concat(dst, $1, $2); strcpy($$, dst);}
        ;

expr    : subexpr				{$$ = $1;}
        ;
%%

void localParser(char *s, char *output_buf)
{
	strcpy(output_buf, s);
	debug_print(DEBUG_LVL_LIGHT, "output_buf = %s\n", output_buf);
}

void processCommand(char *dst, const char *s1, const unsigned int cnt)
{

	if (0 == strcmp(s1, "t")) // t-chinese
	{
		char *output = (char *)malloc(len_dst);
		
		gen_trad_chiniese_text(output, cnt);
		debug_print(DEBUG_LVL_LIGHT, "output = %s, cnt = %d\n", output, cnt);
		strcpy(dst, output);
		free(output);
	}
    else if (0 == strcmp(s1, "d")) // d-digit
    {
        char *output = (char *)malloc(len_dst);
        
        genRandomString(cnt, output, DIGIT, DIGIT);
        debug_print(DEBUG_LVL_LIGHT, "output = %s, cnt = %d\n", output, cnt);
        strcpy(dst, output);
        free(output);
    }
    else if (0 == strcmp(s1, "D")) // D-alphabet
    {
        char *output = (char *)malloc(len_dst);
        
        genRandomString(cnt, output, CAPITAL_LETTER, LOWERCASE_LETTER);
        debug_print(DEBUG_LVL_LIGHT, "output = %s, cnt = %d\n", output, cnt);
        strcpy(dst, output);
        free(output);
    }
    else if (0 == strcmp(s1, "＊") || 0 == strcmp(s1, "○") || 0 == strcmp(s1, "*"))
    {
        char *output = (char *)malloc(len_dst);
        
        for(int i = 0; i < cnt; i++)
        {
            strcat(output, s1);
        }
        
        debug_print(DEBUG_LVL_LIGHT, "output = %s, cnt = %d\n", output, cnt);
        strcpy(dst, output);
        free(output);
    }
	else
	{
		printf("no such command: %s, program terminate\n", s1);
		exit(1);
	}
}

char *processOR(char *a, char *b)
{
	return (0 == rand_interval(0, 1)) ? a : b;
}

unsigned int processDefInBRA(char *s)
{
	// logger(s);

	if (*s != '{')
		return 0;

	s++;

	int first = 0;
	int second = 0;

	while (*s >= '0' && *s <= '9')
	{
		first = first * 10 + (*s - '0');
		s++;
	}

	if (*s == ',')
	{
		s++;

		while (*s >= '0' && *s <= '9')
		{
			second = second * 10 + (*s - '0');
			s++;
		}

		if (second == 0)
		{
			second = (int)maximum_length;
		}

		if (second < first)
		{
			int temp = first;
			first = second;
			second = temp;
		}
	}
	else
	{
		second = first;
	}

	return rand_interval((unsigned int)first, (unsigned int)second);
}

char *processDef(char *s, unsigned int a)
{
	const size_t len = strlen(s);
	char *r = (char *)malloc(a * len + 1);
	char *write = r;
	for (unsigned int i = 0; i < a; i++)
	{
		strcpy(write, s);
		write += len;
	}

	return r;
}

char *processDefInSQR(char *s, unsigned int a)
{

	// logger(s);

	char *r = (char *)malloc(128);
	memset(r, 0, 128);
	int r_i = 0;
	char *t = trimFirstLastChar(s);

	const int len = strlen(t);
	if (len > 1)
	{
		char *temp = (char *)malloc(3);
		memset(temp, 0, 3);
		int iRange = 0;
		int eRange = 0;
		int i = 0;
		int temp_i = 0;

		while (i < len)
		{
			temp[temp_i] = t[i];
			if (temp[0] != '\0' && temp[1] != '\0' && temp[1] != '-')
			{
				r[r_i] = temp[0];
				temp[0] = temp[1];
				temp[1] = '\0';
				temp_i--;
				r_i++;

				if (i == len - 1)
					r[r_i] = temp[0];
			}
			else if (temp[0] != '\0' && temp[1] == '-' && temp[2] != '\0')
			{
				iRange = temp[0];
				eRange = temp[2];
				while (iRange <= eRange)
				{
					r[r_i] = (char)iRange;
					iRange++;
					r_i++;
				}
				temp[0] = '\0';
				temp[1] = '\0';
				temp[2] = '\0';
			}

			i++;
			temp_i++;
			if (temp_i > 2)
				temp_i = 0;
		}

		char *result = (char *)malloc(128);
		unsigned int k = 0;

		int l = 0;
		while (r[l] != '\0')
		{
			l++;
		}
        
		unsigned int j = 0;
        result[0] = '1';
        
        for (j = 0; j < a; j++)
        {
            if(r[0] == '^')
            {
                int i = 0;
                int check_char = 1;
                genRandomString(1, (result + j), CAPITAL_LETTER, DIGIT);
                while(check_char)
                {
                    if(result[j] != r[i] && r[i] != '\0')
                    {
                        i++;
                    }
                    else if(result[j] == r[i] && r[i] != '\0')
                    {
                        genRandomString(1, (result + j), CAPITAL_LETTER, DIGIT);
                        check_char = 1;
                        i = 0;
                    }
                    else if(r[i] == '\0')
                    {
                        check_char = 0;
                    }
                }
                
            }
            else
            {
                k = rand_interval(0, (unsigned int)l - 1);
                result[j] = r[k];
            }
        }
        result[j] = '\0';
        return result;
	}
	else
	{
		char *result = (char *)malloc(2);
		result[0] = t[0];
		result[1] = '\0';
		return result;
	}
}

char *trimFirstLastChar(const char *s)
{
	const size_t len = strlen(s);
	char *r = (char *)malloc(len - 1);
	r[len - 2] = '\0';
	int j = 0;
	int i = 1;

	while (i < (int)len - 1)
	{
		r[j] = s[i];
		i++;
		j++;
	}
	return r;
}

void execParser(const char *s, char *output_buf)
{
	yyscan_t scanner;
	
	yylex_init(&scanner);

	/*Copy string into new buffer and Switch buffers*/
	yy_scan_string(s, scanner);

	/*Analyze the string*/
	yyparse(scanner, output_buf);

	yylex_destroy(scanner);
}

unsigned int rand_interval(unsigned int min_num, unsigned int max_num)
{
    char myString[32];
    uint32_t myInt;

    if (sodium_init() < 0)
	{
        printf("panic! the Sodium library couldn't be initialized, it is not safe to use\n");
        return 0; 
    }

    // myString will be an array of 32 random bytes, not null-terminated     
    randombytes_buf(myString, 32);

    // myInt will be a random number between 0 and RAND_MAX
    myInt = randombytes_uniform(RAND_MAX);
    
	unsigned int result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num)
	{
		low_num = min_num;
		hi_num = max_num; // include max_num in output
	}
	else
	{
		low_num = max_num; // include max_num in output
		hi_num = min_num;
	}

	result = (myInt % (hi_num - low_num + 1)) + low_num;
	return result;
}

void genRandomString(int length, char *ouput, int min, int max)
{
	int flag, i;
	for (i = 0; i < length; i++)
	{
		flag = rand_interval(min, max);
		switch (flag)
		{
		case 0:
			ouput[i] = 'A' + rand_interval(0, 25);
			break;
		case 1:
			ouput[i] = 'a' + rand_interval(0, 25);
			break;
		case 2:
			ouput[i] = '0' + rand_interval(0, 9);
			break;
		default:
			ouput[i] = 'x';
			break;
		}
	}
}

void concat(char *dst, const char *s1, const char *s2)
{
	strcpy(dst, s1);
	strcat(dst, s2);
}

void yyerror(yyscan_t scanner, char *output_buf, const char* msg)
{
	fprintf(stderr, "scanner = %p, output_buf = %s, %s\n", scanner, output_buf, msg);
}

// void logger(const char *toLog)
// {
// 	FILE *f = fopen("./log.txt", "a");
// 	if (f == NULL)
// 	{
// 		printf("Error opening file!\n");
// 		exit(1);
// 	}

// 	time_t t = time(NULL);
// 	struct tm tm = *localtime(&t);

// 	fprintf(f, "%d-%d-%d %d:%d:%d -- Logging: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, toLog);

// 	fclose(f);
// }
