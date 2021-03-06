#include <string>
#include "moveUtils.h"

namespace AdiChess {
    
    enum Side {
        W=0, B=1, NUM_SIDES=2, NONE=3
    };

    struct Piece {
    
        enum Type {
            K, Q, B, R, N, P, NONE, NUM_PIECES=6
        };

        Type type;
        Side side;
        
        Piece(Type const &type_, Side const &side_): type(type_), side(side_) {}
        
        friend std::ostream &operator<<(std::ostream &os, Piece const &piece) {
            switch (piece.type) {
                case Type::NONE:
                    os << std::string(" ");
                    break;
                case Type::R:
                    os << std::string(piece.side ? "r" : "R");
                    break;      
                case Type::K:
                    os << std::string(piece.side ? "k" : "K");
                    break;
                case Type::Q:
                    os << std::string(piece.side ? "q" : "Q");
                    break;
                case Type::B:
                    os << std::string(piece.side ? "b" : "B");
                    break;
                case Type::N:
                    os << std::string(piece.side ? "n" : "N");
                    break;  
                case Type::P:
                    os << std::string(piece.side ? "p" : "P");
                    break; 
                default:
                    os << std::string(" ");       
            }
            return os;
        }

    };

};