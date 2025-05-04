/** Functional programming API with Preprocessor */
//all macros have been tested and can be used in production

/* NOTE! If you ever get a parenthesis syntax error (missing them or too much), or macro pasting error
 * of a closing parenthesis via CONC or CONCIF, then the reason probably is that a nested macro occurred.
 * One common reason is to use the same ...I() function for two arguments of a loop macro call.
 * Nestings can happen quickly when using the same loop basis in macro arguments of loops (see CARTESIAN).
 * For example calling MAPX within another MAP routine. This problem can be circumvented by delaying the
 * nested loop call until later by replacing parentheses with macros or escaping calls with commas
 * which are deleted afterwards. Also watch out that the argument expansion during a macro call will not
 * generate the a nested call which then is called after argument expansion. (Nested calls must always
 * be fully expanded during argument expansion to work.)
 * You can also work around the problem by cloning the macro-chain with a new prefix and use that. If the
 * non-expanded macros in the final macro expansion don't contain any unbalanced parentheses then you can just
 * put the final expansion into I(...) to evaluate the final expansion again.
 * */
#ifndef MACRO_LOOPS_H
#define MACRO_LOOPS_H
#include "macro_programming.h"
/** if ARG is one of the types in the variadic TYPES set it will evaluate to 1, otherwise to ERROR.
 * The default value for ERROR is 0. */
#define IS_TYPES(ARG,THEN,ERROR,TYPES...)\
	_Generic((ARG) _MAP(F_IS_TYPE,_MAPI0, THEN, TYPES) IF(I(ERROR),I(, default: ERROR)))
#define F_IS_TYPE(A,...) IF(A,I(, A: TRY(I(__VA_ARGS__),1)))

/** FUNC(A,AKK) is called on each element of __VA_ARGS__ (even blank ones) and the current AKK
 * to produce the next AKK. AKK is allowed to include unparenthesized commas unlike FPAREN in
 * _MAP or _MAPX. Last expanded AKK is the entire expansion result.
 * FUNC can also be a real function name which allows for nice nesting! */
#define REDUCE2(FUNC,AKK,...) _REDUCE2(FUNC,(AKK),__VA_ARGS__)
#define _REDUCE2(FUNC,AKK,...) _REPEATN(ARGNUM(__VA_ARGS__),F_REDUCE,F_REDUCESTEP,((__VA_ARGS__),FUNC,AKK),)
/* if it does not work with REDUCE (if FUNC itself calls REPEAT cycles), it could work with REDUCE2.
 * REDUCE2 has the drawback, that it will not expand to the result if last argument is blank. */
#define REDUCE(FUNC,AKK,...) _REDUCE(FUNC,(AKK),__VA_ARGS__)
#define _REDUCE(FUNC,AKK,...) _MAPN(ARGNUM(__VA_ARGS__),F_REDUCE,F_REDUCESTEP,((__VA_ARGS__),FUNC,AKK),__VA_ARGS__)
#define F_REDUCE(_,ARGS) _F_REDUCE ARGS
//if only one arg left, expand to FUNC(arg,AKK)
#define _F_REDUCE(ARGS,FUNC,AKK) APPLYIFNOT(_DROP0 ARGS,FUNC,(_TAKE0 ARGS,_REDUCEI AKK))
#define _REDUCEAPPLY(A,B) A B
#define F_REDUCESTEP(ARGS) _REDUCESTEP ARGS
#define _REDUCESTEP(IDX,FUNC,AKK) ((_DROP IDX),FUNC,(FUNC(_TAKE0 IDX,_REDUCEI AKK)))
#define _REDUCEI(...) __VA_ARGS__

//debugging version, it expands to every loop cycle with index
#define REDUCE2_DBG(FUNC,AKK,...) _REDUCE2_DBG(FUNC,(AKK),__VA_ARGS__)
#define _REDUCE2_DBG(FUNC,AKK,...) _REPEATN(ARGNUM(__VA_ARGS__),F_REDUCE_DBG,F_REDUCESTEP_DBG,(PP0,(__VA_ARGS__),FUNC,AKK),)
#define REDUCE_DBG(FUNC,AKK,...) _REDUCE_DBG(FUNC,(AKK),__VA_ARGS__)
#define _REDUCE_DBG(FUNC,AKK,...) _MAPN(ARGNUM(__VA_ARGS__),F_REDUCE_DBG,F_REDUCESTEP_DBG,(PP0,(__VA_ARGS__),FUNC,AKK),__VA_ARGS__)
#define F_REDUCESTEP_DBG(ARGS) _REDUCESTEP_DBG ARGS
#define _REDUCESTEP_DBG(IDX,ARGS,FUNC,AKK) (ADD_ONE(IDX),(_DROP0 ARGS),FUNC,(FUNC(_TAKE0 ARGS,_REDUCEI AKK)))
#define F_REDUCE_DBG(_,ARGS) _F_REDUCE_DBG ARGS
#define _F_REDUCE_DBG(IDX,ARGS,FUNC,AKK) (COUNT_N(IDX): FUNC,(_TAKE0 ARGS,_REDUCEI AKK) = FUNC(_TAKE0 ARGS,_REDUCEI AKK)),

/** IMPORTANT! This unary number defines the maximum number of used loop cycles per loop call.
 *
 * I don't know why but every increase of the limit significantly increases Ubuntu startup time of
 * Atollic IDE but only if the initially displayed file contains a complex macro expansion, FLATTEN.
 * I don't know if this is an Atollic IDE bug on Linux-systems. Simply close the files before quitting.
 * When not displayed on startup, there ain't any lag problems.
 *
 * The number of available _MAPx lines specifies maximum number of supported list elements. The time
 * and overhead needed for each expansion of any _MAPX macro depends on LOOPLIMIT which must be in bounds.
 * However, the number of lines determine the MAXLIMIT of deepness/cycles that loops can handle.
 * If you increase the number of _MAPx lines you also have to add arguments to ARGNUM in macro_magic.h.
 */
#define LOOPLIMIT ADD(PP10,PP6)
//#define LOOPLIMIT LOOPMAX
//maximum number of loop cycles as unary number
#define LOOPMAX (_LOOPMAXDROP(_ARGNUMLIST))
//maximum number of loop cycles as integer literal
#define LOOPMAXN TAKE(_ARGNUMLIST)
#define _LOOPMAXDROP(...) _DROP0(__VA_ARGS__)
//unsaturated unary to decimal, undefined behaviour for overflow over LOOPMAX
#define COUNT_N(LIMIT) ARGNUM LIMIT
/**
 * These _MAP loop cycles are the basis for any other loop macros. They will always execute all cycles
 * until the last _MAP cycle. _MAPX itself will execute LOOPLIMIT loop cycles starting excluding the first.
 * The first is left unchanged. _MAP will also execute LOOPLIMIT cycles but starting with the
 * 1st element.
 * @param[in] FUNC will be applied to each element passing the element and the current FPAREN argument as
 * 		2nd argument. Suitable predefined functions have been prefixed with F_ .
 * @param[in] CHG is a function which changes the FPAREN value after each _MAPx call, it only uses FPAREN as argument
 * @param[in] FPAREN is a 2nd argument for FUNC, the name originates in the use as parenthesis-functions for LMAP and RMAP
 * @param[in] A, ... the list of arguments */
#define MAPX(FUNC,...) _MAPXN(ARGNUM(_MAPI0(__VA_ARGS__)),FUNC,_MAPI0,_MAPI1, __VA_ARGS__)
#define _MAPX(FUNC, CHG, FPAREN, ...) _MAP_2ND(1,CONC(_MAP,COUNT_N(SUB_ONE(LOOPLIMIT))), FUNC,CHG,FPAREN,__VA_ARGS__)
#define _MAP_2ND(COND,MAPSTART, FUNC, CHG, FPAREN, A, ...) IF(COND,A) MAPSTART(FUNC,CHG,FPAREN,__VA_ARGS__)
#define _MAP_2ND0

/** applies a macro FUNC to every list element after it and does not insert commas in between*/
#define MAP(FUNC, ...) _MAPN(ARGNUM(_MAPI0(__VA_ARGS__)),FUNC,_MAPI0,_MAPI1, __VA_ARGS__)
#define _MAPI0(...) __VA_ARGS__
#define _MAPI1(...) __VA_ARGS__
/*	Did you know? FPAREN also can be a real function name and used together with F_COMMA_N or
 * F_INSTR you can generate function calls in a chain. */
#define _MAP(FUNC, CHG, FPAREN, ...) _MAP_1ST(CONC(_MAP,COUNT_N(LOOPLIMIT)), FUNC,CHG,FPAREN,__VA_ARGS__)
#define _MAP_1ST(MAPSTART, FUNC, CHG, FPAREN, ...) MAPSTART(FUNC,CHG,FPAREN,__VA_ARGS__)
//generalized MAP and MAP interface
#define _MAPXN(N, FUNC, CHG, FPAREN, ...) _MAP_2ND(CONC(_MAP_2ND,N),CONC(_MAP,N), FUNC,CHG,FPAREN,__VA_ARGS__)
#define _MAPN(N, FUNC, CHG, FPAREN, ...) _MAP_1ST(CONC(_MAP,N), FUNC,CHG,FPAREN,__VA_ARGS__)

#define _MAP30(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP29(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP29(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP28(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP28(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP27(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP27(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP26(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP26(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP25(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP25(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP24(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP24(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP23(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP23(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP22(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP22(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP21(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP21(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP20(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP20(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP19(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP19(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP18(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP18(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP17(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP17(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP16(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP16(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP15(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP15(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP14(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP14(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP13(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP13(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP12(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP12(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP11(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP11(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP10(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP10(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP9(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP9(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP8(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP8(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP7(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP7(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP6(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP6(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP5(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP5(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP4(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP4(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP3(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP3(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP2(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP2(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN)) _MAP1(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _MAP1(FUNC, CHG, FPAREN, A, ...)\
	CALLIF((A,##__VA_ARGS__),FUNC,(A,FPAREN))
#define _MAP0(...)

//without conditional execution
#define _REPEAT(FUNC,CHG,FPAREN,...) _REPEAT_1ST(CONC(_REPEAT,COUNT_N(LOOPLIMIT)), FUNC,CHG,FPAREN, __VA_ARGS__)
#define _REPEATN(COUNT,FUNC,CHG,FPAREN,...) _REPEAT_1ST(CONC(_REPEAT,COUNT), FUNC,CHG,FPAREN, __VA_ARGS__)
#define _REPEAT_1ST(REPEATSTART, FUNC,CHG,FPAREN, ...) REPEATSTART(FUNC,CHG,FPAREN,__VA_ARGS__)

#define _REPEATX(FUNC,CHG,FPAREN,...) _REPEAT_2ND(1,CONC(_REPEAT,COUNT_N(SUB_ONE(LOOPLIMIT))), FUNC,CHG,FPAREN, __VA_ARGS__)
#define _REPEATXN(COUNT,FUNC,CHG,FPAREN,...) _REPEAT_2ND(CONC(_REPEAT_2ND,COUNT),CONC(_REPEAT,DEC_SUB(COUNT,1)), FUNC,CHG,FPAREN, __VA_ARGS__)
#define _REPEAT_2ND(COND,REPEATSTART, FUNC,CHG,FPAREN, A, ...) IF(COND,A) REPEATSTART(FUNC,CHG,FPAREN,__VA_ARGS__)
#define _REPEAT_2ND0
#define _REPEATI0(...) __VA_ARGS__
#define _REPEATI1(...) __VA_ARGS__

#define _REPEAT30(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT29(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT29(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT28(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT28(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT27(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT27(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT26(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT26(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT25(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT25(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT24(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT24(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT23(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT23(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT22(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT22(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT21(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT21(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT20(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT20(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT19(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT19(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT18(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT18(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT17(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT17(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT16(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT16(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT15(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT15(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT14(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT14(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT13(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT13(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT12(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT12(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT11(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT11(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT10(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT10(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT9(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT9(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT8(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT8(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT7(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT7(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT6(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT6(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT5(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT5(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT4(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT4(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT3(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT3(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT2(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT2(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN) _REPEAT1(FUNC, CHG, CHG(FPAREN), __VA_ARGS__)
#define _REPEAT1(FUNC, CHG, FPAREN, A, ...) FUNC(A, FPAREN)
#define _REPEAT0(...)

/* a more efficient loop idea, which I came up with only late.
 * The disadvantage: you can't address loop cycles linearly, so the two loops above are still useful.
 * @param[in] MFUNC - MAP macro, is applied to each individual element from __VA_ARGS__ together
 * 						with current AKK. It's expansion will appear in the final loop expansion.
 * @param[in] RFUNC - MAP macro, is applied to individual __VA_ARGS__ element and AKK and expands
 * 						to the new AKK value. Must not contain unparenthesized commas
 * @param[in] AKK - akkumlator value which is an intermediate result that is not thought to appear in
 * 					the final expansion result of the loop (you can still retrieve the result when
 * 					using _TREELOOP()).
 * @param[in] ... - elements which are iterated over. It will loop a constant number of times so
 * 					it will ignore the actual number of given elements and use blank, if no next
 * 					element is available anymore. */
#define TREELOOP(MFUNC,RFUNC,AKK,...) _TREELOOPDROP(_TREELOOP(MFUNC,TRY(RFUNC,_TREEI0),TRY(AKK,_TREEI1),(__VA_ARGS__)))
#define _TREELOOPDROP(...) _TREELOOPDROP0(__VA_ARGS__)
#define _TREELOOPDROP0(AKK,LIST,...) __VA_ARGS__
#define _TREELOOP(MFUNC,RFUNC,AKK,LIST) _TREECYC4(MFUNC,RFUNC,AKK,LIST,)
#define _TREEI0(...) __VA_ARGS__
#define _TREEI1(...) __VA_ARGS__

//TREELOOP with exclusive first elemment
#define TREELOOPX(MFUNC,RFUNC,AKK,A,...) A TREELOOP(MFUNC,RFUNC,AKK,__VA_ARGS__)

#define _TREECYC4(MFUNC,RFUNC,...) _TREECYC3(MFUNC,RFUNC,_TREECYC3(MFUNC,RFUNC,_TREECYC3(MFUNC,RFUNC,__VA_ARGS__)))
#define _TREECYC3(MFUNC,RFUNC,...) _TREECYC2(MFUNC,RFUNC,_TREECYC2(MFUNC,RFUNC,_TREECYC2(MFUNC,RFUNC,__VA_ARGS__)))
#define _TREECYC2(MFUNC,RFUNC,...) _TREECYC1(MFUNC,RFUNC,_TREECYC1(MFUNC,RFUNC,_TREECYC1(MFUNC,RFUNC,__VA_ARGS__)))
#define _TREECYC1(MFUNC,RFUNC,...) _TREECYC(MFUNC,RFUNC,__VA_ARGS__)
#define _TREECYC(MFUNC,RFUNC,AKK,LIST,...) _TREECYCTAKE(RFUNC(AKK)),(_DROP0 LIST),__VA_ARGS__ MFUNC(_TAKE0 LIST,AKK)
#define _TREECYCTAKE(...) _TAKE0(__VA_ARGS__)
#define _TREECYC0(MFUNC,RFUNC,...)


//call MAP function with left/right-associativity (first or last reduction is evaluated first)
#define LMAP(FUNC, A, ...) MAP(F_LPAR_IF, __VA_ARGS__) _MAPX(FUNC,_MAPI0,_RPAREN,A,## __VA_ARGS__)
#define RMAP(FUNC, ...) (_MAPX(FUNC,_MAPI0,_LPAREN, __VA_ARGS__) MAP(F_RPAR_IF, __VA_ARGS__)
#define _RPAREN(...) __VA_ARGS__)
#define _LPAREN(...) (__VA_ARGS__
#define F_RPAR_IF(A, ...) CONCIF(A,_,RPAR) /*generates closing parentheses if A not blank*/
#define F_LPAR_IF(A, ...) CONCIF(A,_,LPAR)
#define _LPAR (
#define _RPAR )
#define F_LPAR(...) (
#define F_RPAR(...) )
//allows for concatenating a variable number of arguments together to a single preprocessing symbol
#define CAT(...) CONCAT(F_ID, __VA_ARGS__)
//concatenates the symbols resulting from FUNC applied to each list element
#define CONCAT(FUNC, ...) _CONCATI(_MAP(F_CONCAT,_CONCATI,FUNC,__VA_ARGS__)MAP(F_RPAR_IF, __VA_ARGS__))
#define F_CONCAT(A, ...) APPLYIF(A,_CONCATCONC,(A,__VA_ARGS__))
#define _CONCATCONC(A,...) _CONCATCONC0 _LPAR __VA_ARGS__(A),
#define _CONCATCONC0(...) _CONCATCONC1(__VA_ARGS__)
#define _CONCATCONC1(A,...) A ## __VA_ARGS__
#define _CONCATI(...) __VA_ARGS__
//basically like CONCAT but stringifies the results
#define STRCAT(FUNC, ...) STRING(CONCAT(FUNC, __VA_ARGS__))
//you can define fonts for STRMAP where your FONT macro concatenates A to a variable prefix which gives
//the string contents for the character
#define STRMAP(FONT, ...) _MAP(F_STR,_MAPI0,FONT, __VA_ARGS__)

//lets you remove all the arguments which expand to blank when given as argument to the predicate FUNC
#define FILTER(FUNC, ...) _FILTERDROP(_MAP(F_IF,_MAPI0,FUNC,__VA_ARGS__))
#define _FILTERDROP(...) _DROP0(__VA_ARGS__)
#define F_IF(A,...) IF(AND(A,I(__VA_ARGS__(A))),I(, A))
//like FILTER but removes arguments for the opposite condition when they expand to non-blank
#define FILTER_OUT(FUNC, ...) _FILTERDROP(_MAP(F_IFNOT,_MAPI0,FUNC,__VA_ARGS__))
#define F_IFNOT(A,...) IFNOT(OR(_NOT(A),I(__VA_ARGS__(A))),I(, A))

/* iterates over the pairs of equally-indexed elements from LIST and __VA_ARGS__ and applies FUNC to each
 * pair.
 * @param[in] FUNC - is applied to each pair of (LIST[i],__VA_ARGS__[i])
 * @param[in] LIST - parenthesized (2nd) list of arguments (right argument of FUNC)
 * @param[in] ... - 1st list of arguments (left argument of FUNC) */
#define ZIP(FUNC,LIST,...) _ZIP(FUNC,IF_ELSE(DROP_N(LIST,__VA_ARGS__),_ZIPI(F_ZIP1,LIST,__VA_ARGS__),_ZIPI(F_ZIP2,(__VA_ARGS__),_ZIPI LIST)))
#define _ZIP(FUNC,...) _ZIP0(FUNC,__VA_ARGS__)
#define _ZIP0(FUNC,ZIPFUNC,LIST,...) _MAP(ZIPFUNC,_ZIPDROP,(FUNC,LIST),__VA_ARGS__)
#define _ZIPI(...) __VA_ARGS__
#define _ZIPDROP(ARGS) _ZIPDROP0(_TAKE0 ARGS, _DROP0 ARGS)
#define _ZIPDROP0(FUNC,LIST) (FUNC,(_DROP0 LIST))
#define F_ZIP1(A,ARGS) _ZIP1(A,_TAKE0 ARGS,_DROP0 ARGS)
#define _ZIP1(A,F,LIST) APPLYIF(OR(A,_TAKE0 LIST),F,(A,_TAKE0 LIST))
#define F_ZIP2(A,ARGS) _ZIP2(A,_TAKE0 ARGS,_DROP0 ARGS)
#define _ZIP2(A,F,LIST) APPLYIF(OR(A,_TAKE0 LIST),F,(_TAKE0 LIST,A))

//this is a "ZIP bomb" XD!! It can crash your PC with Atollic because _TAKE0 AND _DROP0 are undefined here!
//int x = ZIP(CONC,(1,2),1,2);	//DON'T UNCOMMENT!!

/** For all initial non-blank arguments in parenthesized list N, it will apply FUNC to __VA_ARGS__.
 * The current result of the last FUNC application will be returned as expansion result after N cycles.
 * If N > available number of cycles, the expansion result will be blank.
 * @param[in] FUNC - loop body which is executed every loop cycle. It takes the last loop expansion
 * 						as arguments (which will be __VA_ARGS__ for the first loop cycle).
 * @param[in] N - unary number of loop cycles. The loop expansion, preceding the first blank argument in
 * 					N, is the result of the macro.
 * @param[in] ... - the first arguments to FUNC. If the first argument of N is blank, this would be
 * 					expanded. */
#define LOOP_N(FUNC,N,...) _LOOP((FUNC,(__VA_ARGS__)),ARGNUM N,_LOOPI N)
//variant which takes an integer literal for N, not a unary number. Might run slower
#define LOOP(FUNC,N,...) _LOOP((FUNC,(__VA_ARGS__)),N,TAKEN(N,_ARGNUMLIST))
#define _LOOP(ARGS,COUNT,...)  /*END_ is undefined and prevents removal of commas before*/\
	_LOOPTAKE(F_LOOP(_TAKE0(__VA_ARGS__),ARGS) _REPEATN(COUNT,F_LOOP,_LOOPCYCLE,_LOOPCYCLE(ARGS),_DROP0(__VA_ARGS__),,END_))
#define _LOOPTAKE(...) _LOOPTAKE0(__VA_ARGS__) /*_DROP0 does not work*/
#define _LOOPTAKE0(A,...) APPLYIF(A,_LOOPI,A)
#define _LOOPI(...) __VA_ARGS__
#define F_LOOP(COND,VALUE) APPLYIFNOT(COND,_LOOP0,VALUE)
#define _LOOP0(F,RESULT) RESULT,
#define _LOOPCYCLE(ARGS) (_TAKE0 ARGS,(APPLY(_TAKE0 ARGS,_DROP0 ARGS)))
/** use LOOP_DBG and LOOP_DBG_N for debugging. Every intermediate result is expanded with index. */
#define LOOP_DBG_N(FUNC,N,...) _LOOP_DBG_N((FUNC,PP0,(__VA_ARGS__)),ARGNUM N,_LOOPI N)
#define LOOP_DBG(FUNC,N,...) _LOOP_DBG_N((FUNC,PP0,(__VA_ARGS__)),N,TAKEN(N,_ARGNUMLIST))
#define _LOOP_DBG_N(ARGS,COUNT,...)\
	F_LOOP_DBG(_TAKE0(__VA_ARGS__),ARGS) _REPEATN(COUNT,F_LOOP_DBG,_LOOPCYCLE_DBG,_LOOPCYCLE_DBG(ARGS),_DROP0(__VA_ARGS__),,END_)
#define F_LOOP_DBG(COND,VALUE) _LOOP_DBG0(COND,_DROP0 VALUE)
#define _LOOP_DBG0(COND, ARGS) (COUNT_N(_TAKE0(ARGS)): COND? _DROP0(ARGS)),
#define _LOOPCYCLE_DBG(ARGS) _LOOPCYCLE_DBG0 ARGS
#define _LOOPCYCLE_DBG0(F,IDX,ARGS) (F,ADD_ONE(IDX),(F ARGS))

//will be blank, if more elements than maximum limit of loop cycle is in list
//use SATURATE() on the argument list to cut away exceeding elements to always get a result
#define REVERSE(...) _REVERSEDROP(LOOP_N(F_REVERSE,(__VA_ARGS__),(__VA_ARGS__)))
#define _REVERSEDROP(...) _DROP0(__VA_ARGS__)
#define F_REVERSE(...) _REVERSE __VA_ARGS__
#define _REVERSE(A,...) (__VA_ARGS__),A

//#define TO_UNARY(NATURAL) (_TO_UNARYDROP(LOOP(F_UNARY_INC,NATURAL,)))
#define TO_UNARY(NATURAL) _TO_UNARY(CONC(PP,NATURAL),NATURAL)
#define _TO_UNARY(UNARY,NATURAL) APPLYIF(BLANK UNARY,_TO_UNARY0,(NATURAL))IFNOT(BLANK UNARY,UNARY)
#define _TO_UNARY0(NATURAL) (REPEATN(NATURAL,1))

/** takes two integer literals START <= END and generates an increasing list. Will at least have one element.
 *  END's maximum is LOOPMAX */
#define RANGE(START,END) _RANGEDROP(_RANGEVARGS(RANGEX(START,END)),END)
//version with exclusive END
#define RANGEX(START,END) DROPN(START,TAKEN(END,REVERSE(_RANGEDROP(_ARGNUMLIST))))
#define _RANGEVARGS(...) _RANGEVARGS0(__VA_ARGS__)
#define _RANGEVARGS0(...) ,##__VA_ARGS__
#define _RANGEDROP(...) _DROP0(__VA_ARGS__)
/** takes two integer literals START >= END and generates a decreasing list. Will at least have on element.
 * Dropping/Taking the last elements a list indeed takes more overhead than reversing it and dropping from. */
#define RANGEDOWN(START,END) REVERSE(RANGE(END,START))
#define RANGEDOWNX(START,END) REVERSE(RANGEX(END,START))
/* using DROP_UPPERN, DROP_LAST and TAKE_UPPERN macros together takes more overhead than
 * using REVERSE twice with DROPN and TAKEN! */

/** Implements a while Loop in the preprocessor. In each cycle it executes FUNC with the result
 * of the previous cycle. It will stop when PREDICATE applied to the result is blank. */
#define WHILE(PREDICATE,FUNC,...) _WHILE((PREDICATE,FUNC,(__VA_ARGS__)),UNGROUP(LOOPLIMIT))
#define _WHILE(ARGS,...) _WHILETAKE(_MAP(F_WHILE,_WHILECYCLE,ARGS,__VA_ARGS__))
#define _WHILETAKE(...) _WHILETAKE0(__VA_ARGS__)
#define _WHILETAKE0(A,...) APPLYIF(A,_WHILEI,A)
#define _WHILEI(...) __VA_ARGS__
#define F_WHILE(_,ARGS) _WHILE0 ARGS
#define _WHILE0(PREDICATE,FUNC,ARGS) IFNOT(PREDICATE ARGS,I(ARGS,))
#define _WHILECYCLE(ARGS) _WHILECYCLE0 ARGS
#define _WHILECYCLE0(PREDICATE,FUNC,ARGS) (PREDICATE,FUNC,(_WHILEI(IF_ELSE(PREDICATE ARGS,FUNC,_WHILEI)ARGS)))
/** use this loop for debugging. It expands the intermediate result with index in every cycle. */
#define WHILE_DBG(PREDICATE,FUNC,...) _WHILE_DBG((PREDICATE,FUNC,PP0,PP1,(__VA_ARGS__)),UNGROUP(LOOPLIMIT))
#define _WHILE_DBG(ARGS,...) _MAP(F_WHILE_DBG,_WHILECYCLE_DBG,ARGS,__VA_ARGS__)
#define F_WHILE_DBG(_,ARGS) _WHILE_DBG0 ARGS
#define _WHILE_DBG0(P,F,IDX,C,ARGS) IF(_WHILEI C,_WHILEI((ARGNUM IDX: ARGS),))
#define _WHILECYCLE_DBG(ARGS) _WHILECYCLE_DBG0 ARGS
#define _WHILECYCLE_DBG0(P,F,IDX,C,ARGS) (P,F,ADD_ONE(IDX),(P ARGS),(_WHILEI(IF_ELSE(P ARGS,F,_WHILEI)ARGS)))
/** will return true for WHILE as long as the next list element is not the END symbol and not blank
 *  you can only use this, if all arguments start with a letter that can be concatenated. */
#define P_NOEND(...) NOTSTARTSWITH(_WHILE,__VA_ARGS__)
#define _WHILE_
#define _WHILE_END_
//returns current value. When treated as boolean it will be true if non-blank otherwise false.
#define P_NOBLANK(A,...) A
#define P_NOEMPTY(A,B,...) BOOL(__VA_ARGS__)
#define ITER_NEXT(...) F_ROTATE(__VA_ARGS__)
//use this to directly return the elements after from ITERATE for more readability
#define ITER_RETURN ,END_,
/** allows to iterate over the argument list with a FUNC macro that changes
 *  the current argument list in each iteration (usually only the first element
 *  which is the current one). It will stop the iteration when the CURRENT ELEMENT IS EMPTY.
 *  You have to use ITER_NEXT in your FUNC macro to get the next list element in a list
 *  of elements. It will stop automatically if the first list element
 *  is blank. If it will not reach the end within the LIMIT of _MAPx
 *  cycles, it will expand to blank (which could mean that you have an endless loop). */
#define ITERATE(FUNC,...) _ITERATE(WHILE(P_NOBLANK,FUNC,__VA_ARGS__,,END_))
#define _ITERATE(...) APPLYIF(I(__VA_ARGS__),_ITERATE0,(__VA_ARGS__))
#define _ITERATE0(A,B,...) __VA_ARGS__
//debugging
#define ITER_DBG(FUNC,...) WHILE_DBG(P_NOBLANK,FUNC,__VA_ARGS__,,END_)

/** Repeats __VA_ARGS__ N times with a comma in between the repetitions. */
#define REPEATN(N,...) _REPEAT_NDROP(_REPEATN(N,F_COMMA_N,_REPEATI0,(__VA_ARGS__)))
#define REPEAT_N(N,...) _REPEAT_NDROP(_REPEATN(ARGNUM N,F_COMMA_N,_REPEATI0,(__VA_ARGS__)))
#define _REPEAT_NDROP(...) _DROP0(__VA_ARGS__)

/** will set the length of a given argument list to length given by decimal literal N by either
 * adding empty elements or deleting elements */
#define ADJUST(N, ...) _REPEATXN(N,F_COMMA,_MAPI0,_MAPI1,__VA_ARGS__)

/** turns a parenthesized function parameter list into a list of aggregate type member fields. */
#define PARAMS_TO_MEMBERS(...) MAP(F_INSTR,UNGROUP(__VA_ARGS__))
//generates N commas, using a unary N as argument
#define EMPTYARGS(N) REPEAT_N(N,)
//generates N commas, using an integer literal as N
#define EMPTYARGSN(N) REPEATN(N,)
/* removes all remaining empty arguments up to END_ if you don't know in every case how many empty
 * args are left. This is useful after ITERATE calls. N gives the maximum number of args to drop. */
#define DROPTAIL(N, ...) WHILE(P_DROPTAIL,F_DROPTAIL,TAKEN(N,__VA_ARGS__)) VARGS(DROPN(N,__VA_ARGS__))
#define P_DROPTAIL(A,...) STARTSWITH(_WHILE,A)
#define F_DROPTAIL(A,...) __VA_ARGS__

/* tries to remove all nested parentheses from the argument list. It can only reach parentheses which
 * start the element. */
#define FLATTEN(...) ITERATE(F_FLATTEN,__VA_ARGS__)
//if starting with pair of paren: remove, else: go to next element
#define F_FLATTEN(...) _FLATTEN(STARTSGROUP(__VA_ARGS__),__VA_ARGS__)
#define _FLATTEN(COND,...) APPLYIF(COND,_FLATTEN0,(_FLATTENI __VA_ARGS__))APPLYIFNOT(COND,ITER_NEXT,(__VA_ARGS__))
#define _FLATTENI(...) __VA_ARGS__
#define _FLATTEN0(A,...) OPT(A)__VA_ARGS__
/** Replaces all starting parentheses of any argument with braces. Useful for initializer generation.*/
/*#define ENBRACE(...) ITERATE(F_ENBRACE,__VA_ARGS__) //replacing nested pairs is more complicated, requires depth-first search on syntax tree
#define F_ENBRACE(...) _ENBRACE(STARTSGROUP(__VA_ARGS__),__VA_ARGS__)
#define _ENBRACE(COND,...) IF(COND,_ENBRACEI(_ENBRACE0 __VA_ARGS__))APPLYIFNOT(COND,ITER_NEXT,(__VA_ARGS__))
#define _ENBRACE0(...) {__VA_ARGS__}
#define _ENBRACEI(...) __VA_ARGS__*/
/** Generates an increasing list, starting at START up to at most END with linear steps of PACE in between. */
#define SLICE(START,PACE,END) _SLICE(PACE,RANGE(START,END))
#define SLICEX(START,PACE,END) _SLICE(PACE,RANGEX(START,END))
#define SLICEDOWN(START,PACE,END) _SLICE(PACE,RANGEDOWN(START,END))
#define SLICEDOWNX(START,PACE,END) _SLICE(PACE,RANGEDOWNX(START,END))
#define _SLICE(PACE,...) DROPTAIL(PACE,ITERATE(F_SLICE,PACE,__VA_ARGS__ EMPTYARGSN(PACE)))
#define F_SLICE(PACE,A,...) IF_ELSE(A,I(PACE,ITER_NEXT(A,DROPN(PACE,A,__VA_ARGS__))),I(A,__VA_ARGS__))

#define NAME_ALLOWED _ //only for NOT_NAMESPACE
/** Checks that all arguments are in the namespace with the PREFIX. Returns 1, iff true.
 * Exits the loop when all arguments were processed. It should not finish, if a word is not contained
 * in the namespace so that the expansion is blank then. If a name must have a successor name
 * in the same argument, define it as a single comma ",".
 * If you want to define *forbidden* names, you must define them as doulbe-comma ",," .*/
#define IS_NAMESPACE(PREFIX,...) _NAMESPDROP(WHILE(P_NOBLANK,F_NAMESP,PREFIX,__VA_ARGS__))
#define _NAMESPDROP(...) _DROP0(__VA_ARGS__)
#define _NAMESPTAKE(...) _TAKE0(__VA_ARGS__)
#define F_NAMESP(PREFIX,...) _NAMESP(BOOL(__VA_ARGS__),PREFIX,__VA_ARGS__)
#define _NAMESP(COND,PREFIX,A,...) APPLYIF(COND,_NAMESP0,((PREFIX ## _ ## A),PREFIX,__VA_ARGS__))IFNOT(COND,_NAMESPI(,1))
#define _NAMESP0(COND,PREFIX,...) IF_ELSE(_NAMESPTAKE COND,_NAMESPI(,),_NAMESPI(PREFIX,_NAMESPDROP(_NAMESPI COND,)__VA_ARGS__))
#define _NAMESPI(...) __VA_ARGS__
//very helpful caveman debugging utility
//#define IS_NAMESPACE_DBG(PREFIX,...) WHILE_DBG(P_NOBLANK,F_NAMESP,PREFIX,__VA_ARGS__)
/** This has the limitation that we can only test whole arguments and not single words.
 * But not entirely true! If you define your names as single comma, "," they allow for checking
 * the next word in the argument too. */
#define NOT_NAMESPACE(PREFIX,...) _NAMESPDROP(WHILE(P_NOBLANK,F_NOT_NAMESP,PREFIX,__VA_ARGS__))
#define F_NOT_NAMESP(PREFIX,...) _NOT_NAMESP(BOOL(__VA_ARGS__),PREFIX,__VA_ARGS__)
#define _NOT_NAMESP(COND,PREFIX,A,...) APPLYIF(COND,_NOT_NAMESP0,((PREFIX ## _ ## A),PREFIX,__VA_ARGS__))IFNOT(COND,_NAMESPI(,1))
#define _NOT_NAMESP0(COND,PREFIX,...) IF_ELSE(_NAMESPTAKE COND,_NAMESPI(PREFIX,_NAMESPDROP(_NAMESPI COND,)__VA_ARGS__),_NAMESPI(,))
/** Checks one argument against one prefix of a parenthesized list of namespaces.
 * It stops directly after every argument from __VA_ARGS__ was checked (which can be less than PREFIXES. */
#define IS_NAMESPACES(PREFIXES,...) _NAMESPDROP(WHILE(P_NOBLANK,F_NAMESPS,PREFIXES,__VA_ARGS__))
#define F_NAMESPS(PREFIX,...) _NAMESPS(BOOL(__VA_ARGS__),PREFIX,__VA_ARGS__)
#define _NAMESPS(COND,PREFIX,A,...) APPLYIF(COND,_NAMESPS0,((CONC(_NAMESPTAKE PREFIX,_ ## A)),PREFIX,__VA_ARGS__))IFNOT(COND,_NAMESPI(,1))
#define _NAMESPS0(COND,PREFIX,...) IF_ELSE(_NAMESPTAKE COND,_NAMESPI(,),_NAMESPI(SUB_ONE(PREFIX),_NAMESPDROP(_NAMESPI COND,)__VA_ARGS__))
/** Checks one argument against one prefix of a parenthesized list of namespaces. */
#define NOT_NAMESPACES(PREFIXES,...) _NAMESPDROP(WHILE(P_NOBLANK,F_NOT_NAMESPS,PREFIXES,__VA_ARGS__))
#define F_NOT_NAMESPS(PREFIX,...) _NOT_NAMESPS(BOOL(__VA_ARGS__),PREFIX,__VA_ARGS__)
#define _NOT_NAMESPS(COND,PREFIX,A,...) APPLYIF(COND,_NOT_NAMESPS0,((CONC(_NAMESPTAKE PREFIX,_ ## A)),PREFIX,__VA_ARGS__))IFNOT(COND,_NAMESPI(,1))
#define _NOT_NAMESPS0(COND,PREFIX,...) IF_ELSE(_NAMESPTAKE COND,_NAMESPI(SUB_ONE(PREFIX),_NAMESPDROP(_NAMESPI COND,)__VA_ARGS__),_NAMESPI(,))
/** tests that the given arguments behave to the PREFIXES like the given NAMES.
 * This macro is usable but it will not work well with names having one of the NAME_... properties above.
 * Name properties change the name list themselves and initially were not thought for this macro.
 * Don't use names with NAME_... properties here and you're fine. */
#define IS_EQUAL(PREFIXES,NAMES,...) _NAMESPDROP(WHILE(P_NOBLANK,F_IS_EQUAL,PREFIXES,NAMES,__VA_ARGS__))
#define F_IS_EQUAL(PREFIX,NAMES,...) _IS_EQUAL(_NOT NAMES,_NOT(__VA_ARGS__),PREFIX,NAMES,__VA_ARGS__)
#define _IS_EQUAL(COND1,COND2,PREFIX,NAMES,A,...)\
	APPLYIFNOT(OR(COND1,COND2),\
		_IS_EQUAL0,((CONC(_NAMESPTAKE PREFIX,CONC(_,_NAMESPTAKE NAMES))),(CONC(_NAMESPTAKE PREFIX,_ ## A)),PREFIX,(_NAMESPDROP NAMES),__VA_ARGS__))\
	IF(OR(COND1,COND2),_NAMESPI(,1/*AND(COND1,COND2)*/))
#define _IS_EQUAL0(COND1,COND2,PREFIX,NAMES,...)\
	IF_ELSE(XNOR(_NAMESPTAKE COND1,_NAMESPTAKE COND2),_NAMESPI(PREFIX,NAMES,_NAMESPDROP(_NAMESPI COND2,)__VA_ARGS__),_NAMESPI(,))
//I found an XNOR bug with this
#define IS_EQUAL_DBG(PREFIXES,NAMES,...) _NAMESPDROP(WHILE_DBG(P_NOBLANK,F_IS_EQUAL,PREFIXES,NAMES,__VA_ARGS__))


#define F_INCR(A, ...) IF(A,+1)
#define F_COMMA_N(A, N) , UNGROUP(N)
#define F_COMMA_IF_N(A, N) IF(_COMMAIFTAKE N,I(, A))
#define _COMMAIFTAKE(A,...) A
#define F_PREPEND(A,...) IF(A,I(, __VA_ARGS__ ## A))
#define F_APPEND(A,...) IF(A,I(, A ## __VA_ARGS__))
#define F_ID(A, ...) APPLYIF(A,__VA_ARGS__,(A))	/*removes commas which can't be made undone*/
#define F_CONC(A,...) , CONC(I(__VA_ARGS__),A)

#define _RFUNC(OP, A, ...) APPLYIF(A,OP I(__VA_ARGS__),(A))
#define _RRFUNC(OP, A, ...) APPLYIF(A,OP A, (__VA_ARGS__))

#define F_PLUS(...) _RFUNC(+, __VA_ARGS__)
#define F_MUL(...) _RFUNC(*, __VA_ARGS__)
#define F_MINUS(...) _RFUNC(-, __VA_ARGS__)
#define F_DIV(...) _RFUNC(/, __VA_ARGS__)
#define F_OR(...) _RFUNC(||, __VA_ARGS__)
#define F_AND(...) _RFUNC(&&, __VA_ARGS__)
#define F_XOR(A, ...) _RFUNC(^, (_Bool)(A),__VA_ARGS__)
#define F_B_OR(...) _RFUNC(|, __VA_ARGS__)
#define F_B_AND(...) _RFUNC(&, __VA_ARGS__)
#define F_B_XOR(...) _RFUNC(^, __VA_ARGS__)
#define F_COMMA(A,...) , __VA_ARGS__(A)
#define F_COMMA_IF(A, ...) APPLYIF(A, I(, __VA_ARGS__),(A))
#define F_CALL(A,...) APPLYIF(A, __VA_ARGS__,());
#define F_INSTR(A,...) __VA_ARGS__(A);
#define F_STR(A,...) APPLYIF(A,STRING,(__VA_ARGS__(A)))

//you don't need SPREAD macro for these
/* this should be prefixed with a type */
#define DEFINE(...) _DEFINE0(__VA_ARGS__)
#define _DEFINE0(VALUE, A, ...) _IF(A, A = (VALUE)) _MAP(F_DEFINE, _MAPI0, VALUE, __VA_ARGS__)
#define F_DEFINE(A, VAL) IF(A, I(, A = (VAL)))
/* this should not be prefixed with a type */
#define ASSIGN(...) _ASSIGN0(__VA_ARGS__)
#define _ASSIGN0(...) _MAPX(F_ASSIGN, _MAPI0,, __VA_ARGS__)
#define F_ASSIGN(A, ...) IF(A, = A)

#define F_SORTED(A, CMP) APPLYIF(A,_SORTED0,(CMP (A),(A)))
#define _SORTED0(A,B) A) && (B
//LIMIT specifies the value which should be compared with the last argument
#define ISSORTED(CMP, ...) (_MAPX(F_SORTED,_MAPI0,CMP, __VA_ARGS__)|1)

//DROPs the N first elements from the list. N must be a unary number represented as (1,1,...,1) with N-times 1
#define DROP_N(N, ...) _DROP_N(ARGNUM N,__VA_ARGS__)
//same as DROP_N but uses a decimal literal as N, not a unary number
#define DROPN(N, ...) _DROP_N(N,__VA_ARGS__)
#define _DROP_N(COUNT,...) _MAPI0(_REPEATN(COUNT,F_DROP,_MAPI0,,) __VA_ARGS__ _REPEATN(COUNT,F_RPAR,_MAPI0,,))
#define F_DROP(...) _DROPNSPREAD _LPAREN(_DROP0),
#define _DROPNSPREAD(F,...) F(__VA_ARGS__)

#define _DROP0(A,...) __VA_ARGS__

//you don't need to use SPREAD, added layer of indirection instead
#define TAKE_N(N, ...) _TAKE_N(ARGNUM N,__VA_ARGS__)
//uses an integer literal instead of unary number
#define TAKEN(N, ...) _TAKE_N(N, __VA_ARGS__)
#define _TAKE_N(COUNT,...) _TAKE_NDROP(_REPEATN(COUNT,F_COMMA_IF, _MAPI0,_MAPI1, __VA_ARGS__))
#define _TAKE_NDROP(...) _DROP0(__VA_ARGS__)
#define _TAKE0(A,...) A
//cuts away all exceeding elements that exceed the maximum number of available loop cycles
#define SATURATE(...) TAKE_N(LOOPMAX, __VA_ARGS__)
/** extracts a number of elements from a list. It starts at element index given by N and extracts M elements.
 * Both numbers must be represented as unary numbers using parenthesized lists of ones */
#define EXTRACT(N,M,...) TAKE_N(M, DROP_N(N, __VA_ARGS__))


//rotate the argument list N times to the left (N is a unary number)
#define LROTATE(N,...) LOOP_N(F_ROTATE,N,__VA_ARGS__)
#define F_ROTATE(A,...) __VA_ARGS__,A

/* operations on the last elements. If you need multiple of those at once,
 * it is often faster to use REVERSE twice and operate with normal DROP and TAKE in between. */
//drops the last element of the list with significantly less overhead than DROP_UPPER().
#define DROP_LAST(...) _DROP_LAST(__VA_ARGS__)
#define _DROP_LAST(...) TAKE_N((__VA_ARGS__),,__VA_ARGS__)
//removes the uppermost unary N elements from the list. Mind the list length limit. Everything above is cutoff.
#define DROP_UPPER(N,...) DROP_N(N,TAKE_N((__VA_ARGS__),_DROP_UPPERI N,__VA_ARGS__))
#define _DROP_UPPERI(...) __VA_ARGS__
//like DROP_UPPER but with integer literal for N instead of unary number
#define DROP_UPPERN(N,...) DROPN(N,TAKEN(ARGNUM(__VA_ARGS__),TAKEN(N,_ARGNUMLIST),__VA_ARGS__))
//returns the last element from the argument list as expansion result
#define TAKE_LAST(...) DROPN(ARGNUM(__VA_ARGS__),,##__VA_ARGS__)
#define TAKE_UPPER(N,...) _TAKE_UPPER(_TAKE_UPPERI N, __VA_ARGS__)
#define _TAKE_UPPERI(...) __VA_ARGS__
#define _TAKE_UPPER(N,...) DROP_N((__VA_ARGS__),N,##__VA_ARGS__)
//this will additionally return blank if N > __VA_ARGS__
#define TAKE_UPPER2(N,...) _TAKE_UPPER2(DROP_N((__VA_ARGS__),_TAKE_UPPERI N),N,__VA_ARGS__)
#define _TAKE_UPPER2(COND,N,...) APPLYIFNOT(_TAKE_UPPERI(COND),TAKE_UPPER,(N,__VA_ARGS__))IF(_TAKE_UPPERI(COND),_TAKE_UPPERI(__VA_ARGS__))
//like TAKE_UPPER but with N being an integer literal
#define TAKE_UPPERN(N,...) DROPN(ARGNUM(__VA_ARGS__),TAKEN(N,_ARGNUMLIST),##__VA_ARGS__)
#define TAKE_UPPER2N(N,...) DROPN(ARGNUM(__VA_ARGS__),TAKEN(N,TAKEN(ARGNUM(__VA_ARGS__),_ARGNUMLIST)),##__VA_ARGS__)

//rotate the argument list N times to the right (N is a unary number)
#define RROTATE(N,...) DROP_N(SUB((__VA_ARGS__),N),__VA_ARGS__),DROP_UPPER(N,__VA_ARGS__)

/* converts preprocessor unary numbers into decimal integer literals.
 * Everything over LOOPMAX is saturated to LOOPMAX itself. */
#define TO_DECIMAL(UNARY) ARGNUM(SATURATE UNARY)
//unary arithmetic done by preprocessor, unary numbers are not limited themselves by LOOPLIMIT, only 0
#define ADD(N,M) (_ADDDROP(_ADDVARGS N _ADDVARGS M))
#define _ADDVARGS(...) ,##__VA_ARGS__
#define _ADDDROP(...) _DROP0(__VA_ARGS__)
#define ADD_ONE(N) (1 _ADDVARGS N)	//adds 1 to the unary number
#define SUB(N,M) (DROP_N(M, _SUBI N))
#define _SUBI(...) __VA_ARGS__
#define SUB_ONE(N) (_DROP0 N)	//subtracts 1 from the unary number
//reverse subtract
#define RSUB(M,N) SUB(N,M)

#define MUL(N,M) (APPLYIF(_MULI M,REPEAT_N,(N,_MULI M)))
#define _MULI(...) __VA_ARGS__
/* DIV(int divident,divisor)
 * int q=0;
 * while (1) {
 *   if (divident >= divisor) {
 *     q++;
 *     divident -= divisor;
 *   }
 *   else return q;
 * }
 * */
/** A division of unary numbers. Using unary numbers it barely gets any more efficient than this. */
#define DIV(N,M) _DIV(N,M)
#define _DIV(N,M) _DIVDROP(WHILE(P_NOBLANK,F_DIVIDE,PP0,M,_DIVI N))
#define _DIVDROP(...) _DROP0(__VA_ARGS__)
#define _DIVI(...) __VA_ARGS__
#define F_DIVIDE(Q,M,...)	_DIV0(DROP_N((__VA_ARGS__),_DIVI M),Q,M,__VA_ARGS__)
#define _DIV0(COND,Q,M,...)	APPLYIFNOT(_DIVI(COND),_DIV1,(Q,M,__VA_ARGS__))IF(_DIVI(COND),I(,Q))
#define _DIV1(Q,M,...)		ADD_ONE(Q),M,DROP_N(M,__VA_ARGS__)
//debugging example, prints all loop cycles with index:
//#define DIV_DBG(N,M) ITER_DBG(F_DIVIDE,PP0,M,_DIVI N)
#define RDIV(M,N) _DIV(N,M)

//would have been easier to only use commas but that would require changing and retesting the code
#define PP0 ()
#define PP1 (1)
#define PP2 (1,1)
#define PP3 (1,1,1)
#define PP4 (1,1,1,1)
#define PP5 (1,1,1,1,1)
#define PP6 (1,1,1,1,1,1)
#define PP7 (1,1,1,1,1,1,1)
#define PP8 (1,1,1,1,1,1,1,1)
#define PP9 (1,1,1,1,1,1,1,1,1)
#define PP10 (1,1,1,1,1,1,1,1,1,1)
#define PP11 ADD(PP10,PP1)
#define PP12 ADD(PP10,PP2)
#define PP13 ADD(PP10,PP3)
#define PP14 ADD(PP10,PP4)
#define PP15 ADD(PP10,PP5)

/* compare decimals */
//this uses unary numbers instead of decimal literals
#define CMP_N(N,M) (NOT(_CMP_NI SUB(M,N)),NOT(_CMP_NI SUB(N,M)))
#define _CMP_NI(...) __VA_ARGS__
#define EQ_N(N,M) _CMP_NI(AND CMP_N(N,M))
#define NE_N(N,M) _CMP_NI(_NE_N CMP_N(N,M))
#define _NE_N(GE, LE) OR(_NOT(GE),_NOT(LE))
#define LT_N(N,M) _CMP_NI(_LT_N CMP_N(N,M))
#define _LT_N(GE, LE) _NOT(OR(GE,_NOT(LE)))
#define GT_N(N,M) _CMP_NI(_GT_N CMP_N(N,M))
#define _GT_N(GE,LE) _NOT(OR(_NOT(GE),LE))
#define LE_N(N,M) _LE_NDROP(CMP_N(N,M))
#define _LE_NDROP(CC) _DROP0 CC
#define GE_N(N,M) _GE_NTAKE(CMP_N(N,M))
#define _GE_NTAKE(CC) _TAKE0 CC
/** returns 2-tuple, left side is true if N => M, right side is true if N <= M. Parentheses and commas
	are illegal. Everything other than that, which is not decimal literal <= LIMIT, will be treated as 0 */
#define DEC_CMP(N,M) CMP_N(TO_UNARY(N),TO_UNARY(M))
#define _DEC_CMPI(...) __VA_ARGS__
#define DEC_EQ(N,M) _DEC_CMPI(AND DEC_CMP(N,M))
#define DEC_NE(N,M) _DEC_CMPI(_NE_N DEC_CMP(N,M))
#define DEC_LT(N,M) _DEC_CMPI(_LT_N DEC_CMP(N,M))
#define DEC_GT(N,M) _DEC_CMPI(_GT_N DEC_CMP(N,M))
#define DEC_LE(N,M) _LE_NDROP(DEC_CMP(N,M))
#define DEC_GE(N,M) _GE_NTAKE(DEC_CMP(N,M))

#define _DEC_ADDI(...) __VA_ARGS__
#define DEC_ADD(N,M) TO_DECIMAL(ADD(TO_UNARY(N),TO_UNARY(M)))
#define DEC_SUB(N,M) COUNT_N(SUB(TO_UNARY(N),TO_UNARY(M)))
/* Implements a full adder which could allow for decimal calculations with up to
 * 30 comma-separated digits (128 bit). Whitespace separated digits would be possible
 * when commas are inserted by concatenation to a symbol prefix within ITERATE. */
#define DEC_FULLADD(D1,D2,...) _DEC_ADDI(_DEC_FULLADD ADD(ADD(TO_UNARY(D1),TO_UNARY(D2)),(__VA_ARGS__)))
#define _DEC_FULLADD(...) _DEC_FULLADD0((DROPN(9,__VA_ARGS__)),(__VA_ARGS__))
#define _DEC_FULLADD0(C,SUM)  IF_ELSE(BOOL C,_DEC_CMPI(1,ARGGNUM(_DEC_FULLADDDROP C)),_DEC_CMPI(0,ARGNUM SUM))
#define _DEC_FULLADDDROP(...) _DROP0(__VA_ARGS__)
#define DEC_FULLSUB(D1,D2,...) _DEC_ADDI(_DEC_FULLSUB(DROPN(TRY(__VA_ARGS__,0),_DEC_ADDI SUB(ADD(PP10,TO_UNARY(D1)),TO_UNARY(D2)))) )
#define _DEC_FULLSUB(...)  _DEC_FULLSUB0((DROPN(9,__VA_ARGS__)),(__VA_ARGS__))
#define _DEC_FULLSUB0(C,DIFF)  IF_ELSE(BOOL C,_DEC_CMPI(0,ARGNUM(_DEC_FULLADDDROP C)),_DEC_CMPI(1,ARGNUM DIFF))
//the borrow bit is always the inverted carry bit
#define DEC_MUL(N,M) TO_DECIMAL(MUL(TO_UNARY(N),TO_UNARY(M)))
#define DEC_DIV(N,M) COUNT_N(DIV(TO_UNARY(N),TO_UNARY(M)))
//dividing by zero is an endless loop and expands to blank!


//expands to the count of arguments represented as unary number
#define UNARY_ARGC(...) (_UNARY_ARGCDROP(_REPEATN(ARGNUM(__VA_ARGS__),F_COMMA_ONE,_REDUCI0,,)))
#define F_COMMA_ONE(...) , 1
#define _UNARY_ARGCDROP(...) _DROP0(__VA_ARGS__)

/** removes all unparenthesized commas. This can be very useful if you generate preprocessor code
 * in a loop and separate macros from parentheses with a comma in between to prevent early expansion.
 * This macro then can use the generated code as argument to evaluate it. */
#define UNIFY(...) _UNIFYI(_REPEATN(ARGNUM(__VA_ARGS__),F_ID,_REPEATI0,_REPEATI1,__VA_ARGS__))
#define _UNIFYI(...) __VA_ARGS__

/** Applies FUNC to each pair of the Cartesian product of LIST (in parentheses) and __VA_ARGS__.
 * This is done by iterating over LIST for each element of __VA_ARGS__.
 * The maximum number of elements, that can be generated this way is LIMIT*LIMIT
 * where LIMIT is the maximum number of MAP cycles. If there would be more elements, they are cutoff.
 * @param[in] CHGARGS is a macro name which changes (FUNC,CHG,LIST) as single argument and expands to next tuple.
 * 				CHGARGS is applied for each element of __VA_ARGS__.
 * @param[in] FUNC is a macro, expanded for each pair, LIST element is 1st and __VA_ARGS__ element as 2nd arg.
 * @param[in] CHG is a macro which changes the current element of __VA_ARGS__ while mapped to LIST.
 * @param[in] LIST - elements of left list
 * @param[in] ... - elements of right list. KEEP IN MIND!: if the first element is blank, it is ignored.
 * 					This rule is due to CARTPOW to prevent mapping an additional blank element in a
 * 					subsequent CARTESIAN call which is generated by the previous CARTESIAN call. */
#define CARTESIAN(CHGARGS,FUNC,CHG,LIST,...) _CARTESIAN(TRY(CHGARGS,_MAPI0),(FUNC,TRY(CHG,_MAPI1),LIST),__VA_ARGS__)
#define _CARTESIAN(CHGARGS,ARGS,A,...) _CARTESIAN0(ARGNUM(OPT(A)__VA_ARGS__),CHGARGS,ARGS,OPT(A)__VA_ARGS__)
#define _CARTESIAN0(COUNT,CHGARGS,ARGS,...) _CARTESIANI(_MAPN(COUNT,F_CARTES,CHGARGS,CHGARGS(ARGS),__VA_ARGS__))
/* Even though called later, any nested macro name here is flagged as illegal for any later macro expansion.
 * This is the reason why we need indirection. */
#define F_CARTES(A,ARGS) _CARTES0 _LPAR _TAKE0 ARGS,A,_DROP0 ARGS _RPAR
#define _CARTESIANI(...) __VA_ARGS__
#define _CARTESIANI0(...) __VA_ARGS__
#define _CARTES0(FUNC,A,CHG,LIST) _CARTES1(FUNC,CHG,A,_CARTESIANI0 LIST)
//every argument from CARTESIAN's __VA_ARGS__ is replaced with a loop where this __VA_ARGS__ is LIST
#define _CARTES1(FUNC,CHG,A,...) _MAP(FUNC,CHG,A,__VA_ARGS__)

/** Cartesian Map. Replaces each pair with their expansion result in FUNC. */
#define CARTMAP(FUNC,LIST,...) _CARTMAP(FUNC,LIST,__VA_ARGS__)
#define _CARTMAP(FUNC,LIST,...) _CARTMAPI(_MAP(F_CARTMAP,_MAPI0,(FUNC,LIST), __VA_ARGS__))
#define _CARTMAPI(...) __VA_ARGS__
#define _CARTMAPI0(...) __VA_ARGS__
#define F_CARTMAP(A,ARGS) _CARTMAP0 _LPAR _TAKE0 ARGS,A,_DROP0 ARGS _RPAR
#define _CARTMAP0(FUNC,SYM,LIST) _CARTMAP1(FUNC,SYM,_CARTMAPI0 LIST)
#define _CARTMAP1(FUNC,SYM,LIST) _MAP(FUNC,_MAPI0,SYM,LIST)

/** Cartesian Concatenation. Generates a list of all concatenated Cartesian product pairs. */
#define CARTCAT(LIST,...) _CARTCATDROP(_CARTMAP(F_APPEND,LIST,__VA_ARGS__))
#define _CARTCATDROP(...) _DROP0(__VA_ARGS__)
/** Reverse Cartesian Concatenation. Differs only be swapping the order of LIST and __VA_ARGS__.
 * This macro exists because it is recommended to put the smaller list as __VA_ARGS__. */
#define RCARTCAT(LIST,...) _CARTCATDROP(_CARTMAP(F_PREPEND,LIST,__VA_ARGS__))

/* Numbers */
/** concatenates each number on the left to each number on the right. Leading zeroes are not concatenated. */
#define CONCNUMS(LIST,...) _CONCNUMS(LIST,__VA_ARGS__)
#define _CONCNUMS(LIST,...) _CARTCATDROP(_MAP(F_CONCNUM,_MAPI0,LIST, __VA_ARGS__))
#define F_CONCNUM(A,LIST) APPLYIF(A,_CONCNUM,(A,LIST))
#define _CONCNUM(A,LIST) , _CONCNUM0 _LPAR PPBOOL(A),LIST _RPAR
#define _CONCNUM0(SYM,LIST) _CONCNUM1(SYM, _TAKE0 LIST, _DROP0 LIST)
#define _CONCNUM1(SYM,A,LIST) CONC(SYM,A) _MAP(F_PREPEND,_MAPI0,SYM,LIST)
#define F_PREPENDNUM(A,...) IF(A,I(, CONC(PPBOOL(__VA_ARGS__),A)))

/* This can only can generate up to LOOPMAX ^ 2 elements (potentially LOOPMAX^LOOPMAX eqv. to ca. 180 bit
 * as a number with LOOPMAX = 30) but they are cut off in between with each CARTESIAN call.
 * The result will show only the first LOOPMAX^2 elements at most. A gigantic number of elements wouldn't
 * fit in your memory anyways. It basically executes for each argument in __VA_ARGS__ a new nesting of
 * CARTESIAN. Blank arguments for GEN, CHGARGS and CHG are replaced with _REPEATI0.
 * @param[in] GEN - macro which generates the argument tuple without variadic arguments for each nesting
 * 					of CARTESIAN using the current element from __VA_ARGS__ and then the tuple of
 * 					non-variadic arguments from the last CARTESIAN nesting as macro arguments.
 * @param[in] CHGARGS - initial CHARGS argument for outermost CARTESIAN call
 * @param[in] FUNC - initial FUNC argument
 * @param[in] CHG - initial CHG argument
 * @param[in] LIST - initial (parenthesized) LIST argument
 * @param[in] STARTLIST - parenthesized list which is used as __VA_ARGS__ for the deepest nested call.
 * @param[in] ... - elements which should generate a CARTESIAN call per element */
#define CARTPOW(GEN,CHGARGS,FUNC,CHG,LIST,STARTLIST,...)\
	_CARTPOW(ARGNUM(__VA_ARGS__),TRY(GEN,_REPEATI0),(TRY(CHGARGS,_REPEATI0),FUNC,TRY(CHG,_REPEATI1),LIST),_REPEATI0 STARTLIST,__VA_ARGS__)
#define _CARTPOW(COUNT,GEN,ARGS,START,...) _CARTPOWI(_REPEATN(COUNT,F_CARTPOW,GEN,ARGS,__VA_ARGS__) START _REPEATN(COUNT,F_RPAR,_REPEATI0,,))
#define F_CARTPOW(_,ARGS) CARTESIAN _LPAR _CARTPOWI ARGS ,
#define _CARTPOWI(...) __VA_ARGS__

/** enumerates all binary literals from 2^N-1 down to 0. N must be a decimal literal for safety reasons. */
#define ENUMBIN(N) _ENUMBIN(_ENUMBINI SUB_ONE(TO_UNARY(N)))
#define _ENUMBIN(N) _ENUMBINDROP(CARTPOW(_ENUMBINI,,F_PREPENDNUM,,(BINARY),(BINARY),N))
#define _ENUMBINDROP(...) RCARTCAT((_DROP0(__VA_ARGS__)),0b)
#define _ENUMBINI(...) __VA_ARGS__

#define BIN(D) IFNOT(_BIN BIN_##D,BIN_##D)
#define _BIN(...)
#define BIN_0 PP0
#define BIN_1 PP1

#define DEC(D) IFNOT(_DEC DEC_##D,DEC_##D)BIN(D)
#define _DEC(...)
#define DEC_2 PP2
#define DEC_3 PP3
#define DEC_4 PP4
#define DEC_5 PP5
#define DEC_6 PP6
#define DEC_7 PP7
#define DEC_8 PP8
#define DEC_9 PP9

#define HEX(D) IFNOT(_HEX HEX_##D,HEX_##D)DEC(D)
#define _HEX(...)
#define HEX_A PP10
#define HEX_B PP11
#define HEX_C PP12
#define HEX_D PP13
#define HEX_E PP14
#define HEX_F PP15

#define F_ISBIN(A,...) NOT(_BIN BIN(A))
#define F_ISDEC(A,...) NOT(_DEC DEC(A))
#define F_ISHEX(A,...) NOT(_HEX HEX(A))

//lets you create cool underscored names!
#define UNDERSCORE(...) CONCAT(F_UNDERSCORE, __VA_ARGS__)
#define F_UNDERSCORE(A,...) _ ## A ## _
#define F_DECORATE(A,...) .~*+A+*~

//test namespace recognition
#define _NAME_hello NAME_NOLAST
#define _NAME_world
#define _NAME_day
#define _NAME_my NAME_NOLAST
#define _NAME_death NAME_FORBIDDEN
#define _TEST_failure
#define _TEST_error NAME_NOLAST
#define _TEST_bug NAME_ALLOWED
#define _TEST_
#define _NAMEA_herbert
#define _NAMEA_dieter
#define _NAMEB_achim
#define _NAMEC_jojo
#if 0
int x = COUNT_N(REDUCE2(ADD,PP0,PP1,PP2,PP3,PP4,PP5)),		//very cheap calculation,
y = REDUCE(DEC_ADD,0,1,2,3,4,5),	//expensive calculation
//REDUCE2_DBG(DEC_MUL,1,2,3,4);    //this does not work because _REPEATN is nested, it will return 0
z = REDUCE(DEC_MUL,1,2,3,4),
aa;
char *halloWelt;
OPTIMIZE(2)

IGNORE_WARNING(unused-parameter)
static inline int test(int arg) {
	if(IS_NAMESPACE(_NAME, hello my day,/*,*/ hello /*babe*/world, day, day, my world) &&
		NOT_NAMESPACE(_TEST, hello, world, i, fixed, a, bug, errors are, human; failure) &&
		IS_NAMESPACES((_NAMEA,_NAMEC,_NAMEB,_NAMEA),dieter,jojo,achim,herbert) &&
		NOT_NAMESPACES((_NAMEA,_NAMEC,_NAMEB,_NAMEA),achim,dieter,jojo,jürgen,björn)
	)
		x = IS_TYPES(arg,2,-1,int,float,double);

	int ints[] = {
		DROP(ZIP(F_CONC,(RANGE(0,9)),RANGEDOWN(10,3))),
		LROTATE(TO_UNARY(5),1,2,3,4,5,6,7,8),
		RROTATE(UNARY_ARGC(,2,3,,4),1,2,3,4,5,6,7,8),
		REVERSE(0x5,0x6,0x7,0x8,0x9,0xA),
		WHILE(I,I,a,b,c)    //condition is always true, thus expansion is blank
		//this works but too high LOOPLIMIT leads to multiple seconds of Atollic IDE startup lag
		FLATTEN((),(2),(3,4),((1,2,(3),4),5)),	//this example takes about 15 loop cycles and exits in 16th. LOOPLIMIT must be >= 16
		ENUMBIN(4)
	};

	int cart[] = {100 CARTESIAN(,F_PREPENDNUM,,(DECIMAL),DECIMAL)};
	int d1000[] = {CONCNUMS((DECIMAL),CONCNUMS((DECIMAL),DECIMAL))};	//this generates comma-separated 999 down to 1000 - LIMIT*10
	int h256[] = {CONCNUMS((HEXADEC),CONCNUMS((HEXADEC),0x))};	//generates comma-separated 0xFF down to 0x0

	int dec[] = {FILTER(F_ISDEC,HEXADEC)};
	int hex[] = {RCARTCAT((FILTER_OUT(F_ISDEC,HEXADEC)),0x)};	//try to put smaller list on the right, always

	int DEFINE(0, a, b, c, d, e, f);
	ASSIGN(DROPN(2, b, c, d, e), RMAP(F_DIV,20,57,111,60,12,2));
	ASSIGN(TAKEN(DEC_DIV(17,5), a, b, c, d, e, f), LMAP(F_MUL, 4, 2, e));
//	DIV_DBG(TO_UNARY(17),PP5);		//hihi, I forgot to use _DIVI for the multi-element COND argument
	if (ISSORTED(<,SLICE(2,7,30)))
		x = a;
	else
		x = b;
	y = e;
	z = c;
	aa = TREELOOPX(F_B_OR,,, a, b, c, d, e, f);
	halloWelt = STRING(UNDERSCORE(h, a, l, l, o, , W, e, l, t)) //whitespace, commas and parentheses won't work here
					STRCAT(F_UNDERSCORE,COULD,ONLY,CONCATENATE,LETTERS)
					STRMAP(F_DECORATE,this,does,not!,concatenate,words,together!);
	return ARGNUM(EXTRACT(PP2,(SLICEDOWN(23,5,11)), a, b, c, d, e, f));
}
END_IGNORE
END_OPTIMIZE
#endif

#endif 
