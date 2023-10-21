# Instantiating Vulkan Objects

vulkan object instantiation is a little different than some more popular patterns.

instantiation parameters are compiled into an info struct, 
which is then passed as a reference to a vulkan function.

the variable to be initialized is also initialized with `VK_NULL_HANDLE`,
and passed as a reference to the vulkan function.

the vulkan function will then: 
1. accept a reference to the info struct and the variable to initialize, 
2. will initialize the referenced variable, 
3. and will then return a `vkResult` enum that can be checked for success.


# Example Code:

## the basic structure for instantiating vulkan objects is a 3-step process:
the following pattern is followed for all vulkan functions & creation of vulkan objects.

### 1. create an info structure that describes the parameters of the object.
- the `sType` field is required for all info structs, and is used to tell vulkan what type of info struct is being passed.
- other fields are specific to the object being created, and are documented in the vulkan spec.
- the `p*` fields are typically references. in this case, it's a string literal, but in the next example we see a reference to this very struct.

``` cpp
VkApplicationInfo appInfo{};
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; // all info structs take the sType field
appInfo.pApplicationName = "Vulkan Application";
appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
appInfo.pEngineName = "Walrus Engine";
appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
appInfo.apiVersion = VK_API_VERSION_1_3;
...
```

### 2. sometimes a reference to one info struct can be passed as a parameter of another info struct.
``` cpp
VkInstanceCreateInfo createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // standard sType field
createInfo.pApplicationInfo = &appInfo; // <-- a reference to the previous info struct
...
```

### 3. create the vulkan object and check for success.
``` cpp
VkInstance _instance = VK_NULL_HANDLE;
vkResult success_fail = vkCreateInstance(&createInfo, nullptr, &_instance);

if (success_fail != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
}
```

- Note that this is done a little differently than what might be expected.
- Instead of something like `auto _instance = vkCreateInstance(params...)`, we pass reference of the info struct and the variable to initialize to the vulkan function, and the function will initialize the passed variable.
- This is because the vulkan functions _instead_ return a `vkResult` which is an enum that can be compared to the vulkan defined `VK_SUCCESS` to check for success.


- in this code, I created a macro `VK_CHECK` to assert for success instead of checking for success and throwing an error every time.

``` cpp
#define VK_CHECK(x) assert(x == VK_SUCCESS)

...

VkInstance _instance = VK_NULL_HANDLE;
VK_CHECK(vkCreateInstance(&createInfo, nullptr, &_instance));
```
