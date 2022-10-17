"auto"					AUTO
"break"					BREAK
"case"					CASE
"char"					CHAR
"const"					CONST
"continue"				CONTINUE
"default"				    DEFAULT
"do"					    DO
"double"				    DOUBLE
"else"					ELSE
"enum"					ENUM
"extern"				    EXTERN
"float"					FLOAT
"for"					    FOR
"goto"					GOTO
"if"					    IF
"inline"				    INLINE
"int"					    INT
"long"					LONG
"register"				REGISTER
"restrict"				RESTRICT
"return"				    RETURN
"short"					SHORT
"signed"				    SIGNED
"sizeof"				    SIZEOF
"static"				    STATIC
"struct"				    STRUCT
"switch"				    SWITCH
"typedef"				    TYPEDEF
"union"					UNION
"unsigned"				UNSIGNED
"void"					VOID
"volatile"				VOLATILE
"while"					WHILE
"_Alignas"                ALIGNAS
"_Alignof"                ALIGNOF
"_Atomic"                 ATOMIC
"_Bool"                   BOOL
"_Complex"                COMPLEX
"_Generic"                GENERIC
"_Imaginary"              IMAGINARY
"_Noreturn"               NORETURN
"_Static_assert"          STATIC_ASSERT
"_Thread_local"           THREAD_LOCAL
"__func__"                FUNC_NAME
#
"[a-zA-Z_][a-zA-Z_0-9]*"			      IDENTIFIER
#
# integer constants: hex, dec, oct
# NOTE!!! 按照qutt网站,整常数不包含0???
"(0[xX])[a-fA-F0-9]+(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))?"                       I_CONSTANT
"[1-9][0-9]*(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))?"                               I_CONSTANT
"0[0-7]*(((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))?"				                    I_CONSTANT
#
# floating point constants: sci-rep, sci-rep with decimal(dec, hex)
"[0-9]+([Ee][+-]?[0-9]+)(f|F|l|L)?"                                                   F_CONSTANT
"[0-9]*\.[0-9]+([Ee][+-]?[0-9]+)?(f|F|l|L)?"			                                F_CONSTANT
"[0-9]+\.([Ee][+-]?[0-9]+)?(f|F|l|L)?"			                                    F_CONSTANT
"0[xX][a-fA-F0-9]+([Pp][+-]?[0-9]+)(f|F|l|L)?"                                        F_CONSTANT
"0[xX][a-fA-F0-9]*\.[a-fA-F0-9]+([Pp][+-]?[0-9]+)(f|F|l|L)?"                         F_CONSTANT
"0[xX][a-fA-F0-9]+\.([Pp][+-]?[0-9]+)(f|F|l|L)?"                                     F_CONSTANT
#
"\"(\\.|[^\\"])*\""    STRING_LITERAL
#
"\.\.\."			 ELLIPSIS
">>="					 RIGHT_ASSIGN
"<<="					 LEFT_ASSIGN
"\+="			     ADD_ASSIGN
"-="					 SUB_ASSIGN
"\*="				 MUL_ASSIGN
"/="					 DIV_ASSIGN
"%="					 MOD_ASSIGN
"&="					 AND_ASSIGN
"\^="				 XOR_ASSIGN
"\|="				 OR_ASSIGN
">>"					 RIGHT_OP
"<<"					 LEFT_OP
"\+\+"				 INC_OP
"--"					 DEC_OP
"->"					 PTR_OP
"&&"					 AND_OP
"\|\|"				 OR_OP
"<="					 LE_OP
">="					 GE_OP
"=="					 EQ_OP
"!="					 NE_OP
";"					 ;
"\{|<%"				 {
"\}|%>"				 }
","					 ,
":"					 :
"="					 =
"\("					 (
"\)"					 )
"\[|<:"				 [
"\]|:>"				 ]
"\."					 .
"&"					 &
"!"					 !
"~"					 ~
"-"					 -
"\+"					 +
"\*"					 *
"/"					 /
"%"					 %
"<"					 <
">"					 >
"\^"					 ^
"\|"					 |
"\?"					 ?
#
"/*"                                    MULTI_LINE_COMMENT
"//.*"                                  SINGLE_LINE_COMMENT
#
# 以下是一些次要的东西(目前不太清楚有啥用的)
"[ \t\v\n\f]+"            WHITE_SPACES
#"."					    DISCARD_BAD_CHARACTER