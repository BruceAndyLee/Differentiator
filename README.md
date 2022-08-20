# Differentiator

Builds a tree after reading an infix expression (inf_diff.cpp) and takes a derivative of the expression.

Issues: 
- Memory leaking like crazy
- parentheses appear when evaluating derivatives of exponential functions
- post-derivetive calculations fail to simplify all arithmetics (raising an expression to the power of 1)
