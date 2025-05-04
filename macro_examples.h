#include "macro_api.h"

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