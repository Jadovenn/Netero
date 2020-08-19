# Error Handling

This section give you more information on how to deal with errors.
This information are very basics and obvious. But you might be interrested
to read them if you work on a critical system where error management could be a big deal.

Errors could be identified into two distinc cathegories:
 * Fatal errors
 * Sementic errors

## Fatal Errors

While a container or the state of a module fall into an unretrivable error

Those errors are handle using exceptions, you are not suppose 
to give a try at catching thems at each api calls. A try catch
block at the very begining of your program or at strategic sementic point is sufficient.

<p align="center">
    <img src="cat_exception_meme.PNG" height=300 alt="Exception cat meme" />
</p>

In case of exception there is noting else to do, than notify the user of a fatal
error and/or create an error log file. And terminate the programe.

e.g:
 - You try to use the graphics module on a computer with no GPU (but it is mendatory for this module to work properly)
 - Memory is exauthted

The `netero::exception` is the base class of every exceptions use in the library. Then you can easly disociate thems from the standard ones.

## Sementic Errors

This errors are related to programming with netero api. They are not fatal, they
warns you while somethings is going wrong with what you asked or that
you have asked something impossible in the current state of a container/module.

Sementics errors are values returned by functions/methods. And they
are likely to be used for control flow.

e.g:
 - You ask the audio module to play a sound but you forgot to choose a device.

Control flow error are usually defined in each class or namespace where they are necessary like `netero::audio::RtCode`


