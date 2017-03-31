//
// Created by dylan on 29/03/2017.
//

#ifndef COMPILERTHEORY_ASTASSIGNMENT_H
#define COMPILERTHEORY_ASTASSIGNMENT_H

#include "ASTStatementNode.h"
#include "../ASTExpression/ASTExprNode.h"

namespace parser {
    namespace ast {
        class ASTAssignment : public ASTStatementNode{
        public:
            std::string identifier;

            ASTExprNode * exprNode;

            ASTAssignment(const std::string &identifier, ASTExprNode *exprNode);

            void accept(Visitor *v) override;
        };
    }
}

#endif //COMPILERTHEORY_ASTASSIGNMENT_H
