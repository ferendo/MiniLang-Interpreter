//
// Created by dylan on 20/03/2017.
//

#include "Parser.h"
#include "../Exceptions/UnexpectedTokenWhileParsing.h"
#include "AST/ASTExpression/ASTUnary.h"

using namespace lexer;
using namespace std;
using namespace exceptions;

namespace parser {
    Parser::Parser(Lexer lexer) : lexer(lexer) {
        parse();
    }

    void Parser::parse() {
        ast::ASTNode * programNode = new ast::ASTNode();
        // Program can contain 0 or more Statements. If the first token is
        // TOK_EOF, it indicates that the program is empty.
        try {
            while (lexer.previewNextToken().tokenType != TOK_EOF) {
                programNode->addStatement(parseStatement());
            }
            ast::Visitor * visitor = new ast::XMLConverterVisitor();
            programNode->accept(visitor);
        } catch (UnexpectedTokenWhileParsing &error) {
            cout << error.reasonForError << endl;
        }
    }

    ast::ASTStatementNode * Parser::parseStatement() {

        currentToken = lexer.getNextToken();

        switch (currentToken.tokenType) {
            case TOK_Var:
                return parseVariableDeclarationStatement();
            case TOK_Set:
                return parseAssignmentStatement();
            case TOK_Print:
                return parsePrintStatement();
            case TOK_If:
                return parseIfStatement();
            case TOK_While:
                parseWhileStatement();
                break;
            case TOK_Return:
                parseReturnStatement();
                break;
            case TOK_Def:
                parseFunctionDeclarationStatement();
                break;
            case TOK_LeftCurlyBracket:
                return parseBlock();
            default:
                throw UnexpectedTokenWhileParsing("Unexpected token found while parsing. Expecting Statement Token.");
        }
        return nullptr;
    }

    ast::ASTVariableDeclaration * Parser::parseVariableDeclarationStatement() {
        std::string identifier;
        TOKEN tokenType;
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Identifier) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. "
                                                      "Expected name after var keyword.");
        }

        identifier = currentToken.tokenName;
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Colon) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. "
                                                      "Expected ':' after Identifier when variable declaration");
        }

        tokenType = parseType();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Equals) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected '=' after Variable Type.");
        }

        ast::ASTExprNode * exprNode = parseExpression();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_SemiColon) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected ';' after declaration.");
        }

        return new ast::ASTVariableDeclaration(identifier, tokenType, exprNode);
    }

    ast::ASTAssignment * Parser::parseAssignmentStatement() {
        std::string identifier;
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Identifier) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. "
                                                      "Expected name after set keyword.");
        }

        identifier = currentToken.tokenName;
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Equals) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected '=' after identifier.");
        }

        ast::ASTExprNode * exprNode = parseExpression();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_SemiColon) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected ';' after assignment.");
        }

        return new ast::ASTAssignment(identifier, exprNode);
    }

    ast::ASTPrintStatement * Parser::parsePrintStatement() {
        ast::ASTExprNode * exprNode = parseExpression();
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_SemiColon) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected ';' after assignment.");
        }

        return new ast::ASTPrintStatement(exprNode);
    }

    ast::ASTIfStatement * Parser::parseIfStatement() {
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_LeftParenthesis) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected '(' after If.");
        }

        ast::ASTExprNode * exprNode = parseExpression();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_RightParenthesis) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expect ')' after Expression.");
        }

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_LeftCurlyBracket) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected '{' after If declaration.");
        }

        ast::ASTBlock * ifBlock = parseBlock();

        if (lexer.previewNextToken().tokenType != TOK_Else) {
            return new ast::ASTIfStatement(exprNode, ifBlock);
        }

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Else) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected Else..");
        }
        // ParseBlock will parse the else block.
        return new ast::ASTIfStatement(exprNode, ifBlock, parseBlock());
    }

    void Parser::parseWhileStatement() {
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_LeftParenthesis) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        parseExpression();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_RightParenthesis) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        parseBlock();

        return;
    }

    void Parser::parseReturnStatement() {
        parseExpression();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_SemiColon) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        return;
    }

    void Parser::parseFunctionDeclarationStatement() {
        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Identifier) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_LeftParenthesis) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        parseFormalParam();

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_RightParenthesis) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_Colon) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
        }

        parseType();

        parseBlock();

        return;
    }

    ast::ASTBlock * Parser::parseBlock() {
        ast::ASTBlock * block = new ast::ASTBlock();

        // Statement inside a block contains 0 or more statements. If there are no
        // Statements, do not call parseStatements since it will cause trouble.
        while (lexer.previewNextToken().tokenType != TOK_RightCurlyBracket) {
            block->addStatement(parseStatement());
        }

        currentToken = lexer.getNextToken();

        if (currentToken.tokenType != TOK_RightCurlyBracket) {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected '}' after Block.");
        }
        return block;
    }

    ast::ASTExprNode * Parser::parseExpression() {
        ast::ASTExprNode * factor = parseFactor();

        Token nextToken = lexer.previewNextToken();

        if (nextToken.tokenType == TOK_MultiplicativeOperator ||
            (nextToken.tokenType == TOK_Logic && nextToken.tokenName == "and")) {
            currentToken = lexer.getNextToken();
        } else if (nextToken.tokenType == TOK_AdditiveOperator ||
            (nextToken.tokenType == TOK_Logic && nextToken.tokenName == "or")) {
            currentToken = lexer.getNextToken();
        } else if (nextToken.tokenType == TOK_RelationalOperator) {
            currentToken = lexer.getNextToken();
            parseExpression();
            return nullptr;
        } else if (nextToken.tokenType == TOK_Equals) {
            // For an expression, there need to be '=='.
            currentToken = lexer.getNextToken();
            currentToken = lexer.getNextToken();

            if (currentToken.tokenType != TOK_Equals) {
                throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected '=' after "
                                                          "'=' in an expression.");
            }
            parseExpression();
        } else {
            return factor;
        }
    }

    void Parser::parseFormalParam() {

        if (lexer.previewNextToken().tokenType == TOK_Identifier) {
            // TODO check current token??
            currentToken = lexer.getNextToken();
            currentToken = lexer.getNextToken();

            if (currentToken.tokenType != TOK_Colon) {
                throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing");
            }

            parseType();

            if (lexer.previewNextToken().tokenType == TOK_Comma) {
                currentToken = lexer.getNextToken();
                parseFormalParam();
            }
        }

        return;
    }

    ast::ASTExprNode * Parser::parseFactor() {

        currentToken = lexer.getNextToken();
        ast::ASTLiteralNode * literalNode = parseLiteral();

        if (literalNode != nullptr) {
            return literalNode;
        } else if (currentToken.tokenType == TOK_Identifier) {
            // This factor can either be Identifier or Function call if the next token is TOK_LeftParenthesis.
            ast::ASTFunctionCall * functionCall = parseFunctionCall();

            if (functionCall != nullptr) {
                return functionCall;
            } else {
                return new ast::ASTIdentifier(currentToken.tokenName);
            }
        } else if (currentToken.tokenType == TOK_LeftParenthesis) {
            ast::ASTExprNode * exprNode = parseExpression();
            currentToken = lexer.getNextToken();

            if (currentToken.tokenType != TOK_RightParenthesis) {
                throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expecting ')' after expression");
            }

            return exprNode;
        } else if ((currentToken.tokenType == TOK_AdditiveOperator && currentToken.tokenName == "-") ||
                   (currentToken.tokenType == TOK_Logic && currentToken.tokenName == "not")) {
            return new ast::ASTUnary(currentToken.tokenName, parseExpression());
        } else {
            throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected Expression.");
        }
    }

    ast::ASTLiteralNode * Parser::parseLiteral() {

        switch (currentToken.tokenType) {
            case TOK_BooleanLiteral:
                return new ast::ASTBooleanLiteral(currentToken.tokenName.find("true") != string::npos);
            case TOK_IntegerLiteral:
                return new ast::ASTIntegerLiteral((int) currentToken.tokenValue);
            case TOK_RealLiteral:
                return new ast::ASTRealLiteral(currentToken.tokenValue);
            case TOK_Printable:
                return new ast::ASTStringLiteral(currentToken.tokenName);
            default:
                return nullptr;
        }
    }

    ast::ASTFunctionCall * Parser::parseFunctionCall() {
        // The current token is the identifier.
        std::string identifier = currentToken.tokenName;
        ast::ASTFunctionCall * functionCall = nullptr;
        vector<ast::ASTExprNode *> arguments;

        if (lexer.previewNextToken().tokenType == TOK_LeftParenthesis) {
            currentToken = lexer.getNextToken();
            parseActualParams(arguments);
            currentToken = lexer.getNextToken();

            if (currentToken.tokenType != TOK_RightParenthesis) {
                throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. Expected ')' after parameters.");
            }
            functionCall = new ast::ASTFunctionCall(identifier, arguments);
        }
        return functionCall;
    }

    void Parser::parseActualParams(vector<ast::ASTExprNode *> &arguments) {
        // Function call can have no actualParams
        if (lexer.previewNextToken().tokenType != TOK_RightParenthesis) {

            arguments.push_back(parseExpression());

            if (lexer.previewNextToken().tokenType == TOK_Comma) {
                currentToken = lexer.getNextToken();

                if (lexer.previewNextToken().tokenType == TOK_RightParenthesis) {
                    throw UnexpectedTokenWhileParsing(
                            "Unexpected Token found while parsing. Expected Expression after ','.");
                }
                parseActualParams(arguments);
            }
        }
    }

    TOKEN Parser::parseType() {
        currentToken = lexer.getNextToken();

        switch (currentToken.tokenType) {
            case TOK_RealType:
            case TOK_IntType:
            case TOK_BoolType:
            case TOK_StringType:
                return currentToken.tokenType;
            default:
                throw UnexpectedTokenWhileParsing("Unexpected Token found while parsing. "
                                                          "Expected Variable Type after ':'.");
        }
    }

    ast::ASTBinaryExprNode *
    Parser::combineExpressions(ast::ASTBinaryExprNode *parent, ast::ASTBinaryExprNode *newNode) {
        return nullptr;
    }
}