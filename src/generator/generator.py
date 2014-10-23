#
# Copyright (C) 2013 - present by OpenGamma Inc. and the OpenGamma group of companies
#
# Please see distribution for license.
#

"""Generates the implementation of the dispatcher and runners."""

from dispatch import Dispatch
from runners import Runners, Runners, InfixOpRunner, PrefixOpRunner, UnaryFunctionRunner, \
                    UnimplementedUnary, UnimplementedBinary, UnaryExpressionRunner, \
                    SelectResultRunner, BinaryExpression, BinaryExpressionRunner
from exprtree import Terminal
from expression import Expressions, Numeric
from enums import ExprEnums
from createexpressions import CreateExpressions

# The list of nodes to generate FULLY.
# These must be in the same order as in the Java ExprTypeEnum, otherwise the
# enum values won't match up.
nodes = [ UnimplementedUnary ('ABS'),
          UnaryFunctionRunner('ACOS', 'ACOS_ENUM', 'acos'),
          UnimplementedUnary ('ACOSH'),
          UnimplementedUnary ('ANGLE'),
          UnimplementedUnary ('ASIN'),
          UnaryFunctionRunner('ASINH', 'ASINH_ENUM', 'asinh'),
          UnaryFunctionRunner('ATAN', 'ATAN_ENUM', 'atan'),
          UnimplementedUnary ('ATANH'),
          UnimplementedUnary ('CONJ'),
          UnaryFunctionRunner('COS', 'COS_ENUM', 'cos'),
          UnimplementedUnary ('COSH'),
          UnimplementedUnary ('DIAG'),
          UnimplementedBinary('DOT'),
          UnimplementedUnary ('EIG'),
          UnimplementedUnary ('ERF'),
          UnimplementedUnary ('ERFC'),
          UnaryFunctionRunner('EXP', 'EXP_ENUM', 'exp'),
          UnimplementedUnary ('EYE'),
          UnimplementedUnary ('FLIPLR'),
          UnimplementedUnary ('FLIPUD'),
          UnimplementedUnary ('HILB'),
          UnimplementedBinary('HORZCAT'),
          UnimplementedUnary ('IMAG'),
          UnimplementedUnary ('INVHILB'),
          UnimplementedUnary ('LOG'),
          InfixOpRunner      ('MINUS', 'MINUS_ENUM', '-', 'sub','subx','xsub'),
          PrefixOpRunner     ('NEGATE', 'NEGATE_ENUM', '-'),
          UnimplementedUnary ('NORMCDF'),
          UnimplementedBinary('POWER'),
          InfixOpRunner      ('PLUS', 'PLUS_ENUM', '+', 'add','addx','addx'),
          InfixOpRunner      ('RDIVIDE', 'RDIVIDE_ENUM', '/', 'div','divx','xdiv'),
          UnimplementedUnary ('REAL'),
          UnimplementedUnary ('ROUND'),
          UnaryFunctionRunner('SIN', 'SIN_ENUM', 'sin'),
          UnaryFunctionRunner('SINH', 'SINH_ENUM', 'sinh'),
          UnimplementedUnary ('SQRT'),
          UnaryFunctionRunner('TAN', 'TAN_ENUM', 'tan'),
          UnaryFunctionRunner('TANH', 'TANH_ENUM', 'tanh'),
          InfixOpRunner      ('TIMES', 'TIMES_ENUM', '*', 'mul','mulx','mulx'),
          UnimplementedBinary('VERTCAT'),
          UnimplementedUnary ('WILKINSON')
        ]

# The list of nodes to generate headers and wiring for, but not the implementations
custom_nodes = [
                UnaryExpressionRunner('NORM2', 'NORM2_ENUM'),
                UnaryExpressionRunner('PINV', 'PINV_ENUM'),
                UnaryExpressionRunner('INV', 'INV_ENUM'),
                UnaryExpressionRunner('SVD', 'SVD_ENUM'),
                SelectResultRunner('SELECTRESULT', 'SELECTRESULT_ENUM'),
                BinaryExpressionRunner('MTIMES','MTIMES_ENUM'),
                UnaryExpressionRunner('TRANSPOSE','TRANSPOSE_ENUM'),
                UnaryExpressionRunner('CTRANSPOSE','CTRANSPOSE_ENUM'),
                UnaryExpressionRunner('LU','LU_ENUM'),
                BinaryExpressionRunner('MLDIVIDE','MLDIVIDE_ENUM'),
                UnaryExpressionRunner('QR','QR_ENUM'),
                UnaryExpressionRunner('SUMCOLS','SUMCOLS_ENUM'),
                UnaryExpressionRunner('SUMROWS','SUMROWS_ENUM'),
               ]

# The list of terminals
terminals = [ Terminal('Real', 'Scalar'),
              Terminal('Complex', 'Scalar'),
              Terminal('Integer', 'Scalar'),
              Terminal('Real', 'DenseMatrix'),
              Terminal('Logical', 'Matrix'),
              Terminal('Complex', 'DenseMatrix'),
              Terminal('Real', 'DiagonalMatrix'),
              Terminal('Complex', 'DiagonalMatrix'),
              Terminal('Real', 'SparseMatrix'),
              Terminal('Complex', 'SparseMatrix') ]

def get_parser():
    """Creates a suitable parser for the options to the generator."""
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-o', '--output', help='Output file name', required=True)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument('--runners-hh', action='store_true', help='Generate runners.hh')
    action.add_argument('--runners-cc', action='store_true', help='Generate runners.cc')
    action.add_argument('--dispatch-hh', action='store_true', help='Generate dispatch.hh')
    action.add_argument('--dispatch-cc', action='store_true', help='Generate dispatch.cc')
    action.add_argument('--expression-hh', action='store_true', help='Generate expression.hh')
    action.add_argument('--expression-cc', action='store_true', help='Generate expression.cc')
    action.add_argument('--numeric-hh', action='store_true', help='Generate numeric.hh')
    action.add_argument('--numeric-cc', action='store_true', help='Generate numeric.cc')
    action.add_argument('--exprenum-hh', action='store_true', help='Generate exprenum.hh')
    action.add_argument('--exprenum-java', action='store_true', help='Generate ExprTypeEnum.java')
    action.add_argument('--createexpr-cc', action='store_true', help='Generate createexpr.cc')
    return parser

def main(args):
    """Entry point if run on the command line"""
    with open(args.output, 'w') as f:
        if args.runners_hh:
            code = Runners(nodes + custom_nodes).header
        elif args.runners_cc:
            code = Runners(nodes).source
        elif args.dispatch_hh:
            code = Dispatch(terminals, nodes + custom_nodes).header
        elif args.dispatch_cc:
            code = Dispatch(terminals, nodes + custom_nodes).source
        elif args.expression_hh:
            code = Expressions(nodes).header
        elif args.expression_cc:
            code = Expressions(nodes).source
        elif args.numeric_hh:
            code = Numeric(nodes).header
        elif args.numeric_cc:
            code = Numeric(nodes).source
        elif args.exprenum_hh:
            code = ExprEnums(nodes).code
        elif args.exprenum_java:
            code = ExprEnums(nodes).java
        elif args.createexpr_cc:
            code = CreateExpressions(terminals + nodes + custom_nodes).source
        f.writelines(code)

if __name__ == '__main__':
    main(get_parser().parse_args())
