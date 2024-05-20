%define api.pure full
%lex-param {void * scanner}
%parse-param {void * scanner}
%parse-param {node_q ***queue_head}

%code requires {

#ifdef __cplusplus
	extern "C" {
#endif
#include "treebuilder.h" // for the type tree_node

typedef void* yyscan_t;

extern void treeB_error(yyscan_t scanner, node_q ***queue_head, const char* msg);
extern void treeB_execParser(const char *input, char *output, tree_node **tree_root);
// extern void treeB_logger(const char *toLog);

#ifdef __cplusplus
	}
#endif

}

%{
	#include "tree_parser_yacc.tab.h"
	#include "tree_parser_lex.h"
	#include "queue.h"
	#include "string_builder.h"
	#include "../debug.h"

	//unsigned int rand_interval(unsigned int min, unsigned int max);
	void dequeue_all_tree_node(node_q *head);
	void treeB_concat(char *dst, const char *s1, const char *s2);
	void treeB_concat_three(char *dst, const char *s1, const char *s2, const char *s3);

	const size_t len_dst = 1024; // 1KB
%}

%union {int number; char character; char* text;}
%start line
%token <text> SQR CHR BRA UTF8 CMD DOT
%type <text> expr subexpr chars char sqr dot

%%

line	: expr       
		;

//return def
char    : CHR       
		| CHR '*'               {char dst[len_dst]; treeB_concat(dst, $1, "*"); treeBuilder(dst, "", "", *queue_head);}
		| CHR '+'               {char dst[len_dst]; treeB_concat(dst, $1, "+"); treeBuilder(dst, "", "", *queue_head);}
		| CHR '?'               {char dst[len_dst]; treeB_concat(dst, $1, "?"); treeBuilder(dst, "", "", *queue_head);}
		| CHR BRA               {treeBuilder("Repeat", $1, $2, *queue_head); /* $$ = "Repeat";*/}
		| CMD					{
									char dst[len_dst];
									treeB_concat(dst, "\\", $1);
									treeBuilder(dst, "", "", *queue_head);
								}
		| CMD '*'				{
									char dst[len_dst];
									treeB_concat_three(dst, "\\", $1, "*");
									treeBuilder(dst, "", "", *queue_head);
								}
		| CMD '+'				{
									char dst[len_dst];
									treeB_concat_three(dst, "\\", $1, "+");
									treeBuilder(dst, "", "", *queue_head);
								}
		| CMD '?'				{
									char dst[len_dst];
									treeB_concat_three(dst, "\\", $1, "?");
									treeBuilder(dst, "", "", *queue_head);
								}
		| CMD BRA				{
									char dst[len_dst];
									treeB_concat(dst, "\\", $1);
									treeBuilder("RepeatSQR", dst, $2, *queue_head);
									/* $$ = "RepeatSQR";*/
								}
		;

sqr     : SQR                   {treeBuilder($1, "", "", *queue_head);}
		| SQR '*'               {char dst[len_dst]; treeB_concat(dst, $1, "*"); treeBuilder(dst, "", "", *queue_head); strcpy($$, dst);}
		| SQR '+'               {char dst[len_dst]; treeB_concat(dst, $1, "+"); treeBuilder(dst, "", "", *queue_head); strcpy($$, dst);}
		| SQR '?'               {char dst[len_dst]; treeB_concat(dst, $1, "?"); treeBuilder(dst, "", "", *queue_head); strcpy($$, dst);}
		| SQR BRA               {treeBuilder("RepeatSQR", $1, $2, *queue_head); /*$$ = "RepeatSQR";*/}
		;

chars   : char                  {$$ = $1;}
		| UTF8					{$$ = $1; /*treeBuilder($1, "", "", *queue_head);*/}
		| sqr                   {$$ = $1;}
        | dot                   {$$ = $1;}
		| chars char            {char dst[len_dst]; treeB_concat(dst, $1, $2); strcpy($$, dst);}
		| chars sqr             {char dst[len_dst]; treeB_concat(dst, $1, $2); strcpy($$, dst);}
		;

dot     :DOT                    {treeBuilder(".", "", "", *queue_head);}
        | DOT '*'               {treeBuilder(".*", "", "", *queue_head);}
        | DOT '+'               {treeBuilder(".+", "", "", *queue_head);}
        | DOT '?'               {treeBuilder(".?", "", "", *queue_head);}
        | DOT BRA               {treeBuilder("RepeatSQR", ".", $2, *queue_head);/*char dst[len_dst]; treeB_concat(dst, ".", $2);treeBuilder(dst, "", "", *queue_head);*/}
        ;

subexpr : chars                 {$$ = $1;}
		| '(' subexpr ')'       {treeBuilder($2, "", "", *queue_head);}
		| '(' subexpr ')' '*'   {char dst[len_dst]; treeB_concat_three(dst, "(", $2, ")*"); treeBuilder(dst, "", "", *queue_head);}
		| '(' subexpr ')' '+'   {char dst[len_dst]; treeB_concat_three(dst, "(", $2, ")+"); treeBuilder(dst, "", "", *queue_head);}
		| '(' subexpr ')' '?'   {char dst[len_dst]; treeB_concat_three(dst, "(", $2, ")?"); treeBuilder(dst, "", "", *queue_head);}
		| '(' subexpr ')' BRA   {treeBuilder("Repeat()", $2, $4, *queue_head); /*$$ = "Repeat()";*/}
		| subexpr '|' subexpr   {treeBuilder("Or", $1, $3, *queue_head); /*$$ = "Or";*/}
		| subexpr subexpr       {char dst[len_dst]; treeB_concat(dst, $1, $2); strcpy($$, dst);}
		;

expr    : subexpr               {$$ = $1;}
		;

%%

void treeB_execParser(const char *input, char *output, tree_node **tree_root)
{
	yyscan_t scanner;
	node_q *queue = NULL;
	node_q **queue_ptr = &queue;

	treeB_lex_init(&scanner);
	
	/*Copy string into new buffer and Switch buffers*/
	treeB__scan_string(input, scanner);
	
	/*Analyze the string*/
	treeB_parse(scanner, &queue_ptr);
	
	if (NULL == *tree_root)
	{
		debug_print(DEBUG_LVL_LIGHT, "%s\n", "NULL == *tree_root");
		*tree_root = tree_construct(queue_ptr);
	}

	node_q *queue_for_string = NULL;
	build_string_by_tree(output, *tree_root, &queue_for_string);
	
	// free resource
	treeB_lex_destroy(scanner);
	dequeue_all_tree_node(*queue_ptr);
	queue_ptr = NULL;
	queue = NULL;

	dequeue_all_tree_node(queue_for_string);
	queue_for_string = NULL;
}

void dequeue_all_tree_node(node_q *head)
{
	while (!queue_is_empty(head))
	{
		debug_print(DEBUG_LVL_LIGHT, "%s\n", "dequeue_tree_node");
		dequeue_tree_node(&head);
	}
}

void treeB_concat(char *dst, const char *s1, const char *s2)
{
	strcpy(dst, s1);
	strcat(dst, s2);
}

void treeB_concat_three(char *dst, const char *s1, const char *s2, const char *s3)
{
	strcpy(dst, s1);
	strcat(dst, s2);
	strcat(dst, s3);
}

void treeB_error(yyscan_t scanner, node_q ***queue_head, const char* msg)
{
	fprintf(stderr, "scanner = %p, queue_head = %p, %s\n", scanner, queue_head, msg);
}

// void treeB_logger(const char *toLog)
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