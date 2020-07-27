/*
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <memory>

namespace netero::patterns {

template<class T>
class ISingleton {
    public:
     virtual ~ISingleton() = 0;

     template<typename ...Args>
     static void Initialize(Args... someArguments) {
         if (!_instance) {
             _instance = std::make_unique<T>(std::forward(someArguments)...);
         }
     }

     static void Release() {
         _instance.reset();
         _instance = nullptr;
     }

     static T* GetInstance() {
         return _instance ? _instance : nullptr;
     }

     private:
     static std::unique_ptr<T> _instance;
};

}
