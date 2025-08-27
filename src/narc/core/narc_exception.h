#pragma once

#include <SDL_error.h>

#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

class NarcInitFailedException
    : public std::runtime_error
{
public:
    explicit NarcInitFailedException(const std::string &message)
        : std::runtime_error(message) {}

    explicit NarcInitFailedException()
        : std::runtime_error("Narc init failed without message.") {}

    // -------------------------------------------------------

    static NarcInitFailedException narcWasStartedBefore()
    {
        return NarcInitFailedException("Narc was already started before!");
    }

    static NarcInitFailedException unableToInitSdlWithSignalException(int errorCode)
    {
        std::ostringstream oss;
        oss << "Unable to initialize SDL library (Error code " << errorCode << "): " << SDL_GetError();

        return NarcInitFailedException(oss.str());
    }

    static NarcInitFailedException unableToInitSdlWindowException()
    {
        return NarcInitFailedException(std::string("Unable to initialize SDL window: ") + SDL_GetError());
    }

    static NarcInitFailedException unableToInitSdlRendererException()
    {
        return NarcInitFailedException(std::string("Unable to initialize SDL renderer: ") + SDL_GetError());
    }
};

class NarcShutdownFailedException
    : public std::runtime_error
{
public:
    explicit NarcShutdownFailedException(const std::string &message)
        : std::runtime_error(message) {}

    explicit NarcShutdownFailedException()
        : std::runtime_error("Narc shutdown failed without message.") {}

    // -----------------------------------------------------------

    static NarcShutdownFailedException narcWasNotStartedBefore()
    {
        return NarcShutdownFailedException("Narc wasnt started before!");
    }
};
