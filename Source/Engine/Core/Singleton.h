#pragma once

namespace neu {
    /// <summary>
    /// A thread-safe singleton base class template using the Curiously Recurring Template Pattern (CRTP).
    /// This class ensures that only one instance of the derived class can exist throughout the program's lifetime.
    /// The singleton is implemented using the "Meyers' Singleton" pattern with static local variables,
    /// which guarantees thread-safety in C++11 and later standards.
    /// 
    /// Usage: class MyClass : public Singleton&lt;MyClass&gt; { ... };
    /// Access: MyClass::Instance().SomeMethod();
    /// </summary>
    /// <typeparam name="T">The derived class type that should be a singleton</typeparam>
    template <typename T>
    class Singleton {
    public:
        /// <summary>
        /// Deleted copy constructor to prevent copying of singleton instances.
        /// This ensures that no additional instances can be created through copy construction.
        /// </summary>
        /// <param name="other">The singleton instance to copy (deleted parameter)</param>
        Singleton(const Singleton&) = delete;

        /// <summary>
        /// Deleted copy assignment operator to prevent assignment of singleton instances.
        /// This ensures that singleton instances cannot be assigned to each other.
        /// </summary>
        /// <param name="other">The singleton instance to assign from (deleted parameter)</param>
        /// <returns>Reference to this singleton instance (never actually returns due to deletion)</returns>
        Singleton& operator = (const Singleton&) = delete;

        /// <summary>
        /// Returns the single instance of the derived class type T.
        /// Uses the "Meyers' Singleton" pattern with a static local variable, which is:
        /// - Thread-safe (guaranteed by C++11 standard)
        /// - Lazy-initialized (created on first access)
        /// - Automatically destroyed at program termination
        /// </summary>
        /// <returns>Reference to the unique instance of type T</returns>
        static T& Instance() {
            // Static local variable ensures thread-safe lazy initialization
            // The instance is created on first call and persists for program lifetime
            static T instance;
            return instance;
        }

    protected:
        /// <summary>
        /// Protected default constructor to prevent direct instantiation while allowing derived classes to construct.
        /// This ensures that:
        /// - Only the derived class can create instances (through the Instance() method)
        /// - External code cannot directly instantiate singleton objects
        /// - The singleton pattern is properly enforced
        /// </summary>
        Singleton() = default;
    };
}