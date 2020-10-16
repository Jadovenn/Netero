/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file IObserver.hpp
 * @brief Observer pattern interface.
 */

namespace Netero {

/**
 * Observer interface.
 */
class IObserver {
    public:
    virtual ~IObserver() = default;

    /**
     * Method called while a subject emit.
     */
    virtual void notify() = 0;
};

/**
 * Subject interface.
 */
class ISubject {
    public:
    virtual ~ISubject() = default;

    /**
     * Emit a call to connected Observers.
     */
    virtual void EmitToObservers() = 0;

    /**
     * @brief disconnect given object to actual obs/subject
     */
    virtual void Disconnect(IObserver*) = 0;

    /**
     * @brief connect to other obs/subject
     */
    virtual void Connect(IObserver*) = 0;
};

/**
 * Connectible interface.
 * For class that only need Connect/Disconnect logic
 */
class IConnectibleDelegate {
    public:
    virtual ~IConnectibleDelegate() = default;

    /**
     * @brief disconnect from the given IConnectibleDelegate
     */
    virtual void Disconnect(IConnectibleDelegate*) = 0;

    /**
     * @brief connect to other IConnectibleDelegate
     */
    virtual void Connect(IConnectibleDelegate*) = 0;
};

} // namespace Netero
