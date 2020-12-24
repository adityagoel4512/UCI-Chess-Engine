#include "board.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace AdiChess {

    Board::Board(std::string const &fenString) {
        parseFenString(fenString);
        MoveGeneration::init();
    }

    void Board::makeMove(Move &move) {
        movePiece(move.getFrom(), move.getTo());
        Piece piece = (*this)(move.getFrom());
        ++halfMoveClock;
        if (piece.side == Side::B) {
            ++fullMoveNumber;
        }

        std::swap(player, opponent);
    }

    Piece Board::operator()(int position) const {
        for (int i = 0; i < static_cast<int>(Piece::Type::NUM_PIECES); ++i) {
            if (Utility::checkBit(bitboards[i][0], position))
                return {static_cast<Piece::Type>(i), Side::W};
            else if (Utility::checkBit(bitboards[i][1], position))
                return {static_cast<Piece::Type>(i), Side::B};
        }
        return {Piece::Type::NONE, Side::NONE};
    }


    void Board::movePiece(uint64_t from, uint64_t to) {
        Piece piece = (*this)(from);
        clearPiece(from, piece);
        (*this)(to, piece);
    }
    
    void Board::operator()(int position, Piece const &piece) {
        Utility::setBit(bitboards[static_cast<int>(piece.type)][piece.side], position);
    }
    
    Piece Board::operator()(int i, int j) const {
        return (*this)(Utility::flattenCoordinates(i, j));
    } 

    void Board::operator()(int i, int j, Piece const &piece) {
        (*this)(Utility::flattenCoordinates(i, j), piece);
    }

    void Board::clearPiece(int position, Piece const &piece) {
        Utility::clearBit(bitboards[static_cast<int>(piece.type)][piece.side], position);
    }

    void Board::parseFenString(std::string const &fenString) {        
        std::string token;
        std::stringstream ss(fenString);

        // Parse board into bitboard        
        for (int i = 0; i < 8; ++i) {
            if (i == 8) {
                ss >> token;
            } else {
                std::getline(ss, token, '/');
            }
            int j = 0;
            while (j < 8) {
                if (token[j] <= '8' && token[j] >= '1') {
                    j+=token[j]-'0';
                } else {
                    int position = Utility::flattenCoordinates(i, j);
                    switch (token[j]) {
                        case 'K':
                            Utility::setBit(bitboards[0][0], position);
                            break;
                        case 'k':
                            Utility::setBit(bitboards[0][1], position);
                            break;
                        case 'Q':
                            Utility::setBit(bitboards[1][0], position);
                            break;
                        case 'q':
                            Utility::setBit(bitboards[1][1], position);
                            break;
                        case 'B':
                            Utility::setBit(bitboards[2][0], position);
                            break;
                        case 'b':
                            Utility::setBit(bitboards[2][1], position);
                            break;
                        case 'R':
                            Utility::setBit(bitboards[3][0], position);
                            break;
                        case 'r':
                            Utility::setBit(bitboards[3][1], position);
                            break;
                        case 'N':
                            Utility::setBit(bitboards[4][0], position);
                            break;
                        case 'n':
                            Utility::setBit(bitboards[4][1], position);
                            break;
                        case 'P':
                            Utility::setBit(bitboards[5][0], position);
                            break;
                        case 'p':
                            Utility::setBit(bitboards[5][1], position);
                            break;
                    }
                    ++j;
                }
            }
        }

        ss = std::stringstream(token);
        ss >> token >> token;

        // Side to play
        player = token == "w" ? Side::W : Side::B;
        opponent = token == "w" ? Side::B : Side::W;


        ss >> token;

        // Castling rights
        if (token != "-") {
            for (char c : token) {
                if (c == 'K') 
                    castlingRights[0][0] = 1;
                else if (c == 'k')
                    castlingRights[1][0] = 1;
                else if (c == 'Q') 
                    castlingRights[0][1] = 1;
                else if (c == 'q')
                    castlingRights[1][1] = 1;
            }
        }

        ss >> token;
        // En passant target square
        if (token != "-") {
            enPassantTarget = Utility::flattenCoordinates(token[0]-'a', token[1]-'0');
        }        

        ss >> token;
        halfMoveClock = std::stoi(token);
        ss >> token;
        fullMoveNumber = std::stoi(token);
    }   
}