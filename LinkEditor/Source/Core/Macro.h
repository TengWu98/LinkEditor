#pragma once

#define LINK_EDITOR_EXPAND_MACRO(x) x
#define LINK_EDITOR_STRINGIFY_MACRO(x) #x

#ifdef LINK_EDITOR_ENABLE_ASSERTS
    // Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
    // provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
    #define LINK_EDITOR_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { LOG_ERROR(msg, __VA_ARGS__); __debugbreak(); } }
    #define LINK_EDITOR_INTERNAL_ASSERT_WITH_MSG(type, check, ...) LINK_EDITOR_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    #define LINK_EDITOR_INTERNAL_ASSERT_NO_MSG(type, check) LINK_EDITOR_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", LINK_EDITOR_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

    #define LINK_EDITOR_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define LINK_EDITOR_INTERNAL_ASSERT_GET_MACRO(...) LINK_EDITOR_EXPAND_MACRO( LINK_EDITOR_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, LINK_EDITOR_INTERNAL_ASSERT_WITH_MSG, LINK_EDITOR_INTERNAL_ASSERT_NO_MSG) )

    // Currently accepts at least the condition and one additional parameter (the message) being optional
    #define LINK_EDITOR_ASSERT(...) LINK_EDITOR_EXPAND_MACRO( LINK_EDITOR_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
    #define LINK_EDITOR_CORE_ASSERT(...) LINK_EDITOR_EXPAND_MACRO( LINK_EDITOR_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
    #define LINK_EDITOR_ASSERT(...)
    #define LINK_EDITOR_CORE_ASSERT(...)
#endif

#define BIT(x) (1 << x)

#define LINK_EDITOR_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifndef LINK_EDITOR_NAMESPACE_BEGIN
#define LINK_EDITOR_NAMESPACE_BEGIN \
namespace  LinkEditor{
#endif 

#ifndef LINK_EDITOR_NAMESPACE_END
#define LINK_EDITOR_NAMESPACE_END \
}
#endif 