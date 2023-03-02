#ifndef LESSON_01_HANDLER_H
#define LESSON_01_HANDLER_H

#include <iostream>

class IHandler
{
public:
    IHandler() = default;
    virtual ~IHandler() = default;
    virtual bool handle(std::string& _command) = 0;

};

class ExitChecker : public IHandler
{
private:
    IHandler *next;

public:
    ExitChecker( IHandler *_next)
    : next (_next)
    {};

    ~ExitChecker() override
    {
        if (next != nullptr)
            delete next;
    };

    bool handle(std::string& _command) override
    {
        if (_command == "exit")
        {
            std::cout << "EXIT command detected" << std::endl;
            return true;
        }
        else
            return next->handle(_command);
    };

};

class DefaultHandler : public IHandler
{
    bool handle(std::string& _command) override
    {
        std::cout << "command is not recognized" << std::endl;
        return false;
    };
};


#endif //LESSON_01_HANDLER_H
