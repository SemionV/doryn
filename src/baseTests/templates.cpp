#include "dependencies.h"

namespace basics_varprint2_hpp
{
    template <typename T>
    void print(T arg) {
        std::cout << arg << '\n';  // print passed argument
    }

    template <typename T, typename... Types>
    void print(T firstArg, Types... args) {
        print(firstArg);  // call print() for the first argument
        print(args...);   // call print() for remaining arguments
    }

    TEST_CASE( "Check variadic template basics/varprint2.hpp", "[.][templates]" ) 
    {
        print(2, "test", 3.4);
    }
}

namespace basics_foldtraverse_cpp
{
    // define binary tree structure and traverse helpers:
    struct Node {
        int value;
        Node* left;
        Node* right;
        Node(int i = 0) : value(i), left(nullptr), right(nullptr) {}
        //...
    };
    //Member pointers to Node
    Node* Node::*left = &Node::left;
    Node* Node::*right = &Node::right;
    int Node::*value = &Node::value;

    // traverse tree, using fold expression:
    template <typename T, typename... TP>
    Node* traverse(T np, TP... paths) {
        return (np->* ... ->*paths);  // np ->* paths1 ->* paths2 ...
    }

    TEST_CASE( "Check variadic template basics/foldtraverse.cpp", "[.][templates]" ) 
    {
        // init binary tree structure:
        Node* root = new Node{0};
        root->left = new Node{1};
        root->left->right = new Node{2};
        //...
        // traverse binary tree:
        Node* node = traverse(root, left, right);
        //...

        std::cout << "node value: " << node->value << std::endl;
        std::cout << "node value': " << node->*value << std::endl;
        std::cout << "left size: " << sizeof(left) << std::endl;
        std::cout << "right size: " << sizeof(right) << std::endl;
        std::cout << "node size: " << sizeof(*root) << std::endl;
        std::cout << "left value size: " << sizeof(root->*left) << std::endl;
        std::cout << "right value size: " << sizeof(root->*right) << std::endl;
        std::cout << "value value size: " << sizeof((root->*value)) << std::endl;
    }
}

namespace test_fold_exprexxions
{
    template<int... arguments>
    int sumAllArguments()
    {
        return (... + arguments);
    }

    template<int... arguments>
    int sumAllArgumentsLeft()
    {
        return (arguments + ...);
    }

    template<int... arguments>
    int addArguments(int value)
    {
        return (value + ... + arguments);
    }

    template<int... arguments>
    int addArgumentsLeft(int value)
    {
        return (arguments + ... + value);
    }

    template<int... arguments>
    int addConstant(int constant)
    {
        return((arguments + constant) + ...);
    }

    TEST_CASE( "Test fold expressions: ( ... op pack )", "[.][templates]" ) 
    {
        // (((1 + 2) + 3) + 4)
        auto sum = sumAllArguments<1, 2, 3, 4>();
        REQUIRE(sum == 10);

        // (1 + (2 + (3 + 4)))
        sum = sumAllArgumentsLeft<1, 2, 3, 4>();
        REQUIRE(sum == 10);

        // (((1 + 2) + 3) + 4)
        sum = addArguments<2, 3, 4>(1);
        REQUIRE(sum == 10);

        // (2 + (3 + (4 + 1)))
        sum = addArgumentsLeft<2, 3, 4>(1);
        REQUIRE(sum == 10);

        // ((2 + 1) + ((3 + 1) + ((4 + 1) + (1 + 1))))
        sum = addConstant<1, 2, 3, 4>(1);
        REQUIRE(sum == 14);
    }

    template<std::size_t...>
    struct Indices
    {
    };

    template<typename T, std::size_t... Idx>
    void printArguments(T container, Indices<Idx...>)
    {
        basics_varprint2_hpp::print(std::get<Idx>(container)...);
    }

    TEST_CASE( "Indices struct(TMP collection type)", "[.][templates]" ) 
    {
        std::array<std::string_view, 5> container = {"String1", "String2", "String3", "String4", "String5"};
        printArguments(container, Indices<0, 1, 2>());
    }

    // helper: checking validity of \T\TI{f}(\TI{args}...)} for F \TI{f} and Args...
    // \TI{args:
    template <typename F, typename... Args,
            typename = decltype(std::declval<F>()(std::declval<Args&&>()...))>
    std::true_type isValidImpl(void*);

    // fallback if helper SFINAE'd out:
    template <typename F, typename... Args>
    std::false_type isValidImpl(...);

    // define a lambda that takes a lambda f and returns whether calling f with args
    // is valid
    inline constexpr auto isValid = [](auto f)
    {
        return [](auto&&... args)
        {
            return decltype(isValidImpl<decltype(f), decltype(args)&&...>(nullptr)){};
        };
    };

    // helper template to represent a type as a value
    template <typename T>
    struct TypeT {
        using Type = T;
    };

    // helper to wrap a type as a value
    template <typename T>
    constexpr auto type = TypeT<T>{};

    // helper to unwrap a wrapped type in unevaluated contexts
    template <typename T>
    T valueT(TypeT<T>);  // no definition needed

    constexpr auto isDefaultConstructible = isValid([](auto x) -> decltype((void)decltype(valueT(x))())
    {
    });

    TEST_CASE( "Generic Lambda SFINAE", "[templates]" )
    {
        REQUIRE(isDefaultConstructible(type<int>));
        REQUIRE(!isDefaultConstructible(type<int&>));
    }
}