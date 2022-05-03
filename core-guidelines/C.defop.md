# C.defop: Default Operations
- By default, the language supplies the default operations with their default semantics.
- However, a programmer can disable or replace these defaults.

# C.20: If you can avoid defining default operations, do.
- It's the simplest and gives the cleanest semantics.
- This is known as "the rule of zero".

# C.21: If you define or `=delete` any copy, move, or destructor function, define or `=delete` them all

