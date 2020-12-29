#include "board.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include "moveGenerator.h"
namespace AdiChess {

    Board::Board(std::string const &fenString) {
        parseFenString(fenString);
        MoveGeneration::init();
    }

    void Board::makeMove(Move const &move) {
        
        // Clone irreversible state
        updateState();

        uint64_t source = move.getFrom();
        uint64_t target = move.getTo();
        auto flag = move.getFlag();
        state->descr += "piece " + std::to_string(static_cast<int>((*this)(source).type)) + " from " + MoveGeneration::positionToString(source) + " to " +  MoveGeneration::positionToString(target) + ", ";

        // Update en passant target square if double pawn push
        if (flag == Move::DOUBLE_PAWN_PUSH) {
            if (currentPlayer == Side::W) {
                state->enPassantTarget = target - 8;
            } else {
                state->enPassantTarget = target + 8;
            }
        } else {
            state->enPassantTarget = -1;
        }
        
        if (move.isCapture()) {
            makeCapture(move);
        } else if (flag == Move::KING_CASTLE) {
            makeKingSideCastle();
        } else if (flag == Move::QUEEN_CASTLE) {
            makeQueenSideCastle();
        } else {
            movePiece(source, target);
        }

        makePromotion(move);

        
        if (currentPlayer == Side::B) {
            ++(state->fullMoveNumber);
        }
        std::swap(currentPlayer, opponent);
    }

    // Must be move corresponding to the current state
    void Board::unmakeMove(Move const &move) {
        auto flag = move.getFlag();
        uint64_t toPosition = move.getTo();
        uint64_t fromPosition = move.getFrom();
        
        // Rollback promotions on bitboards
        if (move.isPromotion()) {
            switch (flag) {
                case Move::Flag::BISHOP_PROMOTION:
                    clearPiece(toPosition, Piece(Piece::Type::B, currentPlayer));
                    break;
                case Move::Flag::ROOK_PROMOTION:
                    clearPiece(toPosition, Piece(Piece::Type::R, currentPlayer));
                    break;
                case Move::Flag::KNIGHT_PROMOTION:
                    clearPiece(toPosition, Piece(Piece::Type::N, currentPlayer));
                    break;
                case Move::Flag::QUEEN_PROMOTION:
                    clearPiece(toPosition, Piece(Piece::Type::Q, currentPlayer));
                    break;
            }
            (*this)(fromPosition, Piece(Piece::Type::P, currentPlayer));
        }

        // Rollback captures on bitboards
        if (move.isCapture()) {
            auto movedPiece = (*this)(toPosition);
            clearPiece(toPosition, movedPiece);
            (*this)(fromPosition, movedPiece);
            uint64_t restorePosition = flag == Move::Flag::EN_PASSANT_CAPTURE ? state->enPassantTarget : toPosition;
            (*this)(restorePosition, state->capturedPiece);
        }
        

        // Rollback castling move on bitboards
        if (flag == Move::Flag::KING_CASTLE) {
            bitboards[static_cast<int>(Piece::Type::K)][currentPlayer] = 1ULL << (currentPlayer == Side::W ? 3 : 59);
            bitboards[static_cast<int>(Piece::Type::R)][currentPlayer] = 1ULL << (currentPlayer == Side::W ? 0 : 56);
        } else if (flag == Move::Flag::QUEEN_CASTLE) {
            bitboards[static_cast<int>(Piece::Type::K)][currentPlayer] = 1ULL << (currentPlayer == Side::B ? 3 : 59);
            bitboards[static_cast<int>(Piece::Type::R)][currentPlayer] = 1ULL << (currentPlayer == Side::W ? 7 : 63);
        }

        // Rollback null moves
        if (flag == Move::QUIET_MOVE || flag == Move::DOUBLE_PAWN_PUSH) {
            auto movedPiece = (*this)(toPosition);
            clearPiece(toPosition, movedPiece);
            (*this)(fromPosition, movedPiece);
        } 

        // Reload old state information
        state = state->prev;
        std::swap(currentPlayer, opponent);

    }
    bool Board::legalSlow(Move const &move) {
        makeMove(move);
        bool ok = !inCheck(opponent);
        unmakeMove(move);
        return ok;
    }
    bool Board::legalMove(Move const &move) {
        // makeMove(move);
        // bool ans = !inCheck(opponent);
        // unmakeMove(move);
        // return ans;
        const uint64_t friendlyPositions = getPositions(currentPlayer);
        uint64_t oppositionPositions = getPositions(opponent);
        uint64_t oppositionAttacks = 0;
        while (oppositionPositions) {
            uint64_t pos = Utility::bitScanForward(oppositionPositions);
            auto piece = (*this)(pos);
            // Kingless so we get attack rays "through" king as well, avoiding buggy evasion of sliding attacks
            oppositionAttacks |= getAttackMap(pos, piece.type, getPositions(opponent), friendlyPositions & ~bitboards[static_cast<int>(Piece::Type::K)][opponent], piece.side);
            if (piece.type == Piece::Type::P) {
                oppositionAttacks |= MoveGeneration::pawnAttacks[opponent][pos];
            }
            Utility::clearBit(oppositionPositions, pos);
        }
        
        return legalMove(move, oppositionAttacks, getPositions(opponent), friendlyPositions);
    }

    bool Board::isAbsolutePin(uint64_t sourceAttackPositions, uint64_t defenderPosition) {
        uint64_t friendlies = getPositions(currentPlayer) ^ defenderPosition;
        uint64_t attackers = getPositions(opponent);
        while (sourceAttackPositions) {
            uint64_t pos = Utility::bitScanForward(sourceAttackPositions);
            Piece::Type pt = (*this)(pos).type;
            uint64_t attacks = getAttackMap(pos, pt, attackers, friendlies, opponent);
            if (attacks & bitboards[static_cast<int>(Piece::Type::K)][currentPlayer]) {
                return true;
            }
            Utility::clearBit(sourceAttackPositions, pos);
        }

        return false;
    }

    // oppositionAttacks has attacks including pawn attacked squares and excluding king (so if king moves back along ray or toward queen that is not legal)
    bool Board::legalMove(Move const &move, uint64_t oppositionAttacks, uint64_t oppositionPositions, uint64_t friendlyPositions) {

        uint64_t kingPosition = bitboards[static_cast<int>(Piece::Type::K)][currentPlayer];

        auto flag = move.getFlag();

        switch (flag) {
            case Move::Flag::EN_PASSANT_CAPTURE:
                return legalEnPassantMove(move, oppositionPositions, friendlyPositions, kingPosition);
            case Move::Flag::KING_CASTLE:
                return !(oppositionAttacks & (currentPlayer == Side::W ? 0xE : 0x0E00000000000000));
            case Move::Flag::QUEEN_CASTLE:
                return !(oppositionAttacks & (currentPlayer == Side::W ? 0x78 : 0x7800000000000000));
        }            

        uint64_t fromPosition = 1ULL << move.getFrom();
        uint64_t toPosition = 1ULL << move.getTo();

        if (kingPosition == fromPosition) {
            // A king move is legal if and only if it does not move into check.
            return !(oppositionAttacks & toPosition);
        } else {
            // Compute checking pieces
            uint64_t checkingPieces = 0;
            const uint64_t oppositions = oppositionPositions;
            const uint64_t friendlies = friendlyPositions;

            while (oppositionPositions) {
                uint64_t pos = Utility::bitScanForward(oppositionPositions);
                uint64_t attacks = getAttackMap(pos, (*this)(pos).type, oppositions, friendlies, opponent);
                if (attacks & kingPosition) {
                    // Piece gives check
                    checkingPieces |= 1ULL << pos;
                }
                Utility::clearBit(oppositionPositions, pos);
            }

            int checkCount = Utility::popCnt(checkingPieces);

            // If in double check only a king move would have been valid
            if (checkCount >= 2) {
                return false;
            }

            if (checkCount == 1) {
                uint64_t checkingPiecePosition = Utility::bitScanForward(checkingPieces);
                uint64_t updatedFriendlies = (friendlies ^ fromPosition) | toPosition;

                // One check, non king move. Legal move must be to capture or if sliding piece attack block the piece giving check
                if (toPosition == checkingPieces) {
                    return !isAbsolutePin(oppositions ^ checkingPieces, fromPosition);
                } else if ((getAttackMap(checkingPiecePosition, (*this)(checkingPiecePosition).type, oppositions, updatedFriendlies, opponent) & kingPosition) == 0) {
                    return !isAbsolutePin(oppositions ^ checkingPieces, fromPosition);
                } else {
                    return false;
                }

            } else {
                return !isAbsolutePin(oppositions, fromPosition);
            }
            
        } 

    }
    
    template <Piece::Type pieceType>
    bool Board::legalNonKingMove(uint64_t x, uint64_t y, uint64_t fromPosition, uint64_t toPosition, uint64_t kingPosition) const {
        uint64_t attackerPiecePositions = bitboards[static_cast<int>(pieceType)][opponent];
        uint64_t friendly = getPositions(opponent);
        uint64_t enemy = getPositions(currentPlayer);

        // Friendly is the attacker!!!
        while (attackerPiecePositions) {
            uint64_t pos = Utility::bitScanForward(attackerPiecePositions);
            uint64_t attackMap = getAttackMap(pos, pieceType, friendly, enemy & ~kingPosition, opponent);
            // See if slider has king in check


            Utility::clearBit(attackerPiecePositions, pos);
        }
        
        
    }

    bool Board::inCheck(Side const &side) const {
        Side other = static_cast<Side>((side+1)%(Side::NUM_SIDES));
        uint64_t enemyPos = getPositions(side);
        for (int piece = 0; piece < static_cast<int>(Piece::Type::NUM_PIECES); ++piece) {
            auto type = static_cast<Piece::Type>(piece);
            
            // Positions of side checking
            uint64_t positions = getPositions(Piece(type, other));
           

            // Friendly is side attacking (other)
            uint64_t friendly = getPositions(other);
            uint64_t enemy = getPositions(side);

            while (positions) {
                uint64_t position = Utility::bitScanForward(positions);
                uint64_t attack = getAttackMap(position, type, friendly, enemyPos, other);
                if (attack & bitboards[static_cast<int>(Piece::Type::K)][side]) {
                    return true;
                }
                Utility::clearBit(positions, position);
            }
            
        }

        return false;
    }

    bool Board::legalEnPassantMove(Move const &move, uint64_t oppositionPositions, uint64_t friendlyPositions, uint64_t kingPosition) {
        // En passant captures are infrequent enough that we can check their legality by making the move and seeing if king is left in check
        makeMove(move);
        bool legal = !inCheck(opponent);
        unmakeMove(move);
        return legal;
    }
    uint64_t Board::getPositions(Piece const &piece) const {
        return bitboards[static_cast<int>(piece.type)][piece.side];
    }

    uint64_t Board::getPositions(Side const &side) const {
        uint64_t allPieces = 0;
        for (int i = 0; i < static_cast<int>(Piece::Type::NUM_PIECES); ++i) {
            allPieces |= bitboards[i][side];
        }
        return allPieces;
    }

    void Board::makeQueenSideCastle() {
        uint64_t kingLocation = currentPlayer == Side::B ? 59 : 3;
        movePiece(kingLocation, kingLocation + 2);
        movePiece(kingLocation + 4, kingLocation + 1);
        ++(state->halfMoveClock);
    }

    void Board::makeKingSideCastle() {
        uint64_t kingLocation = currentPlayer == Side::B ? 59 : 3;
        movePiece(kingLocation, kingLocation - 2);
        movePiece(kingLocation - 3, kingLocation - 1);
        ++(state->halfMoveClock);
    }

    bool Board::canKingSideCastle(uint64_t occupiedPositions) const {
        // Check if castling flags permit castling
        bool castleLegal = (state->castlingRights >> (2 * currentPlayer)) & 0b10;
        if (!castleLegal)
            return false;

        // Check if path for castling is clear. Attacks checked during legality of move check.
        uint64_t clearPiecesMask = currentPlayer == Side::W ? 0x0000000000000006 : 0x0600000000000000;
        return !(occupiedPositions & clearPiecesMask);
    }

    bool Board::canQueenSideCastle(uint64_t occupiedPositions) const {
        // Check if castling flags permit castling
        bool castleLegal = (state->castlingRights >> (2 * currentPlayer)) & 0b1;
        if (!castleLegal)
            return false;

        // Check if path for castling is clear. Attacks checked during legality of move check.
        uint64_t clearPiecesMask = currentPlayer == Side::W ? 0x0000000000000070 : 0x7000000000000000;
        return !(occupiedPositions & clearPiecesMask);
    }

    void Board::updateState() {
        state = std::make_shared<StateInfo>(state->halfMoveClock, state->fullMoveNumber, state->enPassantTarget, state->castlingRights, state, state->descr);
    }

    uint64_t Board::getAttackMap(uint64_t position, Piece::Type const pieceType, uint64_t friendlyOccupied, uint64_t oppositionOccupied, Side const &side) const {
        assert(position <= 63);
        
        switch (pieceType) {
            case Piece::Type::P:
                return MoveGeneration::pawnAttacks[side][position] & oppositionOccupied;
            case Piece::Type::K:
                return MoveGeneration::kingAttacks[position] & ~friendlyOccupied;
            case Piece::Type::N:   
                return MoveGeneration::knightAttacks[position] & ~friendlyOccupied;
            case Piece::Type::B:
                return MoveGeneration::getDiagonalAttacks(friendlyOccupied, oppositionOccupied, position) | MoveGeneration::getAntiDiagonalAttacks(friendlyOccupied, oppositionOccupied, position);
            case Piece::Type::R:
                return MoveGeneration::getFileAttacks(friendlyOccupied, oppositionOccupied, position) | MoveGeneration::getRankAttacks(friendlyOccupied, oppositionOccupied, position);
            case Piece::Type::Q:
                return getAttackMap(position, Piece::Type::B, friendlyOccupied, oppositionOccupied, side) | getAttackMap(position, Piece::Type::R, friendlyOccupied, oppositionOccupied, side);
            default:
                break;
        }
        return 0;
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

        // Update castling bits
        if (piece.type == Piece::Type::K) {
            state->castlingRights &= ~(0b11 << (2 * currentPlayer));
        } else if (piece.type == Piece::Type::R) {
            if (from & MoveGeneration::fileA) {
                state->castlingRights &= ~(0b1 << (2 * currentPlayer));
            } else if (from & MoveGeneration::fileH) {
                state->castlingRights &= ~(0b10 << (2 * currentPlayer));
            }
        } 

        if (piece.type == Piece::Type::P) {
            state->halfMoveClock = 0;
        } else {
            ++(state->halfMoveClock);
        }

        clearPiece(from, piece);
        (*this)(to, piece);
    }

    void Board::makeCapture(Move const &move) {
        auto moveFlag = move.getFlag();
        uint64_t source = move.getFrom();
        uint64_t target = move.getTo();

        if (moveFlag == Move::Flag::EN_PASSANT_CAPTURE) {
            Piece capturePiece = (*this)(state->enPassantTarget);
            clearPiece(state->enPassantTarget, capturePiece);
            state->capturedPiece = capturePiece;
        } else {
            Piece capturePiece = (*this)(target);
            clearPiece(target, capturePiece);
            state->capturedPiece = capturePiece;
        }

        auto piece = (*this)(source);
        clearPiece(source, piece);
        (*this)(target, piece);

        state->halfMoveClock = 0;
    }

    void Board::makePromotion(Move const &move) {
        auto moveFlag = move.getFlag();
        switch (moveFlag) {
            case Move::Flag::BISHOP_PROMOTION:
            case Move::Flag::BISHOP_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::B, currentPlayer));
                break;
            case Move::Flag::KNIGHT_PROMOTION:
            case Move::Flag::KNIGHT_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::N, currentPlayer));
                break;
            case Move::Flag::QUEEN_PROMOTION:
            case Move::Flag::QUEEN_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::Q, currentPlayer));
                break;
            case Move::Flag::ROOK_PROMOTION:
            case Move::Flag::ROOK_PROMO_CAPTURE:
                (*this)(move.getTo(), Piece(Piece::Type::R, currentPlayer));
                break;
        }
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

        int position = 63;    
        for (int i = 0; i < 8; ++i) {
            if (i == 8) {
                ss >> token;
            } else {
                std::getline(ss, token, '/');
            }
            int offset = 0;
            for (int j = 0; j < 8-offset; ++j) {
                if (token[j] <= '8' && token[j] >= '1') {
                    position -= token[j]-'0';
                    offset += token[j]-'1';
                } else {
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
                    position--;
                }
            }
        }

        ss = std::stringstream(token);
        ss >> token >> token;

        // Side to play
        currentPlayer = token == "w" ? Side::W : Side::B;
        opponent = token == "w" ? Side::B : Side::W;

        ss >> token;
        uint8_t castlingRights = 0;
        // Castling rights [Black King Side, Black Queen Side, White King Side, White Queen Side]
        if (token != "-") {
            for (char c : token) {
                if (c == 'K') 
                    castlingRights |= 0b0010;
                else if (c == 'k')
                    castlingRights |= 0b0001;
                else if (c == 'Q') 
                    castlingRights |= 0b1000;
                else if (c == 'q')
                    castlingRights |= 0b0100;
            }
        }

        ss >> token;

        uint64_t enPassantTarget = -1;

        // En passant target square
        if (token != "-") {
            enPassantTarget = Utility::flattenCoordinates(token[0]-'a', token[1]-'0');
        } 

        ss >> token;
        int halfMoveClock = std::stoi(token);
        ss >> token;
        int fullMoveNumber = std::stoi(token);

        state = std::make_shared<StateInfo>(halfMoveClock, fullMoveNumber, enPassantTarget, castlingRights, nullptr);
    }   
}