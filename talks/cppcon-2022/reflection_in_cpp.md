# Reflection in C++ - Past, Present, and Hopeful Future - Andrei Alexandrescu

Formal definition: reflection is the ability of a program to observe its own code and shape its behavior accordingly.

- In C++, we are going to focus on compile-time reflection, not runtime, and the focus is on generic code.
- A good definition for C++ reflection would be:


> the ability of a template to query details of its template parameters and shape its definition accordingly.

- This allows the template to morph itself by adding or removing data members, methods, type definitions, and more, depending on its template parameters.


## **Designing Code by Reflection in C++**

### Two Main Components:

1. **Reflection Proper**: Querying attributes of a type (e.g., methods, copyability, virtual destructors).
    - What was the author of the type thinking?
2. **Insertion**: Using information from reflection to change or generate new code.
    - Also known as code generation based on reflection.

### Three Stages of Design by Reflection:

1. **Extraction (Reflection Proper)**:
    - Query and extract information about types.
    - Represented in the current C++ proposal.
2. **Processing (Synthesis)**:
    - Use extracted information to create new information.
    - Evaluated during compilation.
    - More power needed for full capabilities.
3. **Generation (Insertion)**:
    - Use processed information to generate new code.
    - Passed back to the compiler for further compilation.
    - Not well-represented in the current C++ proposal, needs improvement.

## **Andre's Theory of Programming Language Design**

### Human Endeavor in Programming:

1. **Reasoning About Self and Others**:
    - A fundamentally human skill, e.g., empathy, understanding context.
    - Suggested to be incorporated into programming languages.

### Contrast with Technology-Driven Features:

1. **Configurable Syntax**:
    - Possible in technology but not a human endeavor.
    - Receives periodic interest but not aligned with how the human mind works.
    - Suggested to be avoided in programming language design.

### Key Takeaways:

- Programming languages should focus on features that align with human cognitive abilities.
- Avoid features like configurable syntax that, while technologically possible, are not naturally intuitive for humans.

## **Crisis in Software Development: The Million Lines of Code Challenge**

### The Problem:

1. **Growing Codebases**: Every project is heading towards 1 million lines of code or more.
2. **Bug Rate**: One bug per 1,000 lines in large projects, even with high budgets.
3. **Language-Independent**: Bug rate is proportional to lines of code, regardless of the programming language.
4. **Super-Linear Growth**: Complexity grows faster than the size of the code.

### Cultural Attitude:

1. **Code as Achievement**: Many see more lines of code as an accomplishment, not a liability.
2. **Lack of Attention**: Not enough focus on the sheer size of codebases.

### Current Language Mechanisms:

1. **Type Systems, Concepts, Checkers, Linters**: Constrain code but also force more lines to be written.
2. **Eliminate Good and Bad Programs**: Ideal features should only eliminate bad programs, but that's not always the case.

### Python vs C++:

1. **Python's Simplicity**: Python users can achieve more with fewer lines, questioning the need for complex type systems.

### Proposed Solution:

1. **Increase Leverage**: Look for language features that allow more to be done with fewer lines of code.
2. **Generative Aspect**: Good language features should enable more correct programs from fewer lines of code.

### Key Takeaways:

- The size of codebases is a growing problem that needs more attention.
- Language features should aim to reduce the number of lines needed for correct programs.
- There's a need to shift the cultural attitude that equates more lines of code with success.

## **The Boilerplate Problem in Software Development**

### Common Misconception:

1. **Not Just Boilerplate**: While boilerplate code is a problem, it's not the only issue. There's also creative code that adds to the complexity.

### Types of Boilerplate:

1. **Obvious Boilerplate**: Contiguous blocks of repetitive code that are easy to spot.
2. **Interspersed Boilerplate**: Mixed with functionality, making it hard to remove. Includes repeated declarations and signatures.

### Language Limitations:

1. **Namespace Detail**: Indicates a failure of the language to provide more elegant solutions.
2. **Template Complexity**: C++ templates require a lot of boilerplate, especially for recursion and specialization.

### Real-World Example:

1. **Boost Library**: Despite being a great library, it contains a lot of boilerplate.
2. **Unordered Map**: 7,000 lines of code, making it hard to understand how collisions are solved or where the actual work is done.

### Beyond Boilerplate:

1. **Reflection**: While boilerplate is a problem in C++, reflection offers more than just reducing boilerplate.

### Key Takeaways:

- Boilerplate is a symptom of deeper issues, including language limitations and the complexity of creative code.
- Different types of boilerplate exist, some more insidious than others.
- There's a need for language features that reduce boilerplate and increase code efficiency.

## **The Challenge of Creating a "Tainted String" Type in C++**

### The Concept:

1. **Tainted String**: A string type that is not trusted and needs validation.
2. **Strong Typing**: Known as "strong type def" or "strong using" in C++.

### Implementation Challenges:

1. **Type Aliasing**: Using **`typedef`** or **`using`** only creates an alternate name, not a new type.
2. **Public Inheritance**: Not recommended for value types.
3. **Private Inheritance**: Recommended but comes with its own set of problems.

### Code Example:

```cpp
cppCopy code
struct tainted_string : private std::string {
	using std::string::empty;
	using std::string::size;
  //...
};
```

### Issues with Private Inheritance:

1. **Using Directives**: Need to explicitly specify which members to inherit.
2. **Constructors**: Don't take the right types.
3. **Iterators**: Need to create a new iterator type for **`tainted_string`**.
4. **Methods**: Methods that take or return **`std::string`** need to be rewritten for **`tainted_string`**.

### Result:

1. **Code Bloat**: 700 lines of code for a type that does nothing algorithmically.
2. **Lack of Abstraction**: Can't encode the notion of copying a type, which is the opposite of abstraction.

### Key Takeaways:

- Creating a new type based on an existing one is not straightforward in C++.
- The process involves a lot of boilerplate and manual work.
- This example highlights the need for better abstraction mechanisms in programming languages.

## **The Challenge of Customizing Existing Types in C++**

### The Concept:

1. **Custom Types**: Often, developers want a type that is almost identical to an existing one but with slight modifications.
2. **Examples**:
    - A hash table that throws an exception on bracket operator use.
    - An **`std::vector`** that grows automatically without throwing out-of-bounds exceptions.
    - A URL or file type that is mostly like **`std::string`** but with encoding/decoding features.

### The Irony of Inheritance:

1. **Original Promise**: Inheritance was supposed to allow easy customization of existing types.
2. **Reality**: Inheritance from value types is discouraged in C++ literature.
3. **Workaround**: Developers still use inheritance from value types, despite the risks.

### Key Issues:

1. **Customization**: The ability to tweak an existing type for specific needs is crucial for true code reuse.
2. **Inheritance Limitations**: Inheritance often doesn't deliver on its promise of easy customization, especially for value types.

## **The Limitations of Container Adapters in C++**

### The Concept:

1. **Container Adapters**: These are wrappers around existing container types to provide specific functionalities.
2. **Examples**: **`std::stack`**, **`std::queue`**, etc.

### The Problem:

1. **Rigidity**: Container adapters are rigid and code to the least common denominator of the container they adapt.
2. **Limited Success**: They are not widely used because of their limitations.

### Desired Features:

1. **Adaptive Behavior**: Ideally, a container adapter should adapt its functionality based on the capabilities of the underlying container.
2. **Optional Enhancements**: Should offer enhanced functionality when the underlying container supports it.

### Example:

1. **`std::stack`**: Developers often avoid using **`std::stack`** because it lacks features like random access, which might be available in the underlying container like **`std::vector`**.

### Key Takeaways:

- Container adapters in their current form are too rigid and don't adapt to the capabilities of the underlying container.
- There's a need for more flexible container adapters that can offer enhanced functionalities based on the capabilities of the underlying container.

## Pitfalls of attempting to write container-independent code. (Scott Meyers' View)

1. **Effective STL Item 2**: Scott Meyers explaininged the pitfalls of attempting to write container-independent code.
2. **Limited Recourse**: You have very little flexibility to adapt the code to specific needs or optimizations.

### Key Takeaways:

- The idea of container-independent code is largely an illusion due to the rigid limitations it imposes.
- This approach often leads to more boilerplate and less efficient code.

- Static reflection is on the verge of becoming mainstream in C++ due to advancements in compile-time evaluation and data structure manipulation.
- The language wasn't ready for it before because it lacked good constant compile-time evaluation and the ability to create complex data structures during compilation.
- Recent additions like **`constexpr`** and the ability to use **`new`** during compilation are making C++ more suitable for compile-time programming and reflection.
- Earlier approaches to static reflection in C++ were template-based, which were hard to work with and led to memory consumption issues during compilation.
- There has been a proposal for inheritance-based reflection, but it proved to be inefficient and difficult to use in practice.
- The optimal approach for C++ is value-based reflection, allowing developers to use familiar C++ constructs like loops and tests during compile-time against type information.

## Proposed C++ primitives

```cpp
#incluide <meta>

// change of language proposed
// ^T: "reify T" where T is identifier/expression  ...etc 
//     yields a constexpr value of type meta::info
// convert a value back to alias through [:e:]
//     where e is a compile-time value of type meta::info
//     this yields a template name or type.
// [:^T:] is T, and ^[:e:] is e, they are like & and * for lvalues.

tempalte<typename T>
requires(std::is_enum_v<T>) constexpr std::string to_string(T value) {
  template for (constexpr auto e : std::meta::members_of(^T)) {
    if ([:e:] == value) {
      return std::string(std::meta::name_of(e));
    }
  }
  return "<unnamed>";
}
```

- The proposed C++ code example demonstrates the use of static reflection to convert enumerated types to strings.
- The code introduces two new language constructs: **`^T`** to "reify" a type into a value, and **`[:e:]`** to convert a value back to a type or alias.
- The **`template for`** loop iterates over all the members of the enumerated type, using the **`std::meta::members_of`** function.
- The **`if`** statement compares the runtime value of the enumerated type with each member, converting it back to a type using the **`[:e:]`** syntax.
- The function returns the name of the enumerated value if it matches, or "<unnamed>" otherwise.
- The code aims to reduce boilerplate by automating the generation of code for converting enumerated types to strings.
- The example highlights the power of static reflection and compile-time programming in C++, allowing for both compile-time and runtime functionality.
- The proposal aims for minimal changes to the language, focusing on adding powerful features for metaprogramming.
- The use of iteration is emphasized as crucial for reducing the amount of code and for generating boilerplate automatically.
- The example also suggests that the new syntax could be used in a **`switch`** statement, further enhancing its utility.

```cpp
tempalte<typename T>
requires(std::is_enum_v<T>) constexpr std::string to_string(T value) {
  switch (value) {
    template for (constexpr auto e : std::meta::members_of(^T)) {
      case [:e:]: return std::string(std::meta::name_of(e));
    }
  }
  default: return "<unnamed>";
}
```

- The updated C++ code example introduces algorithm selection based on the size of the enumerated type.

```cpp
tempalte<typename T>
requires(std::is_enum_v<T>) constexpr std::string to_string(T value) {
  if constexpr (std::meta::members_of(^T).size() <= 7) {
    template for (constexpr auto e : std::meta::members_of(^T)) {
      if ([:e:] == value) {
        return std::string(std::meta::name_of(e));
      }
    }
  } else {
    switch (value) {
      template for (constexpr auto e : std::meta::members_of(^T)) {
        case [:e:]: return std::string(std::meta::name_of(e));
      }
    }
  }
  return "<unnamed>";
}
```

- The function **`to_string`** now uses an **`if constexpr`** statement to decide between two different algorithms for converting an enumerated value to a string.
- If the size of the enumerated type is less than or equal to 7, the function uses a **`template for`** loop to iterate over each member and compare it to the input value.
- If the size is greater than 7, the function uses a **`switch`** statement to improve performance. The **`case`** labels are generated using the **`[:e:]`** syntax to convert compile-time values back to types.
- This approach allows the function to adapt its behavior based on the characteristics of the input type, optimizing for either code size or performance as needed.
- The example demonstrates the power of static reflection and compile-time programming to make decisions about code generation, a capability usually reserved for compiler writers.
- This level of control allows for more efficient and adaptable code, but also comes with the responsibility to use these powerful features wisely.
- The code example uses Andrei's "Rule of Seven" to decide between two different algorithms for converting an enumerated type to a string. If the enumerated type has seven or fewer members, it uses a cascade of **`if`** statements. Otherwise, it uses a **`switch`** statement.
- The most powerful part of the example is the common path, represented by the line **`return "<unnamed>";`**. This line is shared between the two different algorithms, demonstrating the ability to reuse code even when different paths are taken.
- The limitation of **`if constexpr`** is that it introduces a new scope, preventing the definition of a variable that can be used in the common path. The example is carefully chosen to avoid illustrating this limitation.
- The use of **`if constexpr`** is "painfully close" to being useful for more general code injection into the existing scope, but falls short because it introduces a new scope.
- The example illustrates the high density of code and the reuse of common paths, which are key advantages of this approach.
- While compilers already perform similar optimizations, the general case enabled by static reflection and compile-time programming allows for more sophisticated algorithm selection that compilers cannot do.
- The ability to choose algorithms and generate code conditionally gives programmers immense power, similar to what compiler writers have, but it should be used responsibly.