# ExprApp

This is a sample implementation on the following grammar:

    expr ::= term + expr | term - expr | term 
    term :=  factor * term | factor / term | factor 
    factor := ( expr ) | float

https://cs.stackexchange.com/questions/23738/grammar-for-parsing-simple-mathematical-expression
