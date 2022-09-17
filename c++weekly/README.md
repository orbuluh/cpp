# C++ weekly notes

# Ep 322 - Top 4 Places To Never Use `const`
- don't `const` non-reference return types (it breaks copy elision when return)