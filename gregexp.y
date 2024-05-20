%code requires {

#ifndef __GREGEXP_H__
#define __GREGEXP_H__

#ifdef __cplusplus
  extern "C" {
#endif

extern void yyerror (char *s);
extern void execParser(const char *s);
extern void initParser(void);
extern void getResult(char *s);
extern void logger(const char *toLog);

//UNKNOWN
//int yylex();


#ifdef __cplusplus
  }
#endif

#endif

}

%{

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
  #include "y.tab.h"

  #include <execinfo.h>
  #include <signal.h>
  #include <unistd.h>
  #include <time.h>

  char* concat(const char *s1, const char *s2);
  unsigned int rand_interval(unsigned int min, unsigned int max);

  //PROCESSES
  char * processDefInSQR(char * s, int a);
  char * trimFirstLastChar(const char *s);
  int processDefInBRA(char * s);
  char * processOR(char * a, char * b);
  char * processDef(char * s, int a);
  void localParser(char *s);
  //CONSTANTS
  const int maximun_length = 10;
  static char toReturn[129];

%}

%union {int number; char character; char* text;}
%start line
%token <text> SQR CHR BRA
%type <text> expr subexpr chars char sqr

%%

line	: expr                                             {localParser($1);} //print def
      ;

//return def
char    : CHR
        | CHR '*'                                       {int a= rand_interval(0, maximun_length); $$ = processDef($1, a); }
        | CHR '+'                                       {int a= rand_interval(1, maximun_length); $$ = processDef($1, a); }
        | CHR '?'                                       {int a= rand_interval(0, 1); $$ = processDef($1, a); }
        | CHR BRA                                       {int a= processDefInBRA($2); $$ = processDef($1, a); }
        ;

sqr     : SQR                                           {$$ = processDefInSQR($1, 1);}
        | SQR '*'                                       {int a= rand_interval(0, maximun_length); $$ = processDefInSQR($1, a); }
        | SQR '+'                                       {int a= rand_interval(1, maximun_length); $$ = processDefInSQR($1, a); }
        | SQR '?'                                       {int a= rand_interval(0, 1); $$ = processDefInSQR($1, a); }
        | SQR BRA                                       {int a= processDefInBRA($2); $$ = processDefInSQR($1, a); }
        ;

chars   : char                                          {$$ = $1;}
        | sqr                                           {$$ = $1;}
        | chars char                                    {$$ = concat($1, $2);}
        | chars sqr                                     {$$ = concat($1, $2);}
        ;

subexpr : chars                                         {$$ = $1;}
        | '(' subexpr ')'                               {$$ = $2;}
        | '(' subexpr ')' '*'                           {int a= rand_interval(0, maximun_length); $$ = processDef($2, a); }
        | '(' subexpr ')' '+'                           {int a= rand_interval(1, maximun_length); $$ = processDef($2, a); }
        | '(' subexpr ')' '?'                           {int a= rand_interval(0, 1); $$ = processDef($2, a); }
        | '(' subexpr ')' BRA                           {int a= processDefInBRA($4); $$ = processDef($2, a); }
        | subexpr '|' subexpr                           {$$ = processOR($1,$3);}
        | subexpr subexpr                               {$$ = concat($1,$2);}
        ;

expr    : subexpr                                       {$$ = $1;}
        ;


%%

void localParser(char *s){
  strcpy(toReturn, s);
}

void getResult(char *output){
  logger(toReturn);
  strcpy(output,toReturn);
}

char * processOR(char * a, char * b){
  int i = rand_interval(0,1);
  if(i == 0){
    return a;
  } else {
    return b;
  }
}

int processDefInBRA(char * s){
  
  logger(s);

  if( *s!='{' )
    return 0;

  s++;

  int first = 0;
  int second = 0;

  while( *s>='0' && *s<='9' ) {
    first = first * 10 + (*s - '0');
    s++;
  }

  if( *s==',' ) {
    s++;    

    while( *s>='0' && *s<='9' ) {
      second = second * 10 + (*s - '0');
      s++;
    }

    if( second==0 ) {
      second = maximun_length;
    }

    if( second < first ) {
      int temp = first;
      first = second;
      second = temp;
    }

  } else {
    second = first;
  }

  return rand_interval(first, second);
}

char * processDef(char * s, int a){
  //int r_i = 0;
  const size_t len = strlen(s);
  char *r = (char*)malloc(a * len + 1);
  char *write = r;
  for( int i = 0 ; i < a; i++ ) {
      strcpy( write, s );
      write += len;
  }
  
  return r;
}

char * processDefInSQR(char * s, int a){

  logger(s);
  
  char *r = (char*)malloc(128);
  memset( r, 0, 128);
  int r_i = 0;
  char * t = trimFirstLastChar(s);

  const size_t len = strlen(t);
  if(len > 1){
    char * temp = (char*)malloc(3);
    memset( temp, 0, 3);
    int iRange = 0;
    int eRange = 0;
    int i = 0;
    int temp_i = 0;
    while(i < len){

      temp[temp_i] = t[i];
      if(temp[0] != '\0' && temp[1] != '\0' && temp[1] != '-' ){
        r[r_i] = temp[0];
        temp[0] = temp[1];
        temp[1] = '\0';
        temp_i--;
        r_i++;
        if(i == len - 1){
          r[r_i] = temp[0];
        }
      } else if(temp[0] != '\0' && temp[1] == '-' && temp[2] != '\0'){
        iRange = temp[0];
        eRange = temp[2];
        while(iRange <= eRange){
          r[r_i] = (char) iRange;
          iRange++;
          r_i++;
        }
        temp[0] = '\0'; temp[1] = '\0'; temp[2] = '\0';
      }

      i++;
      temp_i++;
      if(temp_i > 2){
        temp_i = 0;
      }

    }

    char * result = (char*)malloc(128);
    int k = 0;

    int l = 0;
    while(r[l] != '\0'){
      l++;
    }
    
    int j;
    for(j = 0; j < a; j++){
      k = rand_interval(0, l-1);
      result[j] = r[k];
    }
    result[j] = '\0';
    return result;
  } else {
    char * result = (char*)malloc(2);
    result[0] = t[0];
    result[1] = '\0';
    return result;
  }

}

char * trimFirstLastChar(const char *s){
    const size_t len = strlen(s);
    char *r = (char*)malloc(len - 1);
  
    r[len - 2] = '\0';
    int j = 0;
    int i = 1;
    while(i < len - 1){
      r[j] = s[i];
      i++;
      j++; 
    }
    return r;
}

void execParser(const char * s){

  /*Copy string into new buffer and Switch buffers*/
  yy_scan_string (s);

  /*Analyze the string*/
  yyparse();

  yylex_destroy();
}

void initParser(void) {

  srand(time(0));
}

unsigned int rand_interval(unsigned int min_num, unsigned int max_num)
{

  int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num; // include max_num in output
    } else {
        low_num = max_num; // include max_num in output
        hi_num = min_num;
    }

    //srand(time(NULL));
    result = (rand() % (hi_num - low_num + 1)) + low_num;
    return result;
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = (char*)malloc(len1 + len2 + 1); // +1 for the null-terminator
    //I should check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}


void yyerror (char *s){
  fprintf(stderr, "%s\n", s);
}



void logger(const char *toLog)
{
  FILE *f = fopen("/etc/php/7.0/extensions/genregexp/log.txt", "a");
  if (f == NULL)
  {
      printf("Error opening file!\n");
      exit(1);
  }

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);


  fprintf(f, "%d-%d-%d %d:%d:%d -- Logging: %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, toLog);

  fclose(f);
}