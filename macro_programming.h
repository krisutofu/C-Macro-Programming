#ifndef INC_MACRO_MAGIC_H
#define INC_MACRO_MAGIC_H

/** defines a new type of given size */
#define TYPE(SIZE)	struct{uint8_t _[SIZE];}

/* macros to destructure an comma-separated list (e.g. driver arguments) which are reminiscent of Haskell */
#define TAKE(...) _TAKE(__VA_ARGS__)
#define _TAKE(A,...) A
#define DROP(...) _DROP(__VA_ARGS__)
#define _DROP(A,...) __VA_ARGS__
/** @brief this is able to spread the arguments packed together in one define-variable or more generally
 * it expands (evaluates) the arguments BEFORE calling FUNC(...) instead of the reverse.
 * If you write F(ARG), ARG replaces each appearance of the first parameter within F with the expansion of ARG.
 * Now if F would have two parameters A and B and ARG is defined as "1, 2" then F(ARG) would give an error.
 * You first have to expand ARG to 1, 2 and only then call the function. This can only be achieved with
 * intermediate macro call where the final macro name is called with the arguments.
 *
 * Only use this, if you want to use one symbol as argument list for a macro, for example the USE_XXX symbols.
 * The name "CALL" is already used in some source code.
 */
#define SPREAD(FUNC,ARGS) FUNC(ARGS)
/** Expand the argument. This is usually only useful within macros definitions or if you want to delimit a
 * single argument with multiple sub-arguments without generating parentheses in the expansion.
 * Macro arguments are always expanded before any other macro body expansion takes place. To phonily prevent
 * argument expansion in the body, surround it with I() otherwise the contained commas are interpreted as
 * argument delimiters for further macro calls in the macro body. */
#define I(...) __VA_ARGS__
/** can be used with IFNOT for conditional execution on concatenated symbols, when the concatenated symbol
 * is parenthesized in the expansion.
 * e.g.: IF_ELSE(I(BLANK CONC(PREFIX,ARG)), <PREFIX##ARG unsupported>, <PREFIX##ARG supported>) */
#define BLANK(...)
#define COMMA(...) ,
/** expects parenthesized list of arguments after F.
 * More generally said, this removes the first comma in the arguments. This step is hard to undo at
 * a later point. The only undo method I know is to concatenate this expansion to a prefix where the
 * concatenated symbol PREFIX ## F expands to the original "F,". F has to be a constant symbol name */
#define APPLY(F, ...) F __VA_ARGS__
/** abbreviation for SPREAD(_CONC, A, B), you should not need to use this with SPREAD macros */
#define CONC(A,...) _CONC(A,__VA_ARGS__)
/** Call this macro either with SPREAD or in another macro to expand the arguments before concatenation. */
#define _CONC(A, ...) A ## __VA_ARGS__
/** abbreviation for SPREAD(_STRING, A), you should not need to use this with SPREAD macros */
#define STRING(...) _STRING(__VA_ARGS__)
/** This can be used with SPREAD() to stringify expansions of macros and define-variables. */
#define _STRING(...) #__VA_ARGS__
/** stringifies the argument expansion and uses the result as pragma content*/
#define PRAGMA(...) _Pragma(STRING(__VA_ARGS__))
/** gives a new unique variable name based on line number but differs only in ID within caller expansion. */
#define NAME(ID) CONC(CONC( _, __LINE__),CONC(_,ID))

/** inserts a comma, if the arguments don't expand to blank. Safely can be used as function pointer even
 * though does not need SPREADing. */
#define VARGS(...) _VARGS(__VA_ARGS__)
/** This is used to insert a comma, if the given argument(s) are not empty. */
#define _VARGS(...) , ##__VA_ARGS__
/** inserts a comma AFTER the argument(s), iff the arguments are non-blank */
#define OPT(...) DROP(VARGS(__VA_ARGS__),)
/** will result in a valid expression that you can use with operators or return statements.
 * The expression type will be void, if the argument is blank. */
#define EXPR(...) TRY(_EXPRI(__VA_ARGS__),((void)0))
#define _EXPRI(...) __VA_ARGS__
/** allows for use of any statements as a single expression. Can only be used in functions.
 * The last non-terminated expression in the code is used as return value. If you don't
 * want any return value, terminate the last statement with a semicolon.*/
#define EXPRS(...) ({__VA_ARGS__;})
/* This is a constant boolean value. If you want to use conditional macro expansion,
 * use the IF(), IFNOT() or IF_ELSE() macro below (or CONC<>, APPLY<>). */
#define ISBLANK(...)  IF_ELSE(_ISBLANKI(__VA_ARGS__),0,1)
#define _ISBLANKI(...) __VA_ARGS__
/** returns true, if the first argument (or everything) is blank */
#define STARTSBLANK(...) _NOT(TAKE(__VA_ARGS__))
#define NOTSTARTSBLANK(...) BOOL(TAKE(__VA_ARGS__))
/** removes the outer parenthesis pair from the first argument.
 *  DISMANTLE does the same but can ignore arguments without
 *  outer pair. I know, this is not currying but the definition looks like a curried function call.
 *  This macro is only useful outside of macros. Inside macros you always can use I or a synonymous
 *  macro with different name. */
#define UNGROUP(...) _UNGROUPI __VA_ARGS__
#define _UNGROUPI(...) __VA_ARGS__
//all arguments are parenthesized at once if not so already.
#define GROUP(...) _GROUP(_GROUPI(BLANK __VA_ARGS__),__VA_ARGS__)
#define _GROUP(A,...) IF_ELSE(_GROUPI(A),(__VA_ARGS__),_GROUPI(__VA_ARGS__))
#define _GROUPI(...) __VA_ARGS__

/* If the argument is fully surrounded by a pair of parentheses, this pair is removed
 * otherwise nothing is changed. */
#define DISMANTLE(...) _DISMANTLE(_DISMANTLEI(BLANK __VA_ARGS__),__VA_ARGS__)
#define _DISMANTLE(A,...) IF_ELSE(_DISMANTLEI(A),_DISMANTLEI(__VA_ARGS__),UNGROUP(__VA_ARGS__))
#define _DISMANTLEI(...) __VA_ARGS__
//returns true, if is fully surrounded by a pair of parentheses
#define IS_GROUP(...) _IS_GROUP0(BLANK __VA_ARGS__)
#define _IS_GROUP0(...) _IS_GROUP1(__VA_ARGS__)
#define _IS_GROUP1(...) _NOT(__VA_ARGS__)
//returns true, if it starts with a pair of parentheses
#define STARTSGROUP(...) _STARTSGROUP(COMMA __VA_ARGS__)
#define _STARTSGROUP(...) _NOT(_STARTSGROUPTAKE(__VA_ARGS__))
#define _STARTSGROUPTAKE(A,...) A

#define NAME_FORBIDDEN ,, //must not be found
#define NAME_NOLAST , //must not be followed by comma, except if an empty argument is a valid name
/**checks whether ARG starts with an allowed symbol and returns 1 iff true. You must specify any
 * allowed symbols by defining NAMESPACE ## _ ## S (no body) for each allowed symbol S. */
#define STARTSWITH(NAMESPACE,...) _NOT(TAKE(CONC(NAMESPACE,_ ## __VA_ARGS__)))
/**checks whether ARG starts without forbidden symbol. Retruns 1 iff true. Specify forbidden symbols S
 * as NAMESPACE ## _ ## S (no body). */
#define NOTSTARTSWITH(NAMESPACE,...) TAKE(CONC(NAMESPACE,_ ## __VA_ARGS__))
/* you could do a lot more complex things with STARTSWITH by using WHILE() to check all words in one argument */

#define BINARY 1,0
#define DECIMAL 9,8,7,6,5,4,3,2,1,0
#define HEXADEC F,E,D,C,B,A,DECIMAL
#define _ARGNUMLIST  30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,DECIMAL
//gives the number of arguments in the range of 0 to 10 as constant literal (idea from stack overflow).
//use ARGNUM(BOOL(...)) to convert a boolean preprocessor expression (non-/blank) to boolean integer (1,0).
//you don't need use SPREAD with it
#define ARGNUM(...) _ARGNUM(__VA_ARGS__)
#define _ARGNUM(...) _ARGNUM0(_ARGNUMLIST,##__VA_ARGS__)
#define _ARGNUM0(NUMBERS,...) _ARGNUM_CHOOSE(,##__VA_ARGS__, NUMBERS)
#define _ARGNUM_CHOOSE(_,A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,NUM,...) NUM

//expands to 1 if argument is blank, otherwise expands to blank
#define NOT(...) _NOT(__VA_ARGS__)
#define _NOT(...) _NOT0(_NOTVARGS(__VA_ARGS__) 1)
#define _NOTVARGS(...) ,##__VA_ARGS__
#define _NOT0(A) _NOT1(A)
#define _NOT1(A,...) A
//expands to 1 if argument is not blank, otherwise expands to blank
#define BOOL(...) _NOT(_NOT(__VA_ARGS__))
//converts a C boolean to preprocessor boolean,
//i.e. PPBOOL(0) and PPBOOL() expands to blank, everything else expands to the argument
#define PPBOOL(...) IF(_PPBOOLI(CONC(_PPBOOL,CONC(_,__VA_ARGS__))),_PPBOOLI(__VA_ARGS__))
#define _PPBOOLI(...) __VA_ARGS__
#define _PPBOOL_0
#define _PPBOOL_

//logic operators, returns 1 for true, blank for false; ordered from fastest to slowest
//use ARGNUM() to convert this boolean value to boolean integer value for C
#define OR(A,...) A __VA_ARGS__
#define NOR(A,...) _NOT(A __VA_ARGS__)
#define NAND(A,...) _NOT(A) _NOT(__VA_ARGS__)
#define AND(A,...) _NOT(_NOT(A) _NOT(__VA_ARGS__))
#define XOR(A,B) _XOR(_XORVARGS(_NOT(A))_XORVARGS(_NOT(B)),,1)
#define _XOR(...) _XOR0(__VA_ARGS__)
#define _XOR0(A,B,C,...) B
#define _XORVARGS(...) _XORVARGS0(__VA_ARGS__)
#define _XORVARGS0(...) ,##__VA_ARGS__
#define XNOR(A,B) _XNOR(_XORVARGS(_NOT(A))_XORVARGS(_NOT(B)),,1)
#define _XNOR(...) _XNOR0(__VA_ARGS__)
#define _XNOR0(A,B,C,...) C

//expands to THEN if A is blank otherwise expands to blank
//If you don't want your argument to be expanded before the decision, you need to use CONCIF or APPLYIF.
//CONCIF and APPLYIF are the only solution if you need to treat parentheses as literal characters.
#define IF(A,...) _IF(BOOL(A),_IFI(__VA_ARGS__))
#define _IF(A,THEN) _IF0(_IFVARGS(A),_IFI(THEN))
#define _IFVARGS(...) ,##__VA_ARGS__
#define _IFI(...) __VA_ARGS__
#define _IF0(A,B) _IF1(A,_IFI(B))
#define _IF1(A,B,...) __VA_ARGS__

//concatenates both SYM arguments if A is not blank, otherwise expands to blank
#define CONCIF(A, SYM1, ...) _CONCIF(BOOL(A),_CONCIFI(SYM1),_CONCIFI(__VA_ARGS__))
#define _CONCIF(A, SYM1, SYM2) _CONCIF0(_CONCIFVARGS(A),_CONCIFVARGS(A),_CONCIFI(SYM1),_CONCIFI(SYM2),,)
#define _CONCIFVARGS(...) ,##__VA_ARGS__
#define _CONCIFI(...) __VA_ARGS__
#define _CONCIF0(A1,A2,S1,S2,C1,C2) _CONCIF1(A1,A2,_CONCIFI(S1),_CONCIFI(S2),C1,C2)
#define _CONCIF1(A1,A2,B1,B2,C1,C2,...) CONC(C1,C2)

//applies parenthesized ARGS to FUNC if A is not blank, otherwise expands to blank
#define APPLYIF(A, FUNC, ...) _APPLYIF(BOOL(A), _APPLYIFI(FUNC), _APPLYIFI(__VA_ARGS__))
#define _APPLYIF(A, FUNC, ARGS) _APPLYIF0(_APPLYIFVARGS(A),_APPLYIFVARGS(A),_APPLYIFI(FUNC),_APPLYIFI(ARGS),,)
#define _APPLYIFVARGS(...) ,##__VA_ARGS__
#define _APPLYIFI(...) __VA_ARGS__
#define _APPLYIF0(A1,A2,B1,B2,C1,C2) _APPLYIF1(A1,A2,_APPLYIFI(B1),_APPLYIFI(B2),C1,C2)
#define _APPLYIF1(A1,A2,B1,B2,C1,C2,...) C1 C2

//if N is positive (length), call FUNC ARGS
#define _CALLIF_1
#define CALLIF(N, FUNC, ...) _CALLIF(CONC(_CALLIF_,_NOT N),_CALLIFI(FUNC),_CALLIFI(__VA_ARGS__))
#define _CALLIF(A, FUNC, ARGS) _CALLIF0(_CALLIFVARGS(A),_CALLIFVARGS(A),_CALLIFI(FUNC),_CALLIFI(ARGS),,)
#define _CALLIFVARGS(...) ,##__VA_ARGS__
#define _CALLIFI(...) __VA_ARGS__
#define _CALLIF0(A1,A2,B1,B2,C1,C2) _CALLIF1(A1,A2,_CALLIFI(B1),_CALLIFI(B2),C1,C2)
#define _CALLIF1(A1,A2,B1,B2,C1,C2,...) C1 C2

/*
 * If you want to nest conditional macro calls (conditional call in the expansion of a conditional call) you
 * have to write I(IF(A,FUNC)IF(A,ARGS)) explicitly.
 * Unfortunately, using an additional macro name for this conditional call would not allow for nesting.
 *
 * Likewise you can use CONC(IF(A,SYM1),IF(A,SYM2)) for conditional concatenation that can be nested.
 *
 * If you generate an unexpanded I() or CONC() as result of IF(..)IF(..) , and you want to directly expand it
 * after argument expansion, you have to use a synonymous I() or CONC() with different name for the final
 * expansion.
 */

//expands to ELSE if A is not blank otherwise expands to blank
#define IFNOT(A,...) _IFNOT(BOOL(A),_IFNOTI(__VA_ARGS__))
#define _IFNOT(A,ELSE) _IFNOT0(_IFNVARGS(A),,_IFNOTI(ELSE))
#define _IFNVARGS(...) ,##__VA_ARGS__
#define _IFNOTI(...) __VA_ARGS__
#define _IFNOT0(A,B,C) _IFNOT1(A,B,_IFNOTI(C))
#define _IFNOT1(A,B,C,...) C

#define CONCIFNOT(A,SYM1,...) _CONCIFNOT(BOOL(A),_CONCIFNOTI(SYM1),_CONCIFNOTI(__VA_ARGS__))
#define _CONCIFNOT(A,SYM1,SYM2) _CONCIFNOT0(_CONCIFNVARGS(A),_CONCIFNVARGS(A),,,_CONCIFNOTI(SYM1),_CONCIFNOTI(SYM2))
#define _CONCIFNVARGS(...) ,##__VA_ARGS__
#define _CONCIFNOTI(...) __VA_ARGS__
#define _CONCIFNOT0(A1,A2,B1,B2,S1,S2) _CONCIFNOT1(A1,A2,B1,B2,_CONCIFNOTI(S1),_CONCIFNOTI(S2))
#define _CONCIFNOT1(A1,A2,B1,B2,C1,C2,...) CONC(C1,C2)

#define APPLYIFNOT(A,FUNC,...) _APPLYIFNOT(BOOL(A),_APPLYIFNI(FUNC),_APPLYIFNI(__VA_ARGS__))
#define _APPLYIFNOT(A,FUNC,ARGS) _APPLYIFNOT0(_APPLYIFNVARGS(A),_APPLYIFNVARGS(A),,,_APPLYIFNI(FUNC),_APPLYIFNI(ARGS))
#define _APPLYIFNVARGS(...) ,##__VA_ARGS__
#define _APPLYIFNI(...) __VA_ARGS__
#define _APPLYIFNOT0(A1,A2,B1,B2,C1,C2) _APPLYIFNOT1(A1,A2,B1,B2,_APPLYIFNI(C1),_APPLYIFNI(C2))
#define _APPLYIFNOT1(A1,A2,B1,B2,C1,C2,...) C1 C2

#define CALLIFNOT(N, FUNC, ...) _CALLIFNOT(CONC(_CALLIFNOT_,_NOT N),_CALLIFNOTI(FUNC),_CALLIFNOTI(__VA_ARGS__))
#define _CALLIFNOT(A, FUNC, ARGS) _CALLIFNOT0(_CALLIFNOTVARGS(A),_CALLIFNOTVARGS(A),,,_CALLIFNOTI(FUNC),_CALLIFNOTI(ARGS))
#define _CALLIFNOTVARGS(...) ,##__VA_ARGS__
#define _CALLIFNOTI(...) __VA_ARGS__
#define _CALLIFNOT0(A1,A2,B1,B2,C1,C2) _CALLIFNOT1(A1,A2,B1,B2,_CALLIFNOTI(C1),_CALLIFNOTI(C2))
#define _CALLIFNOT1(A1,A2,B1,B2,C1,C2,...) C1 C2

/** Conditionally compiles THEN if the variadic argument is NOT blank, otherwise compiles ELSE. */
#define IF_ELSE(A, THEN, ...) IF(_IFI(A),THEN)IFNOT(_IFNOTI(A),__VA_ARGS__)
#define CONCIF_ELSE(A,THEN1,THEN2,ELSE1,...) CONCIF(_CONCIFI(A),_CONCIFI(THEN1),THEN2)CONFIFNOT(_CONCIFI(A),TRY(_CONCIFI(ELSE1),THEN1),TRY(_CONCIFI(__VA_ARGS__),THEN2))
#define APPLYIF_ELSE(A,THENFUNC,THENARGS,ELSEFUNC,...) APPLYIF(_APPLYIFI(A),_APPLYIFI(THENFUNC),THENARGS)APPLYIFNOT(_APPLYIFI(A),TRY(_APPLYIFI(ELSEFUNC),THENFUNC),TRY(_APPLYIFI(__VA_ARGS__),THENARGS))

//compiles A, if it is not blank, else compiles B. A should NOT contain unparenthesized commas.
#define TRY(A,...) _TRY(_TRYI(A), _TRYI(__VA_ARGS__))
#define _TRY(A,B) _TRY0(_TRYVARGS(A), _TRYI(B))
#define _TRYVARGS(...) ,##__VA_ARGS__
#define _TRYI(...) __VA_ARGS__
#define _TRY0(A, B) _TRY1(A, _TRYI(B))
#define _TRY1(A, B, ...) B


//#define _TRY(VALUE, ELSE) IGNORE_WARNING_EXPR(unused-value,(ELSE _TRYVARGS1(VALUE)))
//#define _TRYVARGS1(...) ,##__VA_ARGS__
//#define TRY(VALUE, ELSE) (VALUE +(ELSE+0)*!*#VALUE)

/** Code macros which have no return value and can only be used like a statement. You can use
 * any control flow keywords like break, continue and return like normally.
 * Use EXPRS() for a macro with return value. */
#define CMACRO(...) if(1){ __VA_ARGS__ }else do;while(0)
#define CMACROIF(COND,...) IF(I(COND),I(if((COND)) {__VA_ARGS__} else do;while(0)))
#define CMACROIF_ELSE(COND,THEN,...) IF(I(COND),if((COND)){THEN}else) MACRO(__VA_ARGS__)
/** Classic macro which allows for jumping out of the macro with break or continue;
 * Use this macro only when users are not intended to provide code statements as arguments. */
#define MACRO(...) do{ __VA_ARGS__ }while(0)
#define MACROIF(COND,...) IF(I(COND),I(do if((COND)){__VA_ARGS__} while(0)))
#define MACROIF_ELSE(COND,THEN,...) IF_ELSE(I(COND),MACRO(if((COND)){THEN}else{__VA_ARGS__}),MACRO(__VA_ARGS__))

/** Removes everything from the variadic arguments until the first unmatched closing parenthesis.
 * It is blank if everything is balanced. Could be used for debugging syntax errors.
 * e.g. REM_BALANCE( A _RPAREN B )  should expand to " B )" */
#define REM_BALANCE(...) BLANK(__VA_ARGS__)

//will generate a compile-time error if condition is not constant 1
#define FAIL_IF(CONDITION, ...) _Static_assert(!(TRY(I(CONDITION),0)), #__VA_ARGS__)
#define FAIL_IFNOT(CONDITION, ...) _Static_assert(TRY(I(CONDITION),0), #__VA_ARGS__)
/** returns 1, if ARG has the expected TYPE otherwise returns __VA_ARGS__. There is no
 * default case if __VA_ARGS__ is blank.
 *  You can use it as IS_TYPE(ARG, TYPE) . */
#define IS_TYPE(ARG,TYPE,...) _Generic((ARG), TYPE: 1 IF(I(__VA_ARGS__),I(, default: __VA_ARGS__)) )

/** Allows for ignoring some annoying warning whose name is given as first argument (with initial hyphen!)
 * for the code which is given after the first argument. It can be any code but it maybe doesn't work
 * within initializer lists but within expression statements from GCC within compound literals. */
#define IGNORE_WARNING(WNAME)\
	_Pragma("GCC diagnostic push")\
	PRAGMA(GCC diagnostic ignored "-W" STRING(WNAME))
#define END_IGNORE\
	_Pragma("GCC diagnostic pop")

/** you can used this to embed expressions into the variadic argument. This macro is only allowed in
 * function bodies. */
#define IGNORE_WARNING_EXPR(WNAME, ...) ({\
	_Pragma("GCC diagnostic push")\
	PRAGMA(GCC diagnostic ignored "-W" STRING(WNAME))\
	__VA_ARGS__;\
	_Pragma("GCC diagnostic pop")\
})

/** optmizes the code given after first argument with the optimization level given in the first argument.
 * You must give WHOLE top-level functions as code to optimize!
 * @warning GCC WILL BEHAVE UNEXPECTEDLY IF YOU PLACE THIS MACRO IN ANOTHER MACRO!
 * 			ANY FUNCTION DEFINITION IN THE SAME MACRO EXPANSION AFTER IT WILL BE IGNORED!
 * */
#define OPTIMIZE(OLEVEL)\
	_Pragma("GCC push_options")\
	PRAGMA(GCC optimize ("O" STRING(OLEVEL)))
/** needed for every OPTIMIZE macro to end the scope of optimization */
#define END_OPTIMIZE _Pragma("GCC pop_options")

#endif
