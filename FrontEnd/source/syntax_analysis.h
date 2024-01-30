#ifndef SYNTAX_ANALYSIS_H_INCLUDED
#define SYNTAX_ANALYSIS_H_INCLUDED

#include "lexical_analysis.h"

/*
/============================= Grammar rules ===========================\
|   G       ::= F+                                                      |
|   F       ::= 'layax' Id Args B ';'                                   |
|   Args    ::= '(' E {',' E}* ')'                                      |
|   B       ::= '{' Op+ '}'                                             |
|   Op      ::= Call | Ret | If | A | Wh ';'                            |
|   Call    ::= "kil kunta" Id Args                                     |
|   Ret     ::= "par" Expr                                              |
|   If      ::= 'enchen te' '(' Cond ')' B {'uraxla' B}*                |
|   A       ::= Id '=' Exp ';'                                          |
|   Wh      ::= 'xallexe' '(' Cond ')' B                                |
|   Cmp     ::= Expr ['>', '<', '>=', '<=', '==', '!='] Expr            |
|   Cond    ::= Cmp {['&&', '||'] Cmp}                                  |
|   Expr    ::= ...                                                     | 
\=======================================================================/
*/

/*====================================STRUCTS_&_CONSTANTS====================================*/



/*========================================FUNCTIONS==========================================*/

FrontErrors CreateTreeFromTokenList(Tree* const tree, List* const token_list, NameSpace* const name_space);

/*===========================================================================================*/

#endif  // SYNTAX_ANALYSIS_H_INCLUDED