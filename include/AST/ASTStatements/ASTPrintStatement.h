//
// Created by dylan on 29/03/2017.
//

#ifndef COMPILERTHEORY_ASTPRINTSTATEMENT_H
#define COMPILERTHEORY_ASTPRINTSTATEMENT_H

#include "ASTStatementNode.h"
#include "../ASTExpression/ASTExprNode.h"

namespace ast {

    class ASTPrintStatement : public ASTStatementNode {
    public:
        ASTExprNode * exprNode;

        ASTPrintStatement(ASTExprNode *exprNode);

        void accept(visitor::Visitor *v) override;

        ~ASTPrintStatement();
    };

}


#endif //COMPILERTHEORY_ASTPRINTSTATEMENT_H
