add_rules("mode.debug", "mode.release")

-- 开发期间默认编译debug版本，发布时默认编译release版本
set_defaultmode("debug")
--set_defaultmode("release")
set_xmakever("2.7.1")
set_languages("c++20")

-- 定义项目全局配置、同时会影响add_requires所导入的第三方库配置
if is_host("windows") then
    add_defines("_WIN32_WINNT=0x0A00", "_WIN32","BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST", "BOOST_BIND_GLOBAL_PLACEHOLDERS")
    set_arch("x64")
    set_runtimes("MT")
    -- 设置运行时库
    if is_mode("debug") then
        set_warnings("all")     -- -Wall
        set_symbols("debug")    -- dbg symbols
        add_syslinks("shell32", "ole32")   -- function CoTaskMemFree and SHGetKnownFolderPath
        add_ldflags("/FORCE:MULTIPLE")   -- fix duplicate symbols
        --add_ldflags("/DEFAULTLIB:libcmtd")  -- 覆盖默认link msvcrt.lib
        --add_syslinks("libcmtd") -- MTd 模式需要手动link libcmtd.lib
    else
        set_optimize("fastest") -- faster: -O2  fastest: -Ox  none: -O0
        add_cxflags("/EHsc")
        add_ldflags("/SAFESEH:NO")
    end
elseif is_host("linux") then
    if is_mode("debug") then
    else
        set_optimize("faster")
    end
end

add_requires("coost", { configs = {
    openssl = true,
    libcurl = true,
} })

add_requires("boost", { configs = {
    system = true,
    context = true,
    thread = true,
    filesystem = true,
} })

target("cryptotrader")
    set_kind("binary")
    add_includedirs("vendor/Boost.Application/include", "src")
    add_files("src/*.cpp")
    add_packages("coost", "boost")
target_end()

includes("unitest")