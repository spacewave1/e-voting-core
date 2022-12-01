//
// Created by wnabo on 10.06.2022.
//

#ifndef VOTE_P2P_HILLLENGTHEXCEPTION_H
#define VOTE_P2P_HILLLENGTHEXCEPTION_H

#include <exception>

class hillLengthException: public std::exception
{
    [[nodiscard]] const char* what() const noexcept override
    {
        return "Invalid length";
    }

public:
    hillLengthException() _GLIBCXX_NOTHROW = default;
};

#endif //VOTE_P2P_HILLLENGTHEXCEPTION_H