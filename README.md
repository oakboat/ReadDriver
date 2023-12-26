# ReadDriver
A kernel memory read driver using data ptr hook
# Usage
Change NtUserRegisterTouchPadCapable offset in Entry.cpp
https://github.com/oakboat/ReadDriver/blob/142da1432a15b16f58dd766c98ac9c0a907baeed/MyDriver1/Entry.cpp#L22
You Can find it with ida:
![image](https://github.com/oakboat/ReadDriver/assets/27909321/789c9717-806b-4be0-8ca5-dc9089f4ad48)
So in my computer, my offset is x664E0.
