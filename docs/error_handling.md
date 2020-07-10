# Error Handling

Using this library, there is two distinct types error:

 * Fatal errors
 * Sementic errors

## Fatal Errors

Fatal errors are 

Those errors are handle using exceptions, they occures while the state
of a module or container become totaly unuseable. There is no-thing todo,
the only things you can do is restarting your program.


<p align="center">
    <img src="media/cat_exception_meme.PNG" height=300 alt="Image" />
</p>


## Sementic Errors

This errors are related to programing with netero. They are not fatal, they
warn you while somethings is going wrong with what you asked or that
you have asked something impossible in the current state of a container.

Sementics errors are values returned by functions/methods. And they
are likely to be used for control flow.

