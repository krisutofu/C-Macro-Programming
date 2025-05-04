/** simplified if-else preprocessor constructs, useful if you don't want to include all the other stuff */

#define _CHAR_PAREN_OPEN (
#define _CHAR_PAREN_CLOSE )
#define _CHAR_ 
#define _CHAR_COMMA ,

#define EVAL(...) __VA_ARGS__
#define EVAL_1ST($A, ...) $A
#define EVAL_2ND($A, $B, ...) $B
// concatenation works only in specific cases, try to replace with ##__VA_ARGS__ and EVAL
#define CONCAT($A,$B,...) $A##$B
#define _CHAR_1ST($A, ...) _CHAR_##$A
#define _CHAR_2ND($A, $B, ...) _CHAR_##$B

#define _COMMA_IF_1ST_ARG($A, ...) EVAL(_CHAR_2ND _CHAR_PAREN_OPEN CONCAT(_CHAR,_COMMA##$A), COMMA  _CHAR_PAREN_CLOSE)
#define _COMMA_IF_NOT_1ST_ARG($A, ...) EVAL(_CHAR_2ND _CHAR_PAREN_OPEN CONCAT(_CHAR,_COMMA##$A) COMMA,  _CHAR_PAREN_CLOSE)
#define COMMA_IF_EMPTY(...) EVAL(_CHAR_1ST _CHAR_PAREN_OPEN _COMMA_IF_1ST_ARG(__VA_ARGS__) __VA_ARGS__ COMMA _CHAR_PAREN_CLOSE)
#define COMMA_IF_NOT_EMPTY(...) EVAL(_CHAR_1ST _CHAR_PAREN_OPEN COMMA_IF_EMPTY(__VA_ARGS__) COMMA, _CHAR_PAREN_CLOSE)

/** triggers ELSE when __VA_ARGS__ is not provided or when __VA_ARGS__ is provided but empty */
#define IF_NOT_EMPTY_ELSE($then, $else, ...) IF_NOT_EMPTY($then, __VA_ARGS__)IF_EMPTY($else, __VA_ARGS__)
#define IF_NOT_EMPTY($then, ...) EVAL(EVAL_1ST _CHAR_PAREN_OPEN COMMA_IF_EMPTY(__VA_ARGS__) $then, _CHAR_PAREN_CLOSE)
#define IF_EMPTY($then, ...) EVAL(EVAL_1ST _CHAR_PAREN_OPEN _COMMA_IF_1ST_ARG(__VA_ARGS__) __VA_ARGS__ $then _CHAR_PAREN_CLOSE)

/** triggers ELSE only when __VA_ARGS__ is not provided */
#define IF_VA_ARGS_ELSE($then, $else, ...)  IF_VA_ARGS($then, ##__VA_ARGS__)IF_NOT_VA_ARGS($else, ##__VA_ARGS__)
#define IF_VA_ARGS($A,...) EVAL(EVAL_2ND _CHAR_PAREN_OPEN CONCAT(_CHAR, ##__VA_ARGS__, _COMMA), $A _CHAR_PAREN_CLOSE)
#define IF_NOT_VA_ARGS($A, ...) EVAL_1ST(, ##__VA_ARGS__ $A)
