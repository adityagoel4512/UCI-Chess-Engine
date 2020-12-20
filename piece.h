#include <string>

namespace AdiChess {
    
    enum Side {
        W, B, NONE, NUM_SIDES=2
    };

    struct Piece {
    
        enum class Type {
            R, H, B, Q, K, P, NONE, NUM_PIECES=6
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
                case Type::H:
                    os << std::string(piece.side ? "k" : "K");
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