#!/bin/bash

for SYMBOL in 'cos' 'sin' 'tan' 'acos' 'asin' 'atan' 'atan2' 'cosh' 'sinh' 'tanh' 'acosh' 'asinh' 'atanh' 'exp' 'frexp' 'ldexp' 'log' 'log10' 'modf' 'exp2' 'expm1' 'ilogb' 'log1p' 'log2' 'logb' 'scalbn' 'scalbln' 'pow' 'sqrt' 'cbrt' 'hypot' 'erf' 'erfc' 'tgamma' 'lgamma' 'ceil' 'floor' 'fmod' 'trunc' 'round' 'lround' 'llround' 'rint' 'lrint' 'llrint' 'nearbyin' 'remainder' 'remquo' 'copysign' 'nextafter' 'nexttoward' 'fdim' 'fmax' 'fmin' 'fabs' 'abs' 'fma' 'fpclassify' 'isinf' 'isnan' 'isnormal' 'signbit'; do
    # Check symbol table
    grep -rnI "[^d]::${SYMBOL}(" * | grep -v third_party | grep -v "://"
    grep -rnI "[^:a-z_\.]${SYMBOL}(" * | grep -v third_party | grep -v "://"
done
exit 0
