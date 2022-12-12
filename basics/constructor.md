# Constructor, things that you didn't think through

## :brain: Why we need default constructor

- Note from [Smart-Pointer - Constructors](https://lokiastari.com/blog/2015/01/23/c-plus-plus-by-example-smart-pointer-part-iii/index.html)
- The default constructor is useful when the type is used in a context where objects of the type **need to be instantiated dynamically by another library**
- (an example is a container resized; when a container is made larger by a resize, new members will need to be constructed, it is the default constructor that will provide these extra instances).
- The default constructor is usually very trivial and thus worth the investment.