//
// Created by dylan on 29/03/2017.
//

#include "../../../../include/Parser/AST/ASTStatements/ASTIfStatement.h"

parser::ast::ASTIfStatement::ASTIfStatement(parser::ast::ASTExprNode *exprNode, parser::ast::ASTBlock *astBlockForIF)
        : exprNode(exprNode), astBlockForIF(astBlockForIF) {}

parser::ast::ASTIfStatement::ASTIfStatement(parser::ast::ASTExprNode *exprNode, parser::ast::ASTBlock *astBlockForIF,
                                            parser::ast::ASTBlock *astBlockForElse) : exprNode(exprNode),
                                                                                      astBlockForIF(astBlockForIF),
                                                                                      astBlockForElse(
                                                                                              astBlockForElse) {}

void parser::ast::ASTIfStatement::accept(parser::ast::Visitor *v) {
    v->visit(this);
}

parser::ast::ASTIfStatement::~ASTIfStatement() {
    delete exprNode;
    delete astBlockForIF;
    if (astBlockForElse != nullptr) {
        delete astBlockForElse;
    }
}
