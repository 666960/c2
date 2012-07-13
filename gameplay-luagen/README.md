## Usage
To generate the Lua script bindings for gameplay, run the generate-doxygen-xml.bat (or .sh) script. Then, on Windows, open the gameplay-luagen Visual Studio solution and build and run the gameplay-luagen project. On Mac, open the gameplay-luagen XCode workspace and build and run the gameplay-luagen project.

There are also prebuilt binaries in the gameplay/bin folder.


## Tips for Using Lua with gameplay
- On any function in gameplay that returns a pointer that is owned by the user calling that function (i.e. a create() function), add @script{create} as the last line of its doxygen comments.
- On any function, variable, class, struct, enum, etc. that should not be accessible from Lua (except for things that are static to a .cpp file, which is already ignored), add @script{ignore} to its doxygen comments.
- On any functions, variables, classes, structs, enums, etc. that are local to a .cpp file, declare them as static 1) because it is good practice and 2) so that Lua does not generate bindings for them.
- To get printf/GP_WARN-like functionality using gameplay and Lua, use printError(string.format("...", ...)).
- To do integer like comparisons or casts on a number variable x in Lua, use math.floor(x)
- Make sure all your member function calls use ':' instead of '.'
- Remember to access all gameplay variables, including static and global variables with '()' on the end of the name.
- Primitive data type arrays and object arrays are both inefficient when created in Lua and passed to C++, so try to minimize this.
- There is no reasonable way to unload a Lua script (one would have to parse the script completely to find all global function and variable names and then set those global table entries to nil and invoke the garbage collector).
    - The recommended usage pattern is to put each script's variables and functions inside a table (see Lua technical note 7). i.e.

    -- If you want to load the module at most once, add a line like this.
    if Module then return end
    
    -- Declare the module Module.
    Module = {}
    
    -- Declare a variable within the module.
    Module.a = 47
    
    -- Declare a function within the module.
    function Module.myFunc()
       return Module.a + 17
    end
- Note: you can't pass an enum to a function that doesn't explicitly take an enum (i.e. Control::setTextColor, which takes an unsigned char). In these cases, you need to go look up the enum values and pass them directly.

## Unsupported Features
- operators
- templates
- variable arguments
- Lua doesn't support as many types as C++ so if there are functions that overload on parameters with types that overlap in Lua, the overloading won't work properly (i.e. char, short, int, long, float, double and all corresponding unsigned variants overlap in Lua).

### To Do List
- Get Mac OS X and iOS working.
- Fix hierarchy inheritance
- Fix memory leaks in gameplay-luagen and in generated code.
    * Add "@script{create}" to the appropriate gameplay functions.
    * Add "@script{own}" to array parameters that are owned by the function or class they are passed to?
- Add a global function that implements casting for use from Lua scripts (i.e. to downcast from a Control to a Button).

### Future Feature List
- Add support for users to generate bindings for their own classes.
- Look into updating bindValue() to support binding to any Lua script function.


## Disclaimer
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE.